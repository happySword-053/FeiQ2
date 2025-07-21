#pragma once
#include<sqllite/sqlite3.h>
template<class T>//do对象
class Mapper{
public:
    virtual T mapper(sqlite3_stmt* stmt)=0;// 映射函数
};