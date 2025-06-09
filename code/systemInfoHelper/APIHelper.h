#pragma once
#include <windows.h>
#include<string>
#include<vector>
#include <stdexcept>
#include<sstream>
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
};

// struct UserInfoAndNetworkInfo{
//     UserInfo userInfo;
//     Adapter adapter;
// };