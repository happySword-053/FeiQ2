// 定义宏：减少Windows头文件冗余包含（移除不常用的API）
#define WIN32_LEAN_AND_MEAN
// 定义Windows版本（最低支持Vista及以上系统）
#define _WIN32_WINNT 0x0600

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
// 显式链接Windows网络库（编译时自动链接）
#pragma comment(lib, "iphlpapi.lib")  // IP Helper库（GetAdaptersAddresses等）
#pragma comment(lib, "ws2_32.lib")    // Windows Sockets库（WSAStartup等）
// 使用标准命名空间（简化代码）
using namespace std;

// 常量定义：IP地址字符串最大长度
#define MAX_ADDR_LEN 128

/**
 * @brief 辅助函数：将宽字符（wchar_t*）转换为多字节字符串（std::string）
 * @param wstr 输入的宽字符指针（Windows API返回的字符串通常为宽字符）
 * @return 转换后的多字节字符串（使用系统默认ANSI代码页）
 */
static std::string WCharToString(const wchar_t* wstr) {
    if (!wstr) return {};  // 空指针直接返回空字符串
    // 计算转换所需的字节数（含终止符）
    int size_needed = WideCharToMultiByte(
        CP_ACP,          // 使用系统默认ANSI代码页（如GBK）
        0,               // 转换标志（无特殊处理）
        wstr,            // 输入宽字符指针
        -1,              // 自动计算字符串长度（含终止符）
        nullptr,         // 输出缓冲区（先获取长度）
        0,               // 缓冲区大小（0表示仅获取长度）
        nullptr,         // 未转换字符的默认替代（不处理）
        nullptr          // 是否使用默认替代字符（不处理）
    );
    // 分配足够大小的缓冲区
    std::vector<char> buf(size_needed);
    // 实际执行宽字符到多字节的转换
    WideCharToMultiByte(
        CP_ACP,          // 代码页同上
        0,               // 转换标志同上
        wstr,            // 输入宽字符指针
        -1,              // 自动计算长度
        buf.data(),      // 输出缓冲区
        size_needed,     // 缓冲区大小
        nullptr,         // 未转换字符的默认替代
        nullptr          // 是否使用默认替代字符
    );
    return std::string(buf.data());  // 转换为std::string返回
}

/**
 * @brief 打印单个网络适配器的详细信息
 * @param adapter 指向网络适配器信息结构体的指针（由GetAdaptersAddresses获取）
 */
