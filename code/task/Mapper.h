#pragma once
#include<any>
#include<cstdint>
#include<string>
extern "C" {
	#include<sqlite3.h>
}

class ConversationMapper {
public: 
    uint64_t mac;
    std::string name;
    std::string ip;

    void Mapper(sqlite3_stmt* stmt) {
        mac = sqlite3_column_int64(stmt, 0);
        const char* name_text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* ip_text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        name = name_text ? name_text : ""; // 防止 name 为 NULL 导致崩溃
        ip = ip_text ? ip_text : "";       // 防止 ip 为 NULL 导致崩溃
    }
};

class MessageMapper {
public: 
    uint64_t conversation_id;
    std::string content;
    std::string timestamp;

    void Mapper(sqlite3_stmt* stmt) {
        conversation_id = sqlite3_column_int64(stmt, 0);
        const char* content_text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* timestamp_text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        content = content_text ? content_text : "";
        timestamp = timestamp_text ? timestamp_text : "";
    }
};