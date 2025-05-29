#pragma once 
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <list>
#include<string>
#include<cstdint>
#include <algorithm>  // 添加标准算法头文件
#include"../../logs/logs.h"
#include "../../selfDomain/SelfDomain.h"

/*
主要是用来实现文件上传功能,同时管理上传的文件，以及对应描述符
*/

struct UploadFile{
    std::string filePath; // 文件路径
    std::fstream filedesc; // 文件描述符
    struct FileInfo fileinfo; // 文件信息
};
//完整文件块，包含二进制数据以及文件元信息，用于传输

class Uploader {
private:
    // 文件列表，用来存储上传的文件信息，包括文件名，文件路径，文件大小，文件描述符
    std::vector<UploadFile> filelist;
    
public:
    Uploader() {};
    ~Uploader();
    //添加要上传的文件信息
    bool addFile(const std::string& filepath);
    bool addFile(const std::vector<std::string>& filepaths);
    //返回要下载的文件块序列化信息
    std::vector<char> getFileBlock(const std::string& filename, uint32_t blockId);
    //返回可下载的文件列表
    std::vector<struct FileInfo> getFileList();
    //删除文件信息
    bool delFile(const std::string& filename);

};