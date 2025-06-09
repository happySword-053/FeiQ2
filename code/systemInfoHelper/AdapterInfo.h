#pragma once
// 定义宏：减少Windows头文件冗余包含（移除不常用的API）
#define WIN32_LEAN_AND_MEAN
// // 定义Windows版本（最低支持Vista及以上系统）
// #define _WIN32_WINNT 0x0600

#include "APIHelper.h"
#include"../logs/logs.h"
// Windows网络编程核心头文件
#include <winsock2.h>     // Windows Sockets API（网络基础）
#include <ws2tcpip.h>    // TCP/IP协议相关函数（如InetNtopA）
#include <iphlpapi.h>    // IP Helper API（获取网络适配器信息）
#include <netioapi.h>    // 网络I/O扩展API（如GetIfEntry2）
#include <windows.h>     // Windows基础API
#include <stdio.h>       // 标准输入输出（兼容旧代码）
#include <stdlib.h>      // 内存管理（如malloc/free）
#include <vector>        // 动态数组容器
#include <string>        // 字符串处理
#include <iostream>      // C++标准输入输出（用户要求替换printf的核心）
#include <sstream>       // 字符串流（用于序列化）
// 显式链接Windows网络库（编译时自动链接）
#pragma comment(lib, "iphlpapi.lib")  // IP Helper库（GetAdaptersAddresses等）
#pragma comment(lib, "ws2_32.lib")    // Windows Sockets库（WSAStartup等）
// 使用标准命名空间（简化代码）


// 常量定义：IP地址字符串最大长度
#define MAX_ADDR_LEN 128
struct Adapter {  // 适配器信息结构体（定义在类内部）

    std::string operStatus;  // 媒体状态（网络是否连接）
    std::string typeStr;     // 网络类型（以太网/无线等）
    std::string friendName;  // 适配器名称（如"本地连接"）
    std::string ipv4;        // IPv4地址
    std::string defaultGateway;  // 默认网关
    std::string macAddress;      // 新增：物理地址（MAC地址）

    // 序列化方法（更新）
    // 序列化方法（修改为 vector<char> 载体）
    std::vector<char> serialize() const {
        std::ostringstream oss;
        oss << operStatus << "|"
            << typeStr << "|"
            << friendName << "|"
            << ipv4 << "|"
            << defaultGateway << "|"
            << macAddress;  // 保持原字符串拼接逻辑
        const std::string str = oss.str();  // 先生成中间字符串
        return std::vector<char>(str.begin(), str.end());  // 转换为 vector<char>
    }

    // 反序列化方法（输入改为 vector<char>）
    static Adapter deserialize(const std::vector<char>& data) {
        // 将 vector<char> 转换为 string 用于解析
        std::string str(data.begin(), data.end());
        std::istringstream iss(str);
        Adapter adapter;
        std::getline(iss, adapter.operStatus, '|');
        std::getline(iss, adapter.typeStr, '|');
        std::getline(iss, adapter.friendName, '|');
        std::getline(iss, adapter.ipv4, '|');
        std::getline(iss, adapter.defaultGateway, '|');
        std::getline(iss, adapter.macAddress, '|');  // 字段顺序与序列化一致
        return adapter;
    }
};
class AdapterInfo {
private:
    std::vector<Adapter> adapters; 
    PIP_ADAPTER_ADDRESSES pAdapterAddresses;  // 适配器信息结构体指针
    ULONG outBufLen = 0;  // 存储所需缓冲区大小（字节）
    Adapter now_adapter;  // 适配器信息结构体
public:
    int init();
    AdapterInfo() { // 初始化适配器信息链表
        // WSADATA wsaData;          // Windows Sockets数据结构
        // WSAStartup(MAKEWORD(2, 2), &wsaData);  // 初始化Sockets环境
        init();
        updateAdapters();
        this->now_adapter = getNowAdapterOnline();
    }
    ~AdapterInfo() {  // 析构函数：释放内存
        if(pAdapterAddresses){
            free(pAdapterAddresses);  // 释放适配器信息链表内存
            pAdapterAddresses = nullptr;  // 置空指针
        }
        // WSACleanup();             // 清理Sockets环境
    }
    int updateAdapters(); //操作adapters数组，更新最新的适配器信息
    std::vector<Adapter> getAdapters() {  // 获取适配器信息列表
        return adapters;  // 返回适配器信息向量
    }
    Adapter getNowAdapterOnline();//获取第一个当前连上网的适配器信息
    Adapter getAdapterByIndex(int index);//根据索引获取适配器信息
    Adapter getNowAdapter()const{return now_adapter;}//获取当前适配器信息
};
