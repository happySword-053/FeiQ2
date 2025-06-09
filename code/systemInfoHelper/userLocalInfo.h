#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include"../logs/logs.h"
#include"AdapterInfo.h"
#include <sstream>
//默认userName和PCName为本机信息,userName后续可改
struct UserInfo{
    std::string userName;
    std::string PCName;
    // 新增序列化方法（载体改为 vector<char>）
    std::vector<char> serialize() const {
        std::ostringstream oss;
        oss << userName << "|" << PCName;  // 保持原字段拼接逻辑
        const std::string str = oss.str();  // 先生成中间字符串
        return std::vector<char>(str.begin(), str.end());  // 转换为 vector<char>
    }

    // 新增反序列化方法（输入改为 vector<char>）
    static UserInfo deserialize(const std::vector<char>& data) {
        // 将 vector<char> 转换为 string 用于解析
        std::string str(data.begin(), data.end());
        std::istringstream iss(str);
        UserInfo info;
        std::getline(iss, info.userName, '|');
        std::getline(iss, info.PCName, '|');
        return info;
    }
};

class UserInfoHelper{
private:
    // Adapter adapter;
    UserInfo userInfo;
    UserInfoHelper();
public:
    UserInfoHelper(const UserInfoHelper&) = delete;
    UserInfoHelper& operator=(const UserInfoHelper&) = delete;
    static UserInfoHelper& getInstance(){
        static UserInfoHelper instance;
        return instance;
    }
    ~UserInfoHelper(){}
    UserInfo getUserInfo(){return this->userInfo;}
    void setUserName(std::string userName){this->userInfo.userName = userName;}

};