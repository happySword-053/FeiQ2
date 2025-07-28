#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include<cstdint>
#include"../systemInfoHelper/AdapterInfo.h"
#include"../systemInfoHelper/userLocalInfo.h"
#include"../dataProcess/DataProcess.h"

#define BLOCK_SIZE 16 * 1024 // 16KB块大小
/* 任务类型*/
#define ERROR_TASK -1
#define NON_TASK 0
/*--------文件任务--------*/
#define FILE_LIST_REQUEST 1 // 请求当前可以下载的文件列表
#define FILE_LIST_RETURN 2 // 返回当前自身可下载的文件列表
#define  FILE_DOWNLOAD 3 // 文件下载
#define FILE_BLOCK 4// 当前的文件块
#define FILE_BLOCK_CONFIRM 5 //确认收到文件块

/*----网络任务----*/
#define USERINFO_AND_ADAPTERINFO 58 // 发送用户信息和网卡信息
#define CHAT_MESSAGE 59 // 聊天文字消息
#define UDP_BROADCAST 60 // 广播消息
#define UDP_BROADCAST_CLOSE 61 // 下机，从udp的endpoints中移除本ip
#define TCP_SEND_MESSAGE 62 // 发送消息给tcp模块

// 自定义协议
class Info {
public:
    
    int taskType;
    std::vector<char> data;  // 数据字节流
    /*|taskType | dataLength | data*/
    // 序列化为 vector<char>
    std::vector<char> serialize()  {
        std::vector<char> buffer;
        // 写入 taskType
        auto* typePtr = reinterpret_cast<const char*>(&taskType);
        buffer.insert(buffer.end(), typePtr, typePtr + sizeof(taskType));
        // 压缩和加密
        DataProcess dataProcess;
        data = dataProcess.compress_vector(data);  // 压缩数据
        data = dataProcess.encrypt(data);  // 加密数据
        // 写入 dataLength
        int dataLength = static_cast<int>(data.size());
        auto* lenPtr = reinterpret_cast<const char*>(&dataLength);
        buffer.insert(buffer.end(), lenPtr, lenPtr + sizeof(dataLength));
        // 写入 data
        buffer.insert(buffer.end(), data.begin(), data.end());
        return buffer;
    }

    // 从 vector<char> 反序列化
    static Info deserialize(const std::vector<char>& buffer) {
        Info info;
        size_t offset = 0;
        // 读 taskType
        info.taskType = *reinterpret_cast<const int*>(&buffer[offset]);
        offset += sizeof(info.taskType);
        // 读 dataLength
        int dataLength = *reinterpret_cast<const int*>(&buffer[offset]);
        offset += sizeof(dataLength);
        // 读 data
        info.data.assign(buffer.begin() + offset, buffer.begin() + offset + dataLength);
        // 解密和解压缩
        DataProcess dataProcess;
        info.data = dataProcess.decrypt(info.data);  // 解密数据
        info.data = dataProcess.decompress_vector(info.data);  // 解压数据
        return info;
    }
};
// 表示每个文件的信息，在info类型中用vector存储上传的所有文件信息
struct FileInfo {
    /*|fileNameSize  |  fileName  |  fileSize | userinfosize | userinfo | */
    std::string fileName;
    uint32_t fileSize;
    UserInfo userInfo;
    // 序列化为 vector<char>
    std::vector<char> serialize() const {
        std::vector<char> buffer;
        // 写入文件名长度
        uint32_t nameLen = static_cast<uint32_t>(fileName.size());
        auto* lenPtr = reinterpret_cast<const char*>(&nameLen);
        buffer.insert(buffer.end(), lenPtr, lenPtr + sizeof(nameLen));
        // 写入文件名
        buffer.insert(buffer.end(), fileName.begin(), fileName.end());
        // 写入文件大小
        auto* sizePtr = reinterpret_cast<const char*>(&fileSize);
        buffer.insert(buffer.end(), sizePtr, sizePtr + sizeof(fileSize));
        //序列化userinfo
        std::vector<char> userInfoBuffer = userInfo.serialize();
        // 写入 userInfoSize
        uint32_t userInfoSize = static_cast<uint32_t>(userInfoBuffer.size());
        auto* userInfoSizePtr = reinterpret_cast<const char*>(&userInfoSize);
        buffer.insert(buffer.end(), userInfoSizePtr, userInfoSizePtr + sizeof(userInfoSize));
        // 写入 userInfo
        buffer.insert(buffer.end(), userInfoBuffer.begin(), userInfoBuffer.end());
        return buffer;
    }

    // 从 vector<char> 反序列化
    static FileInfo deserialize(const std::vector<char>& buffer) {
        FileInfo fileInfo;
        size_t offset = 0;
        // 读文件名长度
        uint32_t nameLen = *reinterpret_cast<const uint32_t*>(&buffer[offset]);
        offset += sizeof(nameLen);
        // 读文件名
        fileInfo.fileName.assign(buffer.begin() + offset, buffer.begin() + offset + nameLen);
        offset += nameLen;
        // 读文件大小
        fileInfo.fileSize = *reinterpret_cast<const uint32_t*>(&buffer[offset]);
        offset += sizeof(fileInfo.fileSize);
        // 读 userInfoSize
        uint32_t userInfoSize = *reinterpret_cast<const uint32_t*>(&buffer[offset]);
        offset += sizeof(userInfoSize);
        // 读 userInfo
        std::vector<char> userInfoBuffer(buffer.begin() + offset, buffer.begin() + offset + userInfoSize);
        fileInfo.userInfo = UserInfo::deserialize(userInfoBuffer);
        return fileInfo;
    }
};

