#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include<cstdint>
#include"../systemInfoHelper/AdapterInfo.h"
#include"../systemInfoHelper/userLocalInfo.h"

#define BLOCK_SIZE 16 * 1024 // 16KB块大小
#define ERROR_TASK -1
#define NON_TASK 0
#define FILE_LIST_REQUEST 1 // 请求当前可以下载的文件列表
#define FILE_LIST_RETURN 2 // 返回房当前自身可下载的文件列表
#define  FILE_DOWNLOAD 3 // 文件下载
#define FILE_BLOCK 4// 当前的文件块
#define FILE_BLOCK_CONFIRM 5 //确认收到文件块
#define USERINFO_AND_ADAPTERINFO 6 // 发送用户信息和网卡信息
#define CHAT_MESSAGE 7 // 聊天文字消息

/*---------通用任务协议，最终封装的任务类---------*/


// 自定义协议
class Info {
public:
    
    int taskType;
    std::vector<char> data;  // 数据字节流
    /*|taskType | dataLength | data*/
    // 序列化为 vector<char>
    std::vector<char> serialize() const {
        std::vector<char> buffer;
        // 写入 taskType
        auto* typePtr = reinterpret_cast<const char*>(&taskType);
        buffer.insert(buffer.end(), typePtr, typePtr + sizeof(taskType));
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
        return info;
    }
};
// 表示每个文件的信息，在info类型中用vector存储上传的所有文件信息
struct FileInfo {
    /*|fileNameSize  |  fileName  |  fileSize*/
    std::string fileName;
    uint32_t fileSize;

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
        auto* sizePtr = reinterpret_cast<const uint32_t*>(&fileSize);
        buffer.insert(buffer.end(), sizePtr, sizePtr + sizeof(fileSize));
        return buffer;
    }

    // 从 vector<char> 反序列化
    static FileInfo deserialize(const std::vector<char>& buffer) {
        FileInfo info;
        size_t offset = 0;
        // 读文件名长度
        uint32_t nameLen = *reinterpret_cast<const uint32_t*>(&buffer[offset]);
        offset += sizeof(nameLen);
        // 读文件名
        info.fileName.assign(buffer.begin() + offset, buffer.begin() + offset + nameLen);
        offset += nameLen;
        // 读文件大小
        info.fileSize = *reinterpret_cast<const uint32_t*>(&buffer[offset]);
        offset += sizeof(info.fileSize);
        return info;
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
    uint32_t datalength;
    /*| datalength | userInfo | adapterInfo */
    // 序列化为 vector<char>
    std::vector<char> serialize() const {
        std::string userInfoStr = userInfo.serialize();
        std::string adapterInfoStr = adapterInfo.serialize();
        std::vector<char> buffer;
        // 写入datalength
        auto* lenPtr = reinterpret_cast<const char*>(&datalength);
        buffer.insert(buffer.end(), lenPtr, lenPtr + sizeof(datalength));
        // 写入 userInfo
        buffer.insert(buffer.end(), userInfoStr.begin(), userInfoStr.end());
        // 写入 adapterInfo
        buffer.insert(buffer.end(), adapterInfoStr.begin(), adapterInfoStr.end());
        return buffer;

    }
    // 从 vector<char> 反序列化
    static UserInfoAndAdapterInfo deserialize(const std::vector<char>& buffer) {
        UserInfoAndAdapterInfo info;
        size_t offset = 0;
        // 读datalength
        info.datalength = *reinterpret_cast<const uint32_t*>(&buffer[offset]);
        offset += sizeof(info.datalength);
        // 读userInfo
        std::string userInfoStr(buffer.begin() + offset, buffer.begin() + offset + info.datalength);
        info.userInfo = UserInfo::deserialize(userInfoStr);
        offset += info.datalength;
        // 读adapterInfo
        std::string adapterInfoStr(buffer.begin() + offset, buffer.end());
        info.adapterInfo = Adapter::deserialize(adapterInfoStr);
        return info;
    }
};