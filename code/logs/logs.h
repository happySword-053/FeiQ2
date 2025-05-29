#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#define ERROR 0x01
#define WARNING 0x02
#define INFO 0x03
class Logs;
//添加日志宏
#define LOG(MSG,TYPE) Logs::getInstance().log_append(MSG,TYPE)

class Logs{
public:
    static Logs& getInstance(){
        static Logs instance; // 静态局部变量，只会在第一次调用时初始化
        return instance;
    }
    void log_append(const std::string& message,int type);// 记录日志
private:
    //禁用默认构造函数和复制构造函数，防止外部实例化
    Logs(){};
    ~Logs(){logFile.close();};  
    Logs(const Logs&) = delete;
    //禁用赋值运算符，防止外部赋值
    Logs& operator=(const Logs&) = delete;
    std::fstream logFile; // 日志文件流对象
};