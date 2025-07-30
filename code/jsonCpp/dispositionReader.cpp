#include "dispositionReader.h"

// 添加辅助函数：按点分隔符拆分路径
std::vector<std::string> splitPath(const std::string& path) {
    std::vector<std::string> result;
    std::stringstream ss(path);
    std::string segment;
    while (std::getline(ss, segment, '.')) {
        if (!segment.empty()) {
            result.push_back(segment);
        }
    }
    return result;
}

// 添加辅助函数：递归设置嵌套JSON值
void setNestedValue(Json::Value& root, const std::vector<std::string>& pathSegments, const ConfigValue& value, size_t index = 0) {
    if (index == pathSegments.size() - 1) {
        // 根据值类型设置JSON节点
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                root[pathSegments[index]] = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                root[pathSegments[index]] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                root[pathSegments[index]] = arg;
            }
        }, value);
        return;
    }
    
    std::string currentKey = pathSegments[index];
    if (!root.isMember(currentKey) || !root[currentKey].isObject()) {
        root[currentKey] = Json::Value(Json::objectValue);
    }
    
    setNestedValue(root[currentKey], pathSegments, value, index + 1);
}

bool DispositionReader::writeConfig(const std::string & configName, std::vector<std::pair<std::string, ConfigValue>>& configs)
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
            std::vector<std::string> pathSegments = splitPath(config.first);
            if (pathSegments.empty()) continue;
            // 使用多类型值设置嵌套节点
            setNestedValue(root, pathSegments, config.second);
            //std::cout << "更新配置项: " << config.first << " = " << config.second << std::endl;
            LOG("更新配置项: " + configName, INFO);
        }
        std::unique_ptr<Json::StreamWriter> stream_writer(writer.newStreamWriter());
        stream_writer->write(root, &ofs);
        ofs.flush(); // 添加强制刷新
        configMap[configName].first = root; // 同步回内存
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
            
            // 提取文件名与路径 - 更健壮的方式
            size_t begin = file.find_last_of("/\\");
            size_t end = file.find_last_of(".");
            std::string name;
            
            if (begin != std::string::npos && end != std::string::npos && begin < end) {
                name = file.substr(begin + 1, end - begin - 1);
            } else if (end != std::string::npos) {
                // 没有路径分隔符，但有扩展名
                name = file.substr(0, end);
            } else if (begin != std::string::npos) {
                // 有路径分隔符，但没有扩展名
                name = file.substr(begin + 1);
            } else {
                // 既没有路径分隔符，也没有扩展名
                name = file;
            }
            // 读取配置文件
            Json::Value root = readConfig(file);
            // 将配置文件的根节点和文件名存储到 configMap 中
            configMap[name] = std::make_pair(root,file);
        }
    }catch(...){
        LOG("DispositionReader init failed!", ERROR);
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

std::string DispositionReader::readValue(const std::string& configName, const std::string& key)
{
    std::unique_lock<std::mutex> lock(config_mutex_);
    if(configMap.find(configName) == configMap.end()){
        return "";
    }
    Json::Value root = configMap[configName].first;
    std::vector<std::string> pathSegments = splitPath(key);
    for (size_t i = 0; i < pathSegments.size(); ++i) {
        if (!root.isMember(pathSegments[i])) {
            return "";
        }
        root = root[pathSegments[i]];
    }
    if (root.isString()) return root.asString();
    if (root.isInt()) return std::to_string(root.asInt());
    if (root.isDouble()) return std::to_string(root.asDouble());
    // 其他类型可自行扩展
    return "";
}
