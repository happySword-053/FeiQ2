#include "../../code/jsonCpp/json/json.h"
#include <fstream>
#include <iostream>

void writeJsonFile(const std::string & filename, Json::Value & root) {
    Json::StreamWriterBuilder writer;
    writer["emitUTF8"] = true;
    writer["indentation"] = "\t";

    // 修改前（直接覆盖原数据）：
    // root["sitting"]["name"] = "小帅";
    
    // 修改后（保留原有数据结构）：
    if (!root.isMember("sitting")) {
        root["sitting"] = Json::objectValue;
    }
    root["sitting"]["name"] = "小帅";

    // 修改写入方式（添加错误处理）
    std::ofstream ofs(filename, std::ios::trunc); // 明确使用截断模式
    if (!ofs.is_open()) {
        throw std::runtime_error("无法写入文件！");
    }
    
    try {
        std::unique_ptr<Json::StreamWriter> stream_writer(writer.newStreamWriter());
        stream_writer->write(root, &ofs);
        ofs.flush(); // 添加强制刷新
    } 
    catch (...) {
        ofs.close();
        throw std::runtime_error("JSON序列化失败");
    }
}

Json::Value readJsonFile(const std::string & filename) {
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

int main() {
    try {
        auto root = readJsonFile("D:\\qtproject\\FeiQ2\\test\\jsonCpp\\test_json.json");
        std::cout << root["name"] << std::endl;
        std::cout << root["sitting"]["name"].asString() << std::endl;
        std::cout << root["number"] << std::endl;
        writeJsonFile("D:\\qtproject\\FeiQ2\\test\\jsonCpp\\test_json.json", root);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}