void printAdapterInfo(PIP_ADAPTER_ADDRESSES adapter) {
    // -------------------- 1. 获取接口别名（Windows网络连接名称） --------------------
    MIB_IF_ROW2 ifRow = { 0 };  // 存储接口详细信息的结构体（需iphlpapi.h）
    ifRow.InterfaceLuid = adapter->Luid;  // 通过适配器的LUID（唯一标识符）定位接口
    // 调用API获取接口详细信息（失败则清空别名）
    if (GetIfEntry2(&ifRow) != NO_ERROR) {
        ifRow.Alias[0] = L'\0';  // 设置别名为空字符串
    }

    // -------------------- 2. 解析媒体状态（网络是否连接） --------------------
    const char* operStatus;  // 媒体状态描述字符串
    switch (adapter->OperStatus) {  // 适配器操作状态（枚举类型）
        case IfOperStatusUp:   operStatus = "已连接"; break;       // 网络已连接
        case IfOperStatusDown: operStatus = "媒体已断开连接"; break; // 网络断开
        default:               operStatus = "未知状态";    break;   // 其他状态
    }

    // -------------------- 3. 解析网络类型（以太网/无线等） --------------------
    const char* typeStr;  // 网络类型描述字符串
    switch (adapter->IfType) {  // 适配器类型（枚举类型）
        case IF_TYPE_ETHERNET_CSMACD: typeStr = "以太网";        break;  // 有线以太网
        case IF_TYPE_IEEE80211:       typeStr = "无线局域网";    break;  // Wi-Fi
        default:                      typeStr = "其他网络适配器"; break;  // 其他类型
    }

    // -------------------- 4. 确定连接名称（优先使用接口别名） --------------------
    // 优先级：接口别名 > 友好名称 > 描述信息（均为宽字符）
    const wchar_t* connName = (ifRow.Alias[0] != L'\0' ? ifRow.Alias  // 接口别名非空
        : (adapter->FriendlyName && adapter->FriendlyName[0] != L'\0'  // 友好名称非空
            ? adapter->FriendlyName 
            : adapter->Description));  // 最终使用描述信息
    // 转换宽字符名称为多字节字符串（用于cout输出）
    std::string temName = WCharToString(connName);
    // 输出适配器名称和类型（使用C++标准输出）
    std::cout << "\n[" << temName << "]  类型" << typeStr << std::endl;

    // -------------------- 5. 输出媒体状态（网络连接状态） --------------------
    cout << "   媒体状态............ : " << operStatus << endl;  

    // -------------------- 6. 输出连接特定的DNS后缀 --------------------
    // 处理宽字符DNS后缀（可能为空），转换后输出
    cout << "   连接特定的 DNS 后缀..... : " 
         << (adapter->DnsSuffix && adapter->DnsSuffix[0] ? WCharToString(adapter->DnsSuffix) : "(无)") 
         << endl;

    // -------------------- 7. 输出MAC地址（物理地址） --------------------
    cout << "   物理地址............ : ";
    // 遍历物理地址字节数组（每个字节为0-255的无符号字符）
    for (UINT i = 0; i < adapter->PhysicalAddressLength; ++i) {
        // 格式化输出两位十六进制（补零），并添加分隔符（如AA-BB-CC-DD-EE-FF）
        printf("%02X%s",  // 使用printf保持十六进制格式化（C++标准输出需额外头文件）
            adapter->PhysicalAddress[i],  // 当前字节
            (i + 1 < adapter->PhysicalAddressLength) ? "-" : "");  // 分隔符
    }
    printf("\n");  // 换行（与cout混用，保持格式统一）

    // -------------------- 8. 输出IPv4地址、前缀长度及默认网关 --------------------
    bool hasIpv4 = false;  // 标记是否存在IPv4地址
    // 遍历单播地址链表（adapter->FirstUnicastAddress为链表头）
    for (auto* ua = adapter->FirstUnicastAddress; ua; ua = ua->Next) {
        if (ua->Address.lpSockaddr->sa_family == AF_INET) {  // 检查是否为IPv4地址
            char ipStr[MAX_ADDR_LEN] = { 0 };  // 存储IPv4地址字符串
            // 将sockaddr_in中的二进制IP转换为点分十进制字符串（如192.168.1.1）
            auto* sin = reinterpret_cast<sockaddr_in*>(ua->Address.lpSockaddr);
            InetNtopA(AF_INET, &sin->sin_addr, ipStr, sizeof(ipStr));
            // 输出IPv4地址及前缀长度（如192.168.1.1/24）
            printf("   IPv4 地址 . . . . . . . . . . . . : %s/%u\n", ipStr, ua->OnLinkPrefixLength);
            hasIpv4 = true;  // 标记存在IPv4地址
        }
    }
    if (hasIpv4) {  // 存在IPv4地址时输出默认网关
        if (adapter->FirstGatewayAddress) {  // 检查是否有网关地址
            char gwStr[MAX_ADDR_LEN] = { 0 };  // 存储网关地址字符串
            // 将sockaddr_in中的二进制网关地址转换为点分十进制字符串
            auto* gw = reinterpret_cast<sockaddr_in*>(adapter->FirstGatewayAddress->Address.lpSockaddr);
            InetNtopA(AF_INET, &gw->sin_addr, gwStr, sizeof(gwStr));
            printf("   默认网关. . . . . . . . . . . . . : %s\n", gwStr);
        } else {
            printf("   默认网关. . . . . . . . . . . . . : (无)\n");
        }
    }

    // -------------------- 9. 输出IPv6地址及前缀长度 --------------------
    // 再次遍历单播地址链表（查找IPv6地址）
    for (auto* ua = adapter->FirstUnicastAddress; ua; ua = ua->Next) {
        if (ua->Address.lpSockaddr->sa_family == AF_INET6) {  // 检查是否为IPv6地址
            char ipStr[MAX_ADDR_LEN] = { 0 };  // 存储IPv6地址字符串
            // 将sockaddr_in6中的二进制IPv6地址转换为字符串（如2001:db8::1）
            auto* sin6 = reinterpret_cast<sockaddr_in6*>(ua->Address.lpSockaddr);
            InetNtopA(AF_INET6, &sin6->sin6_addr, ipStr, sizeof(ipStr));
            // 输出IPv6地址及前缀长度（如2001:db8::1/64）
            printf("   本地链接 IPv6 地址. . . . . . . . : %s/%u\n", ipStr, ua->OnLinkPrefixLength);
        }
    }
}

