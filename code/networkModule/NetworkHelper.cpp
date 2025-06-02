#include"NetworkHelper.h"

int NetworkHelper::init()
{
    int result = 0;
    try{
        // 检查是否已经初始化
        if (isInitialized) {
            return 0; // 已经初始化，直接返回成功
        }

        // 初始化Winsock库
        result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            // 记录错误日志或进行其他处理
            throw std::runtime_error("初始化Winsock库失败");
        }

        // 检查版本是否支持
        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
            WSACleanup();
            return SOCKET_ERROR;
        }
    }catch(const std::exception& e){
        LOG("初始化网络助手失败" + std::to_string(result),ERROR);
        throw e;
    }
    // 标记初始化成功
    isInitialized = true;
    return 0; // 成功返回0
}


