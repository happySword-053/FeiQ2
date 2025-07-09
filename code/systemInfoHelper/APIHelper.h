#pragma once
#include <windows.h>
#include<string>
#include<vector>
#include <stdexcept>
#include<sstream>
#include <iomanip>

// #include"userLocalInfo.h"
// #include"AdapterInfo.h"
#include<cstdint>
class APIHelper{
public:
    static std::string WCharToString(const wchar_t *wstr);//宽字符转多字节字符串
    static std::string calculate_broadcast(const std::string& cidr);//计算广播地址
    
private:
    static std::string ip_to_string(uint32_t ip);  //将IP地址转换为字符串
    static uint32_t parse_ipv4(const std::string& ip_str);//解析IPv4地址
    static uint64_t macToUint64(const std::string& macStr);//将MAC地址转换为uint64_t
    static std::string uint64ToMacString(uint64_t mac);//将uint64_t转换为MAC地址字符串
};

// struct UserInfoAndNetworkInfo{
//     UserInfo userInfo;
//     Adapter adapter;
// };