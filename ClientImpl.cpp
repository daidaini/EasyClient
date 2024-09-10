#include "ClinetInterface.h"
#include "AsioClient.h"

void *HDCreateClient(const char *ipAddr, OnConnectionFuncType connCb, OnMsgFuncType msgCb)
{
    HD::AsioClient *client = new HD::AsioClient(ipAddr, connCb, msgCb);

    client->Run();

    return (void *)client;
}

void HDDeleteCleint(void *client)
{
    if (client != nullptr)
    {
        HD::AsioClient *asioCli = (HD::AsioClient *)client;

        asioCli->Close();

        delete asioCli;
    }
}

void HDSendMsg(void *client, const char *msg, size_t msglen)
{
    if (client == nullptr)
        return;

    HD::AsioClient *asioCli = (HD::AsioClient *)client;

    asioCli->SendMsg(std::string(msg, msglen));
}