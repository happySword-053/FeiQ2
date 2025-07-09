#pragma once
#include <string>
#include <algorithm>
#include"../../task/CurrentTask.h"
//无需要控制访问权限的变量，只提供下载方法
class Downloader {
public:
    std::vector<char> download(const char* filename, int blockId = 0);
};