#pragma once

#include<string>
class ChatMessageDO{
private:
    std::string mac; //对方的mac地址
    std::string content; //消息内容
    int is_self; //0 表示不是自己发的，1 表示自己发的
    std::string timestamp; //消息时间戳
public:
    std::string getMac() const { return mac; }
    std::string getContent() const { return content; }
    int getIsSelf() const { return is_self; }
    std::string getTimestamp() const { return timestamp; }
    void setMac(const std::string& mac) { this->mac = mac; }
    void setContent(const std::string& content) { this->content = content; }
    void setIsSelf(int is_self) { this->is_self = is_self; }
    void setTimestamp(const std::string& timestamp) { this->timestamp = timestamp; }  

};