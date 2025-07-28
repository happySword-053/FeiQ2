#pragma once
#include "json/json.h"
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem> // 包含 C++17 的文件系统库头文件
#include <mutex>


#ifndef CONFIG_PATH
#define CONFIG_PATH "../config/" // 配置文件的根路径，可根据需要修改，注意最后有一个"/"
#endif

//主要任务是获取json对象方便获取配置
class DispositionReader {
public:
    // 获取单例实例
    static DispositionReader& getInstance() {
        static DispositionReader instance;  // C++11起保证线程安全
        return instance;
    }
    Json::Value getConfig(const std::string& configName){
        
        if(configMap.find(configName) == configMap.end()){
           //返回空的json对象
           return Json::Value(); 
        }
        return configMap[configName].first;
    }
    // 删除拷贝构造函数和赋值运算符
    DispositionReader(const DispositionReader&) = delete;
    void operator=(const DispositionReader&) = delete;
    DispositionReader(const DispositionReader&&) = delete;
    void operator=(const DispositionReader&&) = delete;
    // 修改配置
    bool writeConfig(const std::string & configName, std::vector<std::pair<std::string, std::string>>& configs);
    

private:
    // 将构造函数和析构函数私有化
    DispositionReader(){}
    
    // 初始化函数
    bool init();
    // // 销毁函数
    // bool destroy();
    // 读取配置
    Json::Value readConfig(const std::string& filename);
    std::unordered_map<std::string, std::pair<Json::Value,std::string>> configMap; // 存储配置文件的映射
    std::mutex config_mutex_; // 添加互斥锁以确保线程安全

private:
    std::vector<std::string> getFileList(); // 获取指定路径下的所有文件列表的函数声明
};

