#pragma once
#include<string>
#include<set>
#include"../systemInfoHelper/userLocalInfo.h"
#include"../systemInfoHelper/AdapterInfo.h"
#include"../logs/logs.h"
#include<functional>
//表示列表中每一个好友
class FriendInfo {
private:
    std::string ip;// 好友的IP地址
    std::string mac;// 好友的MAC地址
    UserInfo userInfo;  // 用户本地信息
    std::string oldestTimestamp;  // 最早的时间戳 用于下次查找最早的消息
public:
    FriendInfo(){}
    FriendInfo(const std::string& ip, const UserInfo& userInfo) : ip(ip), userInfo(userInfo) {}
    FriendInfo getFriendInfo() const { return *this; }
    void setIp(const std::string& ip) { this->ip = ip; }
    void setUserInfo(const UserInfo& userInfo) { this->userInfo = userInfo; }
    void setMac(const std::string& mac) { this->mac = mac; }
    void setOldestTimestamp(const std::string& oldestTimestamp) { this->oldestTimestamp = oldestTimestamp; }
    // 重载==运算符，用于比较两个FriendInfo对象是否相等
    
    //获取ip
    std::string getIp() const { return ip; }
    //获取用户信息
    UserInfo getUserInfo() const { return userInfo; }
    //获取mac
    std::string getMac() const { return mac; }
    //获取最早的时间戳
    std::string getOldestTimestamp() const { return oldestTimestamp; }
    bool operator==(const FriendInfo& other) const {
        return mac == other.mac;  // 比较IP地址是否相等
    }
    
    // 添加小于运算符重载（set必需）
    bool operator<(const FriendInfo& other) const {
        return ip < other.ip;  // 按IP地址升序排序
    }
};
class Friends{
private:
    std::set<FriendInfo> friendsMap;  // 存储好友信息的映射表，键为IP地址，值为FriendInfo对象
    
public:
    

    // 添加好友信息
    void addFriendWithoutNameInfo(const std::string& ip){
        FriendInfo friendInfo;
        friendInfo.setIp(ip);
        friendsMap.insert(friendInfo);
    }
    void addFriend(const std::string& ip, const UserInfo& userInfo) {
        FriendInfo friendInfo(ip, userInfo);  // 创建FriendInfo对象
        friendsMap.insert(friendInfo);
    }
    void appendUserinfo(const std::string&ip, const UserInfo& userInfo){
        auto it = friendsMap.find({ip, userInfo});
        if(it == friendsMap.end()){
            LOG(std::string("没有找到该好友,ip为") + ip + std::string(",用户名") + userInfo.userName,ERROR);
            throw std::runtime_error("没有找到该好友,ip为" + ip + ",用户名" + userInfo.userName);
        }
        (*it).getFriendInfo().setUserInfo(userInfo);
    }
    void removeFriend(const FriendInfo& it) {
        try{
            friendsMap.erase(it);  // 从映射表中删除指定IP地址的好友信息

        }catch(std::exception& e){
            LOG(std::string("删除好友失败,ip为") + it.getFriendInfo().getIp() + std::string(",用户名") + it.getFriendInfo().getUserInfo().userName,ERROR);
            throw std::runtime_error(std::string("删除好友失败,ip为") + it.getFriendInfo().getIp() + std::string(",用户名") + it.getFriendInfo().getUserInfo().userName); 
        }
    }
    FriendInfo getFriendInfoByMac(const std::string& mac){
        for(auto it = friendsMap.begin(); it != friendsMap.end(); it++){
            if(it->getMac() == mac){
                return *it;
            }
        }
        LOG(std::string("没有找到该好友,mac为") + mac,ERROR);
        throw std::runtime_error(std::string("没有找到该好友,mac为") + mac);
    }
};