// 请求下载的文件块
struct DownloadFileBlock {
    std::string fileName;
    uint32_t blockId;
    /*|fileNameSize  |  fileName  |  blockId*/
    // 序列化为 vector<char>
    std::vector<char> serialize() const {
        std::vector<char> buffer;
        // 写入文件名长度
        uint32_t nameLen = static_cast<uint32_t>(fileName.size());
        auto* lenPtr = reinterpret_cast<const char*>(&nameLen);
        buffer.insert(buffer.end(), lenPtr, lenPtr + sizeof(nameLen));
        // 写入文件名
        buffer.insert(buffer.end(), fileName.begin(), fileName.end());
        // 写入 blockId
        auto* idPtr = reinterpret_cast<const char*>(&blockId);
        buffer.insert(buffer.end(), idPtr, idPtr + sizeof(blockId));
        return buffer;
    }

    // 从 vector<char> 反序列化
    static DownloadFileBlock deserialize(const std::vector<char>& buffer) {
        DownloadFileBlock block;
        size_t offset = 0;
        // 读文件名长度
        uint32_t nameLen = *reinterpret_cast<const uint32_t*>(&buffer[offset]);
        offset += sizeof(nameLen);
        // 读文件名
        block.fileName.assign(buffer.begin() + offset, buffer.begin() + offset + nameLen);
        offset += nameLen;
        // 读 blockId
        block.blockId = *reinterpret_cast<const uint32_t*>(&buffer[offset]);
        offset += sizeof(block.blockId);
        return block;
    }
};

struct FileBlock {
    uint32_t blockId; // 文件块id
    std::vector<char> data; // 文件库实际内容
    /*|blockId | dataLength | data*/
    // 序列化为 vector<char>
    std::vector<char> serialize() const {
        std::vector<char> buffer;
        // 写入 blockId
        auto* blockIdPtr = reinterpret_cast<const char*>(&blockId);
        buffer.insert(buffer.end(), blockIdPtr, blockIdPtr + sizeof(blockId));
        // 写入 dataLength
        int dataLength = static_cast<int>(data.size());
        auto* lenPtr = reinterpret_cast<const char*>(&dataLength);
        buffer.insert(buffer.end(), lenPtr, lenPtr + sizeof(dataLength));
        // 写入 data
        buffer.insert(buffer.end(), data.begin(), data.end());
        return buffer;
    }
    // 反序列化
    static FileBlock deserialize(const std::vector<char>& buffer) {
        FileBlock fileBlock;
        size_t offset = 0;
        // 读blockID
        fileBlock.blockId = *reinterpret_cast<const uint32_t*>(&buffer[offset]);
        offset += sizeof(fileBlock.blockId);
        // 读dataLength
        uint32_t dataLength = *reinterpret_cast<const uint32_t*>(&buffer[offset]);
        offset += sizeof(dataLength);
        // 读取data
        fileBlock.data.assign(buffer.begin() + offset, buffer.begin() + offset + dataLength);
        return fileBlock;
    }
};

struct UserInfoAndAdapterInfo { 
    UserInfo userInfo;
    Adapter adapterInfo;
    /*| userInfoSize | userInfo | adapterInfoSize | adapterInfo*/
    // 序列化为 vector<char>
    std::vector<char> serialize() const {
        std::vector<char> buffer;
        // 序列化 userInfo
        std::vector<char> userInfoBuffer = userInfo.serialize();
        uint32_t userInfoSize = static_cast<uint32_t>(userInfoBuffer.size());
        auto* userInfoSizePtr = reinterpret_cast<const char*>(&userInfoSize);
        buffer.insert(buffer.end(), userInfoSizePtr, userInfoSizePtr + sizeof(userInfoSize));
        buffer.insert(buffer.end(), userInfoBuffer.begin(), userInfoBuffer.end());  
        // 序列化 adapterInfo
        std::vector<char> adapterInfoBuffer = adapterInfo.serialize();
        uint32_t adapterInfoSize = static_cast<uint32_t>(adapterInfoBuffer.size());
        auto* adapterInfoSizePtr = reinterpret_cast<const char*>(&adapterInfoSize);
        buffer.insert(buffer.end(), adapterInfoSizePtr, adapterInfoSizePtr + sizeof(adapterInfoSize));
        buffer.insert(buffer.end(), adapterInfoBuffer.begin(), adapterInfoBuffer.end());
        return buffer;
    }
    // 反序列化
    static UserInfoAndAdapterInfo deserialize(const std::vector<char>& buffer) {
        UserInfoAndAdapterInfo info;
        size_t offset = 0;
        // 读userInfoSize
        uint32_t userInfoSize = *reinterpret_cast<const uint32_t*>(&buffer[offset]);
        offset += sizeof(userInfoSize);
        // 读userInfo
        info.userInfo = UserInfo::deserialize(std::vector<char>(buffer.begin() + offset, buffer.begin() + offset + userInfoSize));
        offset += userInfoSize;
        // 读adapterInfoSize
        uint32_t adapterInfoSize = *reinterpret_cast<const uint32_t*>(&buffer[offset]);
        offset += sizeof(adapterInfoSize);
        // 读adapterInfo
        info.adapterInfo = Adapter::deserialize(std::vector<char>(buffer.begin() + offset, buffer.begin() + offset + adapterInfoSize));
        return info;
    }
    
};

