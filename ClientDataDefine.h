#pragma once

#include <stdio.h>

#define MAX_BUFFER_SIZE 64 * 1024

enum ErrorCode
{
    Succcess,
    Fail,
};

typedef void (*OnConnectionFuncType)(ErrorCode code);

/**
 * @brief 回调函数，用以返回数据消息
 */
typedef void (*OnMsgFuncType)(ErrorCode code, const char *msg, size_t msglen);