/**
 * @brief 主函数：程序入口
 * @return 0表示正常退出，非0表示错误
 */
int main() {
    // -------------------- 1. 初始化Windows Sockets环境 --------------------
    WSADATA wsaData;  // 存储Sockets初始化信息的结构体
    // 初始化Sockets库（版本2.2）
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");  // 初始化失败时输出错误
        return 1;  // 返回错误码
    }

    // -------------------- 2. 获取网络适配器信息（第一阶段：获取缓冲区大小） --------------------
    ULONG outBufLen = 0;  // 存储所需缓冲区大小（字节）
    // 首次调用GetAdaptersAddresses获取所需缓冲区大小（会返回ERROR_BUFFER_OVERFLOW）
    DWORD dwRetVal = GetAdaptersAddresses(
        AF_UNSPEC,        // 不指定地址族（获取所有IPv4/IPv6适配器）
        GAA_FLAG_INCLUDE_PREFIX,  // 包含前缀长度信息（用于子网掩码）
        NULL,             // 保留参数（设为NULL）
        NULL,             // 输出缓冲区（首次调用为NULL）
        &outBufLen        // 输出所需缓冲区大小
    );
    if (dwRetVal != ERROR_BUFFER_OVERFLOW) {  // 检查是否返回预期错误码
        printf("GetAdaptersAddresses failed with error: %u\n", dwRetVal);
        WSACleanup();  // 清理Sockets环境
        return 1;      // 返回错误码
    }

    // -------------------- 3. 分配内存并获取适配器信息（第二阶段：实际获取数据） --------------------
    // 分配指定大小的内存（存储适配器信息链表）
    PIP_ADAPTER_ADDRESSES pAdapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufLen);
    if (!pAdapterAddresses) {  // 内存分配失败处理
        printf("Memory allocation failed.\n");
        WSACleanup();  // 清理Sockets环境
        return 1;      // 返回错误码
    }

    // 再次调用GetAdaptersAddresses获取实际适配器信息
    dwRetVal = GetAdaptersAddresses(
        AF_UNSPEC,        // 地址族同上
        GAA_FLAG_INCLUDE_PREFIX,  // 包含前缀长度
        NULL,             // 保留参数
        pAdapterAddresses,  // 输出缓冲区（已分配内存）
        &outBufLen        // 输出实际使用的缓冲区大小（可能被更新）
    );
    if (dwRetVal != NO_ERROR) {  // 检查是否成功获取数据
        printf("GetAdaptersAddresses failed with error: %u\n", dwRetVal);
        free(pAdapterAddresses);  // 释放已分配的内存
        WSACleanup();             // 清理Sockets环境
        return 1;                 // 返回错误码
    }

    // -------------------- 4. 遍历并打印所有网络适配器信息 --------------------
    for (auto* adapter = pAdapterAddresses; adapter; adapter = adapter->Next) {
        printAdapterInfo(adapter);  // 调用打印函数
    }

    // -------------------- 5. 释放资源并退出 --------------------
    free(pAdapterAddresses);  // 释放适配器信息内存
    WSACleanup();             // 清理Windows Sockets环境
    return 0;                 // 正常退出
}
