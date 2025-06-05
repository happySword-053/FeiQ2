#include"AdapterInfo.h"
int AdapterInfo::init()
{
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
        
        return -1;      // 返回错误码
    }
    // -------------------- 分配内存并获取适配器信息（第二阶段：实际获取数据） --------------------
    // 分配指定大小的内存（存储适配器信息链表）
    pAdapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufLen);
    if (!pAdapterAddresses) {  // 内存分配失败处理
        printf("Memory allocation failed.\n");
       
        return -1;      // 返回错误码
    }
    return 0;
}

int AdapterInfo::updateAdapters()
{
    adapters.clear();//清空adapters数组，防止重复添加
    // 再次调用GetAdaptersAddresses获取实际适配器信息
    DWORD dwRetVal = GetAdaptersAddresses(
        AF_UNSPEC,        // 地址族同上
        GAA_FLAG_INCLUDE_PREFIX,  // 包含前缀长度
        NULL,             // 保留参数
        pAdapterAddresses,  // 输出缓冲区（已分配内存）
        &outBufLen        // 输出实际使用的缓冲区大小（可能被更新）
    );
    if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
        // 重新分配更大的缓冲区
        free(pAdapterAddresses);
        pAdapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufLen);
        if (!pAdapterAddresses) { /* 错误处理 */ }
        dwRetVal = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, pAdapterAddresses, &outBufLen);  // 再次尝试
    }
    if (dwRetVal != NO_ERROR) {  // 检查是否成功获取数据
        printf("GetAdaptersAddresses failed with error: %u\n", dwRetVal);
        free(pAdapterAddresses);  // 释放已分配的内存
        return -1;                 // 返回错误码
    }
    for (auto* adapter = pAdapterAddresses; adapter; adapter = adapter->Next) {
        Adapter adp;
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
                : adapter->Description));  // 最终使用描述信
        // 转换宽字符名称为多字节字符串
        std::string temName = APIHelper::WCharToString(connName);
        adp.friendName = temName;  // 存储连接名称
        adp.typeStr = typeStr;  // 存储网络类型

        // -------------------- 5. 媒体状态（网络连接状态） --------------------
        // cout << "   媒体状态............ : " << operStatus << endl;  
        adp.operStatus = std::string(operStatus);


        // -------------------- 6. 输出连接特定的DNS后缀 --------------------
        // 处理宽字符DNS后缀（可能为空），转换后输出
        // cout << "   连接特定的 DNS 后缀..... : " 
        //     << (adapter->DnsSuffix && adapter->DnsSuffix[0] ? WCharToString(adapter->DnsSuffix) : "(无)") 
        //     << endl;

        // -------------------- 7. 输出MAC地址（物理地址） --------------------
        
        // 遍历物理地址字节数组（每个字节为0-255的无符号字符）
        std::string macStr;
        for (UINT i = 0; i < adapter->PhysicalAddressLength; ++i) {
            char byteBuf[3];  // 存储单个字节的十六进制（如 "AA"）
            snprintf(byteBuf, sizeof(byteBuf), "%02X", adapter->PhysicalAddress[i]);
            macStr += byteBuf;
            // 添加分隔符（最后一个字节不加）
            if (i != adapter->PhysicalAddressLength - 1) {
                macStr += "-";
            }
        }
        adp.macAddress = macStr.empty() ? "(无)" : macStr;  // 无MAC地址时标记为"(无)"
        

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
                //printf("   IPv4 地址 . . . . . . . . . . . . : %s/%u\n", ipStr, ua->OnLinkPrefixLength);
                adp.ipv4 = std::string(ipStr) + "/" + std::to_string(ua->OnLinkPrefixLength);
                hasIpv4 = true;  // 标记存在IPv4地址
            }
        }
        if (hasIpv4) {  // 存在IPv4地址时输出默认网关
            if (adapter->FirstGatewayAddress) {  // 检查是否有网关地址
                char gwStr[MAX_ADDR_LEN] = { 0 };  // 存储网关地址字符串
                // 将sockaddr_in中的二进制网关地址转换为点分十进制字符串
                auto* gw = reinterpret_cast<sockaddr_in*>(adapter->FirstGatewayAddress->Address.lpSockaddr);
                InetNtopA(AF_INET, &gw->sin_addr, gwStr, sizeof(gwStr));
                //printf("   默认网关. . . . . . . . . . . . . : %s\n", gwStr);
                adp.defaultGateway = std::string(gwStr);
            } else {
                //printf("   默认网关. . . . . . . . . . . . . : (无)\n");
                adp.defaultGateway = std::string("(无)");
            }
        }
        adapters.push_back(adp);
        // // -------------------- 9. 输出IPv6地址及前缀长度 --------------------
        // // 再次遍历单播地址链表（查找IPv6地址）
        // for (auto* ua = adapter->FirstUnicastAddress; ua; ua = ua->Next) {
        //     if (ua->Address.lpSockaddr->sa_family == AF_INET6) {  // 检查是否为IPv6地址
        //         char ipStr[MAX_ADDR_LEN] = { 0 };  // 存储IPv6地址字符串
        //         // 将sockaddr_in6中的二进制IPv6地址转换为字符串（如2001:db8::1）
        //         auto* sin6 = reinterpret_cast<sockaddr_in6*>(ua->Address.lpSockaddr);
        //         InetNtopA(AF_INET6, &sin6->sin6_addr, ipStr, sizeof(ipStr));
        //         // 输出IPv6地址及前缀长度（如2001:db8::1/64）
        //         printf("   本地链接 IPv6 地址. . . . . . . . : %s/%u\n", ipStr, ua->OnLinkPrefixLength);
        //     }
        // }
    }
}

