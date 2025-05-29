#include "logs.h"
Logs::Logs() {
    //获取当前时间
    time_t now = time(0);
    //转换为本地时间
    tm* localTime = localtime(&now);
    //格式化时间字符串
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    //用当前时间作为日志名
    std::string logName = buffer + std::string(".log");
    //打开日志文件
    logFile.open(logName, std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "无法打开日志文件" << std::endl;
        //throw std::runtime_error("无法打开日志文件");
        exit(1); // 退出程序，返回错误码1
    }

}

void Logs::log_append(const std::string &message,int type)
{
    //获取当前时间
    time_t now = time(0);
    //转换为本地时间
    tm* localTime = localtime(&now);
    //格式化时间字符串
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    //解析type
    std::string type_str;
    switch(type){
        case ERROR:
            type_str = "ERROR";
            break;
        case WARNING:
            type_str = "WARNING";
            break;
        case INFO:
            type_str = "INFO";
            break;
        default:
            type_str = "UNKNOWN";
            throw std::runtime_error("未知的日志类型");
            break;
    }
    //格式化日志消息
    std::string line = "[" + std::string(buffer) + "] " + "[" + type_str + "] " + message + "\n";
    //写入日志文件
    try{
        logFile << line;
    }catch(std::exception& e){
        //std::cerr << "写入日志文件失败: " << e.what() << std::endl;
        throw std::runtime_error("写入日志文件失败");
        //exit(1); // 退出程序，返回错误码1
    }
}
