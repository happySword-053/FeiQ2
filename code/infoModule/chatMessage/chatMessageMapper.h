#pragma once
#include"../../Mapper/Mapper.h"
#include"chatMessageDO.h"
#include<string>
class ChatMessageMapper : public Mapper<ChatMessageDO> {
public:
    ChatMessageDO mapper(sqlite3_stmt* stmt) override{
        ChatMessageDO chatMessageDO;
        chatMessageDO.setMac(sqlite3_column_int(stmt, 0));
        chatMessageDO.setContent(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        chatMessageDO.setIsSelf(sqlite3_column_int(stmt, 2));
        chatMessageDO.setTimestamp(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
        return chatMessageDO;
    }
};