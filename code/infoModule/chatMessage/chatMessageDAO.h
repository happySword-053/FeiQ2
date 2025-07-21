#pragma once
#include"chatMessageMapper.h"
#include"../../Sqlite3/SqlLite.h"
#include<list>
class ChatMessageDAO {
private:
    const int pageSize = 10;
public:
    /*
    根据最早的时间戳和好友的mac来分页查询聊天消息
    */
    std::list<ChatMessageDO> getChatMessage( const std::string& mac, const std::string& oldestTimestamp = ""){
        std::list<ChatMessageDO> chatMessageDOs;
         std::string sql;
        if(oldestTimestamp.empty()){
            sql = "SELECT * FROM messages "
                  "WHERE mac =? "
                  "ORDER BY timestamp DESC "
                  "LIMIT ?;";// 每次查询10条消息，可根据需要调整
        }
        else {
            sql = "SELECT * FROM messages "
                          "WHERE timestamp <= ? AND mac = ? "
                          "ORDER BY timestamp DESC "
                          "LIMIT ?;";// 每次查询10条消息，可根据需要调整
        }
        ChatMessageMapper mapper;
        try{
            chatMessageDOs = SqlLite::getInstance().query<ChatMessageDO>(
                sql.c_str(),
                mapper,
                oldestTimestamp,
                mac,
                pageSize
            );
        }catch(const std::runtime_error& e){
            LOG(std::string("查询聊天消息失败,sql为: ") + sql + "错误输出" + e.what(), ERROR);
            throw std::runtime_error("查询聊天消息失败"); 
        }
        return chatMessageDOs;
    }

    int insertChatMessage(const ChatMessageDO& chatMessageDO){
        std::string sql = "INSERT INTO messages (mac, content, is_self, timestamp) "
                          "VALUES (?, ?, ?, ?);"; 
        try{
            SqlLite::getInstance().update(
                sql.c_str(),
                chatMessageDO.getMac(),
                chatMessageDO.getContent(),
                chatMessageDO.getIsSelf(),
                chatMessageDO.getTimestamp()
            );
        }catch(const std::runtime_error& e){
            LOG(std::string("插入聊天消息失败,sql为: ") + sql + "错误输出" + e.what(), ERROR);
            throw std::runtime_error("插入聊天消息失败"); 
        }
        return 0;
    }
};