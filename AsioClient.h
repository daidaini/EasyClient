#pragma once

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <thread>
#include <deque>
#include "ClientDataDefine.h"
#include <atomic>

namespace HD
{
    class AsioClient
    {
    public:
        AsioClient(const std::string &ipAddr, OnConnectionFuncType connCb, OnMsgFuncType cb, bool needReconnect = false);
        ~AsioClient();

        void Run();

        void Close();

        void SendMsg(const std::string &message);

    private:
        void Connect();

        void DoRead();

        void DoWrite();

        void StartRepeatTimer();

        void DoRepeatTimer();

    private:
        std::string m_Host;
        std::string m_Port;

        boost::asio::io_context m_IoContext;
        std::unique_ptr<boost::asio::ip::tcp::socket> m_Socket;
        std::unique_ptr<boost::asio::ip::tcp::resolver> m_Resolver;

        OnConnectionFuncType m_ConnCb;
        OnMsgFuncType m_Cb;

        bool m_Connected{false};
        std::atomic_bool m_NeedReConnect{true};

        std::thread m_IoThread;

        char *m_ReadBuffer = nullptr;

        std::deque<std::string> m_WriteMsgs;

        std::unique_ptr<boost::asio::deadline_timer> m_RepeatTimer;
    };
}