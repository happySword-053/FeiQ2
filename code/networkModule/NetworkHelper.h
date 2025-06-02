#pragma once
#include <Windows.h>
#include<string>
#include<vector>
#include"../systemInfoHelper/AdapterInfo.h"

class NetworkHelper {
    private:    
        WSADATA wsaData;         // 存储Sockets初始化信息的结构体
        AdapterInfo adapterInfo; // 适配器信息类
        bool isInitialized;      // 标记是否初始化成功
        NetworkHelper() : isInitialized(false) {
            // 初始化操作在init()方法中完成
        }
    
        ~NetworkHelper() {
            // 基础清理操作
            if (isInitialized) {
                WSACleanup();  // 清理Sockets环境
            }
            // 成员清理操作由AdapterInfo类的析构函数处理
        }
        // 初始化网络助手类
        int init() ;
    public:
        //获取单例
        static NetworkHelper& getInstance() {
            static NetworkHelper instance;
            return instance;
        }
    
        // 获取适配器信息
        const AdapterInfo& getAdapterInfo() const {
            return adapterInfo;
        }
    
        // 其他网络辅助功能可以在这里添加...
};