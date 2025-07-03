#pragma once
//任务分发器
#include"../networkModule/NetworkManager.h"
#include"../fileSystem/FileSystem.h"
#include<QObject>

class TaskDispatcher {
    Q_OBJECT
public:
    TaskDispatcher(QObject* parent = nullptr);
    ~TaskDispatcher();
    
};
