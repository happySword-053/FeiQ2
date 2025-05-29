#include"NetworkHelper.h"

int NetworkHelper::init()
{
    // -------------------- 1. 初始化Windows Sockets环境 --------------------
    WSADATA wsaData;  // 存储Sockets初始化信息的结构体
    // 初始化Sockets库（版本2.2）
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");  // 初始化失败时输出错误
        return -1;  // 返回错误码
    }
}


