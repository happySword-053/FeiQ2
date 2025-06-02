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
    // 新增序列化方法
    std::string serialize() const {
        std::ostringstream oss;
        oss << userName << "|" << PCName;
        return oss.str();
    }

    // 新增反序列化方法
    static UserInfo deserialize(const std::string& data) {
        std::istringstream iss(data);
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
    ~UserInfoHelper();
    UserInfo getUserInfo(){return this->userInfo;}
    void setUserName(std::string userName){this->userInfo.userName = userName;}

};