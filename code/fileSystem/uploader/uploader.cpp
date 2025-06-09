#include "uploader.h"
Uploader::~Uploader()
{
    //关闭所有文件
    try{
        for(auto &file : filelist){
            file.filedesc.close();
        }
    }catch(const std::exception& e){
        LOG("关闭文件失败",ERROR);
        
    }
}
bool Uploader::addFile(const std::string &filepath)
{
    LOG("添加文件" + filepath,INFO);
    //检查文件是否已经存在
    auto it = std::find_if(filelist.begin(), filelist.end(), [&](const UploadFile& file) {
        return file.filePath == filepath; // 检查文件路径是否匹配
    });
    if (it != filelist.end()) {
        LOG("文件已存在",ERROR); 
        return false;
    }
    LOG("尝试添加文件" + filepath,INFO);
    try{
        //打开文件
        std::fstream filedesc(filepath, std::ios::in | std::ios::binary );
        //检查文件是否打开成功

    }catch(const std::exception& e){
        LOG("打开文件失败",ERROR);
        return false;
    }
}
// blockId表示开始下载的位置
std::vector<char> Uploader::getFileBlock(const std::string &filename, uint32_t blockId)
{
    /*
    先检查参数是否合理
    */
    auto it = std::find_if(filelist.begin(), filelist.end(), [&](const UploadFile& file) {
        return file.fileinfo.fileName == filename; 
    });
    //未找到文件
    if (it == filelist.end()) {
        LOG(
            "无法找到指定文件" + filename + "，请检查文件名是否正确。",ERROR
        );
        throw std::runtime_error("无法找到指定文件");
    }
    //判断块id是否溢出
    if(blockId * BLOCK_SIZE >= (*it).fileinfo.fileSize){
        LOG(
            "块id超出文件大小范围",ERROR 
        );
        throw std::runtime_error("块id超出文件大小范围");
    }    
    
    //获取当前文件描述符
    auto &this_filedesc = (*it).filedesc;
    //移动文件指针
    this_filedesc.seekg(blockId * BLOCK_SIZE, std::ios::beg);
    //判断是否还能读BLOCK_SIZE大小的数据
    std::vector<char> buffer(BLOCK_SIZE + 80);
    try{
        //读取文件二进制数据
        if(this_filedesc.read(buffer.data(), BLOCK_SIZE)){

            LOG(std::string("成功获取文件块") + filename + "的第" + std::to_string(blockId) + "块",INFO); 
         }else{
            LOG(std::string("获取文件块") + filename + "的第" + std::to_string(blockId) + "块失败",ERROR);
            std::streamsize bytesRead = this_filedesc.gcount();
            if(bytesRead == 0){
                LOG("文件读取结束",INFO);
                //返回空的vector<char>
                return std::vector<char>(); 
            }
            buffer.resize(bytesRead); // 调整buffer大小以适应实际读取的字节数
            this_filedesc.read(buffer.data(), bytesRead);
         }

    }catch(const std::exception& e){
        LOG(
            "读取文件块失败",ERROR
        );
        throw std::runtime_error("读取文件块失败");
    }
    FileBlock fileblock;
    fileblock.blockId = blockId; // 设置blockId
    fileblock.data = buffer; // 复制文件数据到fileblock中
    //序列化fileblock
    auto fileblockBuffer = fileblock.serialize();
    return fileblockBuffer;
}

std::vector<struct FileInfo> Uploader::getFileList()
{
    std::vector<struct FileInfo> fileinfos;
    for(const auto& file : filelist){
        fileinfos.push_back(file.fileinfo);
    }
    return fileinfos;
}

bool Uploader::delFile(const std::string &filename)
{
    auto it = std::find_if(filelist.begin(), filelist.end(), [&](const UploadFile& file) {
        return file.fileinfo.fileName == filename; 
    });
    //未找到文件
    if (it == filelist.end()) {
        LOG(
            "无法找到指定文件" + filename + "，请检查文件名是否正确。",ERROR
        );
        throw std::runtime_error("无法找到指定文件");
        return false; 
    }
    try{
        //关闭文件
        (*it).filedesc.close(); 
    }catch(const std::exception& e){
        LOG(
            "关闭文件失败",ERROR
        );
        return false;
    }
    LOG(
        "删除文件" + filename + "成功",INFO
    );
    return true;
}
