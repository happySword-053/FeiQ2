#ifndef SELF_DOMAIN
#define SELF_DOMAIN
#include <string>
#include <vector>
#include <cstdint>

//文件块类，包含文件信息和二进制数据
#define BLOCK_SIZE 1024 * 8 // 每个块的大小8kb
#define ERROR_TASK -1
#define NON_TASK 0
#define FILE_LIST_REQUEST 1 // 请求当前可以下载的文件列表
#define FILE_LIST_RETURN 2 // 返回房当前自身可下载的文件列表
#define  FILE_DOWNLOAD 3 // 文件下载
#define FILE_BLOCK 4// 当前的文件块
#define FILE_BLOCK_CONFIRM 5 //确认收到文件块
//////////////////////////////////////////////
//自定义任务协议

class Info  {
    public:
        int taskType;
        std::vector<char> data;  // 数据字节流
    
        // 序列化为 vector<char>
        std::vector<char> serialize()  {
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

//////////////////////////////////////////////
struct FileInfo {
    /*
    序列化布局
    |filenameSize uint32 4字节|filename |filesize 4字节|
    */
    std::string filename; // 文件名
    uint32_t filesize; // 文件大小
    //序列胡函数
    std::vector<char> serialize() const {
        std::vector<char> buffer;
        //写入文件长度
        uint32_t fileNameSize = static_cast<uint32_t>(filename.size());
        auto* len = reinterpret_cast<const char*>(&fileNameSize);
        buffer.insert(buffer.end(),len,len + sizeof(fileNameSize));
        //写入filename
        buffer.insert(buffer.end(),filename.begin(),filename.end());
        //写入filesize
        auto* sizePtr = reinterpret_cast<const uint32_t*>(&filesize);
        buffer.insert(buffer.end(), sizePtr, sizePtr + sizeof(filesize));
        return buffer;
    }
    //反序列化函数
    static FileInfo deserialize(const std::vector<char>& data){
        FileInfo fileinfo;
        size_t offset = 0;
        //读取文件名长度
        uint32_t fileNameSize = *reinterpret_cast<const uint32_t*>(&data[offset]);
        offset += sizeof(fileNameSize);
        //读取文件名
        fileinfo.filename = std::string(data.begin() + offset, data.begin() + offset + fileNameSize);
        offset += fileNameSize;
        //读取文件大小
        fileinfo.filesize = *reinterpret_cast<const uint32_t*>(&data[offset]);
        return fileinfo;
    }
};

struct FileBlock{
    /*
    序列化描述
    |序列化FileInfo大小 uint32 4字节|二进制数据大小 uint32 4字节|FileInfo 序列化|data 二进制数据|
    */
    struct FileInfo fileinfo; // 文件信息
    std::vector<char> data;//二进制文件数据
    //序列化函数
    // 序列化为 vector<char>
    std::vector<char> serialize() const {
        std::vector<char> buffer;
        //写入FileInfo 大小
        auto fileinfoBuffer = fileinfo.serialize();
        uint32_t fileinfoSize = static_cast<uint32_t>(fileinfoBuffer.size());
        auto* fileinfoSizePtr = reinterpret_cast<const char*>(&fileinfoSize);
        buffer.insert(buffer.end(),fileinfoSizePtr,fileinfoSizePtr + sizeof(fileinfoSize));
        //写入FileInfo 二进制数据大小
        uint32_t dataSize = static_cast<uint32_t>(data.size());
        auto* dataSizePtr = reinterpret_cast<const char*>(&dataSize);
        buffer.insert(buffer.end(),dataSizePtr,dataSizePtr + sizeof(dataSize));
        //写入FileInfo 二进制数据
        buffer.insert(buffer.end(),fileinfoBuffer.begin(),fileinfoBuffer.end());
        //写入data 二进制数据
        buffer.insert(buffer.end(),data.begin(),data.end());
        return buffer;
    }

    // 从 vector<char> 反序列化
    static FileBlock deserialize(const std::vector<char>& buffer) {
        FileBlock block;
        size_t offset = 0;
        // 读取 FileInfo 大小
        uint32_t fileinfoSize = *reinterpret_cast<const uint32_t*>(&buffer[offset]);
        offset += sizeof(fileinfoSize);
        // 读取 data 大小
        uint32_t dataSize = *reinterpret_cast<const uint32_t*>(&buffer[offset]);
        offset += sizeof(dataSize);
        // 读取 FileInfo 二进制数据
        std::vector<char> fileinfoBuffer(buffer.begin() + offset, buffer.begin() + offset + fileinfoSize);
        offset += fileinfoSize;
        // 读取 data 二进制数据
        std::vector<char> dataBuffer(buffer.begin() + offset, buffer.begin() + offset + dataSize);
        // 反序列化 FileInfo
        block.fileinfo = FileInfo::deserialize(fileinfoBuffer);
        // 复制 data 二进制数据
        block.data = move(dataBuffer);
        return block;
    }
};
#endif 