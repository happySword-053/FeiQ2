#pragma once
#include<string>
#include"dispositionReader.h"
/*
配置中间层
*/
class DispositionMiddle
{

    
public:
    // 在此处添加配置类dto
    struct functionSetting{
        std::string fileReceivePath;// 文件接收位置

        bool userOnlineNotify;// 用户上线通知
        bool userOfflineNotify;// 用户下线通知

        bool enableAutoRefresh;// 启用自动刷新
        int refreshIntervalSeconds;// 自动刷新间隔时间

        bool enableDoNotDisturb;// 启用勿扰模式
        std::string autoReplyMessage;// 自动回复消息
    };
    struct systeemSetting{
        bool trayClickOpenMain;// 点击托盘图标打开主界面
        bool doubleClickMinimized;// 双击托盘图标最小化
        bool closeToTray;// 关闭到托盘
        std::string chatLogPath;// 聊天日志路径
    };
    struct personalSetting{
        bool isSetting;// 是否设置
        std::string username;// 用户名
    };
    struct networkSetting{
        bool breakPointResume; // 断点续传
        int breakpointResumeSize; // 启用断点续传大小
    };
    // 同步写入函数
    void syncWriteConfig();
    DispositionMiddle();
    ~DispositionMiddle();
    functionSetting& getFunctionSetting(){
        return this->_functionSetting;
    }
    systeemSetting& getSystemSetting(){
        return this->_systemSetting;
    }
    personalSetting& getPersonalSetting(){
        return this->_personalSetting;
    }
    networkSetting& getNetworkSetting(){
        return this->_networkSetting;
    }
    
private:
    functionSetting _functionSetting;
    systeemSetting _systemSetting;
    personalSetting _personalSetting;
    networkSetting _networkSetting;
};