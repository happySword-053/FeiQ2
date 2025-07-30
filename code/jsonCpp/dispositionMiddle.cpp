#include"dispositionMiddle.h"

void DispositionMiddle::syncWriteConfig()
{
    // 将当前的数据写入配置文件
    // 写功能配置
    std::vector<std::pair<std::string, ConfigValue>> functionSettings;
    functionSettings.emplace_back("fileReceivePath", _functionSetting.fileReceivePath);
    functionSettings.emplace_back("notificationSettings.userOnlineNotify", _functionSetting.userOnlineNotify);
    functionSettings.emplace_back("notificationSettings.userOfflineNotify", _functionSetting.userOfflineNotify);
    functionSettings.emplace_back("autoRefreshSettings.enableAutoRefresh", _functionSetting.enableAutoRefresh);
    functionSettings.emplace_back("autoRefreshSettings.autoRefreshInterval", _functionSetting.refreshIntervalSeconds);
    functionSettings.emplace_back("doNotDisturbSettings.enableDoNotDisturb", _functionSetting.enableDoNotDisturb);
    functionSettings.emplace_back("doNotDisturbSettings.autoReplyMessage", _functionSetting.autoReplyMessage);
    DispositionReader::getInstance().writeConfig("functionSetting", functionSettings);

    // 写网络配置
    std::vector<std::pair<std::string, ConfigValue>> networkSettings;
    networkSettings.emplace_back("breakPointResume", _networkSetting.breakPointResume);
    networkSettings.emplace_back("breakpointResumeSize", _networkSetting.breakpointResumeSize);
    DispositionReader::getInstance().writeConfig("networkSetting", networkSettings);

    // 写个人配置
    std::vector<std::pair<std::string, ConfigValue>> personalSettings;
    personalSettings.emplace_back("isSetting", _personalSetting.isSetting);
    personalSettings.emplace_back("username", _personalSetting.username);
    DispositionReader::getInstance().writeConfig("personalSetting", personalSettings);

    // 写系统配置
    std::vector<std::pair<std::string, ConfigValue>> systemSettings;
    systemSettings.emplace_back("trayClickOpenMain", _systemSetting.trayClickOpenMain);
    systemSettings.emplace_back("doubleClickMinimized", _systemSetting.doubleClickMinimized);
    systemSettings.emplace_back("closeToTray", _systemSetting.closeToTray);
    systemSettings.emplace_back("chatLogPath", _systemSetting.chatLogPath);
}

DispositionMiddle::DispositionMiddle()
{
    // 获取功能配置的根节点
    Json::Value functionSettingRoot = DispositionReader::getInstance().getConfig("functionSetting");
    if (!functionSettingRoot.empty()) {
        _functionSetting.fileReceivePath = functionSettingRoot["fileReceivePath"].asString();
        _functionSetting.userOnlineNotify = functionSettingRoot["notificationSettings"]["userOnlineNotify"].asBool();
        _functionSetting.userOfflineNotify = functionSettingRoot["notificationSettings"]["userOfflineNotify"].asBool();
        _functionSetting.refreshIntervalSeconds = functionSettingRoot["autoRefreshSettings"]["autoRefreshInterval"].asInt();
        _functionSetting.enableAutoRefresh = functionSettingRoot["autoRefreshSettings"]["enableAutoRefresh"].asBool();
        _functionSetting.autoReplyMessage = functionSettingRoot["doNotDisturbSettings"]["autoReplyMessage"].asString();
        _functionSetting.enableDoNotDisturb = functionSettingRoot["doNotDisturbSettings"]["enableDoNotDisturb"].asBool();
    }

    // 获取网络配置的根节点
    Json::Value networkSettingRoot = DispositionReader::getInstance().getConfig("networkSetting");
    if (!networkSettingRoot.empty()) {
        _networkSetting.breakPointResume = networkSettingRoot["breakPointResume"].asBool();
        _networkSetting.breakpointResumeSize = networkSettingRoot["breakpointResumeSize"].asInt();
    }

    // 获取个人配置的根节点
    Json::Value personalSettingRoot = DispositionReader::getInstance().getConfig("personalSetting");
    if (!personalSettingRoot.empty()) {
        _personalSetting.isSetting = personalSettingRoot["isSetting"].asBool();
        _personalSetting.username = personalSettingRoot["username"].asString();
    }

    // 获取系统配置的根节点
    Json::Value systemSettingRoot = DispositionReader::getInstance().getConfig("systemSetting");
    if (!systemSettingRoot.empty()) {
        _systemSetting.trayClickOpenMain = systemSettingRoot["trayClickOpenMain"].asBool();
        _systemSetting.doubleClickMinimized = systemSettingRoot["doubleClickMinimized"].asBool();
        _systemSetting.closeToTray = systemSettingRoot["closeToTray"].asBool();
        _systemSetting.chatLogPath = systemSettingRoot["chatLogPath"].asString();
    }
}

DispositionMiddle::~DispositionMiddle()
{
    syncWriteConfig();

}
