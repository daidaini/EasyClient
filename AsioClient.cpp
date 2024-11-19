#include "AsioClient.h"
#include "StringFunc.h"

namespace HD
{
    AsioClient::AsioClient(const std::string &ipAddr, OnConnectionFuncType connCb, OnMsgFuncType cb, bool needReconnect)
        : m_NeedReConnect(needReconnect)
    {
        auto items = str::Split(ipAddr, ':');
        if (items.size() < 2)
        {
            throw std::runtime_error("Error param for AsioClient construct");
        }
        m_Host = items.at(0);
        m_Port = items.at(1);

        m_ConnCb = connCb;
        m_Cb = cb;
        m_ReadBuffer = new char[MAX_BUFFER_SIZE]{};

        m_Resolver = std::unique_ptr<boost::asio::ip::tcp::resolver>(new boost::asio::ip::tcp::resolver(m_IoContext));
    }

    AsioClient::~AsioClient()
    {
        if (m_IoThread.joinable())
        {
            m_IoThread.join();
        }
        delete m_ReadBuffer;
    }

    void AsioClient::Run()
    {
        Connect();

        m_IoThread = std::thread(
            [this]()
            {
                m_IoContext.run();
            });
    }

    void AsioClient::Connect()
    {
        m_Socket = std::unique_ptr<boost::asio::ip::tcp::socket>(new boost::asio::ip::tcp::socket(m_IoContext));

        auto endpoints = m_Resolver->resolve(m_Host, m_Port);
        if (endpoints.empty())
        {
            printf("connect failed..\n");
            this->m_ConnCb(ErrorCode::Fail);
            return;
        }

        boost::asio::async_connect(
            *m_Socket, endpoints,
            [this](boost::system::error_code ec, boost::asio::ip::tcp::tcp::endpoint)
            {
                if (this == nullptr)
                {
                    return;
                }
                if (!ec)
                {
                    m_Connected = true;
                    this->m_ConnCb(ErrorCode::Succcess);
                    this->m_WriteMsgs.clear();
                    DoRead();
                    StartRepeatTimer();
                }
                else
                {
                    m_Connected = false;
                    this->m_ConnCb(ErrorCode::Fail);
                }
            });
    }

    void AsioClient::Close()
    {
        m_NeedReConnect.store(false);

        m_IoContext.post(
            [this]()
            {
                this->m_Socket->close();
            });
    }

    void AsioClient::DoRead()
    {
        if (!m_Connected)
        {
            printf("wait to retry\n");
            return;
        }
        m_Socket->async_read_some(
            boost::asio::buffer(m_ReadBuffer, MAX_BUFFER_SIZE),
            [this](boost::system::error_code ec, std::size_t length)
            {
                if (this == nullptr)
                    return;
                if (!ec)
                {
                    this->m_Cb(ErrorCode::Succcess, m_ReadBuffer, length);

                    DoRead(); // 继续读取下一条消息
                }
                else
                {
                    this->m_Cb(ErrorCode::Fail, ec.message().c_str(), ec.message().size());

                    // 断开连接
                    m_Connected = false;
                    if (this->m_Socket != nullptr)
                    {
                        this->m_Socket->close();
                    }
                }
            });
    }

    void AsioClient::SendMsg(const std::string &message)
    {
        m_IoContext.post(
            [this, msg = message]()
            {
                bool writing = !m_WriteMsgs.empty();
                m_WriteMsgs.emplace_back(std::move(msg));
                if (!writing)
                {
                    DoWrite();
                }
            });
    }

    void AsioClient::DoWrite()
    {
        if (!m_Connected)
        {
            printf("wait to retry\n");
            return;
        }
        boost::asio::async_write(
            *m_Socket, boost::asio::buffer(m_WriteMsgs.front()),
            [this](boost::system::error_code ec, std::size_t length)
            {
                if (this == nullptr)
                    return;
                if (!ec)
                {
                    m_WriteMsgs.pop_front();
                    if (!m_WriteMsgs.empty())
                    {
                        this->DoWrite();
                    }
                }
                else
                {
                    this->m_Cb(ErrorCode::Fail, ec.message().c_str(), ec.message().size());
                    m_Connected = false;
                    if (this->m_Socket != nullptr)
                    {
                        this->m_Socket->close();
                    }
                }
            });
    }

    void AsioClient::StartRepeatTimer()
    {
        m_RepeatTimer = std::unique_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(m_IoContext));

        DoRepeatTimer();
    }

    void AsioClient::DoRepeatTimer()
    {
        int seconds = 10;
        m_RepeatTimer->expires_from_now(boost::posix_time::seconds(seconds));
        m_RepeatTimer->async_wait(
            [this, seconds](const boost::system::error_code &ec)
            {
                if (this == nullptr || !this->m_NeedReConnect)
                {
                    return;
                }
                if (!ec)
                {
                    if (m_Socket != nullptr && !m_Connected)
                    {
                        // 重新连接
                        this->Connect();
                    }

                    DoRepeatTimer();
                }
            });
    }
}