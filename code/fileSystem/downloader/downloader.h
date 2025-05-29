#pragma once
#include <string>
#include <algorithm>
//无需要控制访问权限的变量，只提供下载方法
class Downloader {
public:
    static std::string download(const std::string& url);
};