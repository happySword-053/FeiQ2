#include "dispositionReader.h"
#include <iostream>

bool DispositionReader::writeConfig(const std::string & configName, std::vector<std::pair<std::string, std::string>>& configs)
{
    Json::StreamWriterBuilder writer;
    writer["emitUTF8"] = true;
    writer["indentation"] = "\t";
    std::unique_lock<std::mutex> lock(config_mutex_);
    if(configMap.find(configName) == configMap.end()){
        return false;
    }
    
    // 修改写入方式（添加错误处理）
    std::ofstream ofs(configMap[configName].second, std::ios::trunc); // 明确使用截断模式
    if (!ofs.is_open()) {
        throw std::runtime_error("无法写入文件！");
    }
    try {
        auto root = configMap[configName].first;
        for(auto& config : configs){
            // 检查配置项是否存在于根节点中
            if (root.isMember(config.first)) {
                // 如果存在，更新配置项的值
                root[config.first] = config.second;
                std::cout << "更新配置项: " << config.first << " = " << config.second << std::endl;

            }
        }
        std::unique_ptr<Json::StreamWriter> stream_writer(writer.newStreamWriter());
        stream_writer->write(root, &ofs);
        ofs.flush(); // 添加强制刷新
    } 
    catch (...) {
        ofs.close();
        throw std::runtime_error("JSON序列化失败");
    }
    return true;
}

Json::Value DispositionReader::readConfig(const std::string& filename)
{

    std::ifstream ifile(filename);
    if (!ifile.is_open()) {
        throw std::runtime_error("文件打开失败");
    }

    Json::CharReaderBuilder reader;
    reader["emitUTF8"] = true;

    Json::Value root;
    std::string errs;
    if (!Json::parseFromStream(reader, ifile, &root, &errs)) {
        throw std::runtime_error("JSON 解析失败: " + errs);
    }

    if (!root.isObject()) {
        throw std::runtime_error("根节点不是对象！");
    }

    return root;
}



bool DispositionReader::init()
{
    std::unique_lock<std::mutex> mtx(this->config_mutex_);
    try{
        auto json_files = getFileList();
        for (auto& file : json_files) {
            
            //提取文件名与路径
            auto begin = file.find_last_of("/"); // 查找最后一个'/'的位置
            auto end = file.find_last_of("."); // 查找最后一个'.'的位置
            auto name = file.substr(begin + 1, end - begin - 1); // 提取文件名（不包含路径和扩展名）
            
            // 读取配置文件
            Json::Value root = readConfig(file);
            // 将配置文件的根节点和文件名存储到 configMap 中
            configMap[name] = std::make_pair(root,file);
        }
    }catch(...){
        std::cerr << "DispositionReader init failed!" << std::endl;
        return false;
    }
    
    return true;
}



/**
 * @brief 获取指定路径下的所有 JSON 文件列表
 *
 * 此函数递归遍历指定路径（由 CONFIG_PATH 定义），查找所有的 JSON 文件，并将它们的路径存储在一个字符串向量中。
 * 如果在访问文件系统时发生错误，将捕获并输出错误信息，然后重新抛出异常。
 *
 * @return std::vector<std::string> 包含所有找到的 JSON 文件路径的向量
 */
std::vector<std::string> DispositionReader::getFileList()
{
    // 用于存储找到的 JSON 文件的路径
    std::vector<std::string> json_files;
    try {
        // 递归遍历指定路径下的所有文件和文件夹
        for (const auto& entry : std::filesystem::recursive_directory_iterator(CONFIG_PATH)) {
            // 检查当前条目是否为常规文件且扩展名是否为 .json
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                // 如果是 JSON 文件，则将其路径添加到 json_files 向量中
                json_files.push_back(entry.path().string());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // 若访问文件系统时出错，输出错误信息
        std::cerr << "Error accessing path: " << e.what() << std::endl;
        // 重新抛出异常，以便调用者可以处理
        throw e;
    }
    // 返回包含所有 JSON 文件路径的向量
    return json_files;
}
