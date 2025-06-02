#pragma once
#include <windows.h>
#include<string>
#include<vector>
#include"userLocalInfo.h"
#include"AdapterInfo.h"
class APIHelper{
public:
    static std::string WCharToString(const wchar_t *wstr);//宽字符转多字节字符串
};

struct UserInfoAndNetworkInfo{
    UserInfo userInfo;
    Adapter adapter;
};