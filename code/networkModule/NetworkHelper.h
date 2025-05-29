#pragma once
#include <Windows.h>
#include<string>
#include<vector>
#include"../systemInfoHelper/AdapterInfo.h"
class NetworkHelper {
private:    
    WSADATA wsaData;// 存储Sockets初始化信息的结构体
    AdapterInfo adapterInfo;//适配器信息类
public:
    NetworkHelper();
    ~NetworkHelper(){
        //基础清理操作
        WSACleanup();  // 清理Sockets环境
        //成员清理操作。。。。。。。。。。
    }
    int init();
    
};