Adapter AdapterInfo::getNowAdapterOnline()
{
    Adapter adp;
    
        // 2. 获取 IPv4 适配器地址
        ULONG family = AF_INET;
        ULONG flags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS;
        ULONG outBufLen = 0;
        DWORD ret;
        std::vector<BYTE> buffer;
    
        do {
            if (outBufLen == 0) outBufLen = 15000;
            buffer.resize(outBufLen);
            ret = GetAdaptersAddresses(
                family, flags, nullptr,
                reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data()),
                &outBufLen
            );
        } while (ret == ERROR_BUFFER_OVERFLOW);
    
        if (ret != NO_ERROR) {
            std::cerr << "GetAdaptersAddresses failed: " << ret << "\n";
            throw std::runtime_error("GetAdaptersAddresses failed");
        }
    
        auto* addrs = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
        // 3. 遍历输出
        for (auto* curr = addrs; curr; curr = curr->Next) {
            if (curr->OperStatus != IfOperStatusUp || !curr->FirstGatewayAddress)
                continue;
            // 排除回环
            if (curr->IfType == IF_TYPE_SOFTWARE_LOOPBACK)
                continue;
            // 打印第一个 IPv4 + 前缀
            for (auto* ua = curr->FirstUnicastAddress; ua; ua = ua->Next) {
                if (ua->Address.lpSockaddr->sa_family == AF_INET) {
                    char ip[INET_ADDRSTRLEN] = { 0 };
                    auto* sin = reinterpret_cast<sockaddr_in*>(ua->Address.lpSockaddr);
                    InetNtopA(AF_INET, &sin->sin_addr, ip, sizeof(ip));
    
                    // // 这里先打印 IP，再打印“\”和前缀长度
                    // std::cout << "  IP 地址: "
                    //     << ip
                    //     << "\\"
                    //     << static_cast<unsigned>(ua->OnLinkPrefixLength)
                    //     << "\n";
                    adp.ipv4 = std::string(ip) + "/" + std::to_string(ua->OnLinkPrefixLength);

                    break;  // 打印后再跳出
                }
            }
    
            // 打印默认网关
            auto* ga = curr->FirstGatewayAddress;
            if (ga && ga->Address.lpSockaddr->sa_family == AF_INET) {
                char gw[INET_ADDRSTRLEN] = { 0 };
                auto* sin = reinterpret_cast<sockaddr_in*>(ga->Address.lpSockaddr);
                InetNtopA(AF_INET, &sin->sin_addr, gw, sizeof(gw));
                //std::cout << "  网关: " << gw << "\n";
                adp.defaultGateway = std::string(gw);
            }
    
            //std::cout << "-----------------------------\n";
        }
    
        return adp;
}
