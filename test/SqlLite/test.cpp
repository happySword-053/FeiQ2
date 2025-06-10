extern "C" {
    #include "sqlite3.h"

}
#include <iostream>
#include <string>
using namespace std;


int main(){
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open("test.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return(1);
    }
    const char *sql;
    "ID INT PRIMARY KEY     NOT NULL,"
    "NAME           TEXT    NOT NULL,"
    "AGE            INT     NOT NULL);";

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
    fprintf(stderr, "SQL错误: %s\n", err_msg);
    sqlite3_free(err_msg);
    } else {
    fprintf(stdout, "表创建成功\n");
    }
    sql = "INSERT INTO PERSON (ID, NAME, AGE) VALUES (2, '李浩', 55);";

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL错误: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        fprintf(stdout, "数据插入成功\n");
    }
    sql = "SELECT * FROM PERSON;";

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);


    //查询
    sql = "SELECT * FROM PERSON;";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL错误: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return(1);
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        int age = sqlite3_column_int(stmt, 2);
        printf("ID = %d, NAME = %s, AGE = %d\n", id, name, age); 
    }
    sqlite3_close(db);
}