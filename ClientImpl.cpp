#include "ClinetInterface.h"
#include "AsioClient.h"

void *HDCreateClient(const char *ipAddr, OnConnectionFuncType connCb, OnMsgFuncType msgCb, bool needReconnect)
{
    HD::AsioClient *client = new HD::AsioClient(ipAddr, connCb, msgCb, needReconnect);

    client->Run();

    return (void *)client;
}

void HDCloseClient(void *client)
{
    if (client == nullptr)
        return;

    HD::AsioClient *asioCli = (HD::AsioClient *)client;
    asioCli->Close();
}

void HDDeleteCleint(void *client)
{
    if (client == nullptr)
        return;

    delete ((HD::AsioClient *)client);
}

void HDSendMsg(void *client, const char *msg, size_t msglen)
{
    if (client == nullptr)
        return;

    HD::AsioClient *asioCli = (HD::AsioClient *)client;

    asioCli->SendMsg(std::string(msg, msglen));
}