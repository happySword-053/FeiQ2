#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <stdexcept> // 用于 std::runtime_error
#include "../logs/logs.h"
#include "../task/Mapper.h"

extern "C" {
#include "sqlite3.h"
}

class SqlLite {
private:
    sqlite3* chat_db = nullptr;

    // 创建会话表
    const char* sql_session = "CREATE TABLE IF NOT EXISTS conversations ( "
                              "peer_mac INTEGER PRIMARY KEY, "
                              "peer_name TEXT, "
                              "ip TEXT NOT NULL"
                              ");";
    // 创建消息表
    const char* sql_message = "CREATE TABLE IF NOT EXISTS messages ("
                              "mac INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "content TEXT NOT NULL, "
                              "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP"
                              ");";

    // --- 可变参数模板辅助函数，用于安全地绑定参数 ---

    // 基础情况：没有参数需要绑定时，直接返回
    void bind_params(sqlite3_stmt* stmt, int index) {}

    // 递归步骤：绑定一个参数，然后继续绑定余下的参数
    template<typename T, typename... Args>
    void bind_params(sqlite3_stmt* stmt, int index, T&& value, Args&&... args) {
        bind_single_param(stmt, index, std::forward<T>(value));
        bind_params(stmt, index + 1, std::forward<Args>(args)...);
    }

    // --- 根据不同类型重载的单个参数绑定函数 ---

    void bind_single_param(sqlite3_stmt* stmt, int index, int value) {
        sqlite3_bind_int(stmt, index, value);
    }
    
    void bind_single_param(sqlite3_stmt* stmt, int index, long long value) {
        sqlite3_bind_int64(stmt, index, value);
    }
    void bind_single_param(sqlite3_stmt* stmt, int index, double value) {
        sqlite3_bind_double(stmt, index, value);
    }
    void bind_single_param(sqlite3_stmt* stmt, int index, const char* value) {
        sqlite3_bind_text(stmt, index, value, -1, SQLITE_TRANSIENT);
    }
    void bind_single_param(sqlite3_stmt* stmt, int index, const std::string& value) {
        sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
    }

    void init_db() {
        char* err_msg = nullptr;
        int rc = sqlite3_exec(chat_db, sql_session, 0, 0, &err_msg);
        if (rc != SQLITE_OK) {
            std::string error = std::string("创建会话表错误: ") + err_msg;
            sqlite3_free(err_msg);
            LOG(error.c_str(), ERROR);
            throw std::runtime_error(error);
        }

        rc = sqlite3_exec(chat_db, sql_message, 0, 0, &err_msg);
        if (rc != SQLITE_OK) {
            std::string error = std::string("创建消息表错误: ") + err_msg;
            sqlite3_free(err_msg);
            LOG(error.c_str(), ERROR);
            throw std::runtime_error(error);
        }
    }

public:
    SqlLite() = default;

    ~SqlLite() {
        if (chat_db) {
            sqlite3_close(chat_db);
        }
    }

    // 禁止拷贝和赋值
    SqlLite(const SqlLite&) = delete;
    SqlLite& operator=(const SqlLite&) = delete;

    int open_db(const char* db_name) {
        if (sqlite3_open(db_name, &chat_db) != SQLITE_OK) {
            LOG("打开数据库失败", ERROR);
            return -1;
        }
        try {
            init_db();
        } catch (const std::runtime_error& e) {
            sqlite3_close(chat_db);
            chat_db = nullptr;
            return -1;
        }
        return 0;
    }

    // 安全且灵活的查询函数
    template<typename T, typename... Args>
    std::list<T> query(const char* sql, Args&&... args) {
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(chat_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            std::string error = std::string("查询预编译错误: ") + sqlite3_errmsg(chat_db);
            LOG(error.c_str(), ERROR);
            throw std::runtime_error(error);
        }

        bind_params(stmt, 1, std::forward<Args>(args)...);

        std::list<T> result;
        int rc;
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            T item;
            item.Mapper(stmt);
            result.push_back(item); // 修复了 Bug：将 item 添加到结果列表
        }

        if (rc != SQLITE_DONE) {
            std::string error = std::string("查询执行错误: ") + sqlite3_errmsg(chat_db);
            LOG(error.c_str(), ERROR);
            // 仍然需要释放 stmt
        }

        sqlite3_finalize(stmt);
        return result;
    }
    // 安全且灵活的更新/插入/删除函数
    template<typename... Args>
    void update(const char* sql, Args&&... args) {
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(chat_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            std::string error = std::string("更新/插入预编译错误: ") + sqlite3_errmsg(chat_db);
            LOG(error.c_str(), ERROR);
            throw std::runtime_error(error);
        }

        bind_params(stmt, 1, std::forward<Args>(args)...);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::string error = std::string("更新/插入执行错误: ") + sqlite3_errmsg(chat_db);
            LOG(error.c_str(), ERROR);
            sqlite3_finalize(stmt); // 确保在抛出异常前释放
            throw std::runtime_error(error);
        }
        sqlite3_finalize(stmt);
    }
};