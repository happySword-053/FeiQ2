#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QCheckBox>
#include <QGroupBox>
#include <QFormLayout>
#include<QRadioButton>
#include <QButtonGroup>
#include<QDesktopServices>

#include"../jsonCpp/dispositionMiddle.h"
#include"../networkModule/NetworkHelper.h"
// ----------- PersonalInfoSettingsPage 类声明 -----------
// 因为 SettingsWidget 需要用到这个类，所以它的声明必须在 SettingsWidget 之前
class PersonalInfoSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit PersonalInfoSettingsPage(QWidget *parent = nullptr);
    ~PersonalInfoSettingsPage();

    QString getUserName() const{return usernameLineEdit->text();}
    DispositionMiddle *dispositionMiddle;
private slots:
    void on_commonAvatarSet_clicked();
    void on_customAvatarSet_clicked();
    void on_customAvatarCancel_clicked();
    void on_personalPhotoSet_clicked();
    void on_personalPhotoCancel_clicked();
    void on_viewAllUserInfo_clicked();
    void on_confirm_clicked();
    void on_cancel_clicked();

signals:
    void confirm();
    void cancle();
private:
    void setupUi();
    void setupConnections();

    //DispositionMiddle *dispositionMiddle;

    QLabel* pageTitleLabel;
    QLineEdit* usernameLineEdit;
    QComboBox* groupNameComboBox;

    // 头像设置
    QGroupBox* avatarGroupBox;
    QLabel* commonAvatarDisplay;
    QComboBox* commonAvatarComboBox;
    QLabel* customAvatarDisplay;
    QPushButton* customAvatarSetButton;
    QPushButton* customAvatarCancelButton;

    // 个人形象照片
    QGroupBox* personalPhotoGroupBox;
    QLabel* personalPhotoDisplay;
    QPushButton* personalPhotoSetButton;
    QPushButton* personalPhotoCancelButton;

    // 个性签名
    QGroupBox* signatureGroupBox;
    QTextEdit* signatureTextEdit;

    // 其他
    QPushButton* viewAllUserInfoButton;
    QCheckBox* shareInfoCheckBox;

    // 联系方式
    QLineEdit* officePhoneLineEdit;
    QLineEdit* homePhoneLineEdit;
    QLineEdit* mobilePhoneLineEdit;
    QLineEdit* emailLineEdit;
    QLineEdit* addressLineEdit;
    QLineEdit* qqLineEdit;

    // 底部按钮
    QPushButton* confirmButton;
    QPushButton* cancelButton;
};



// ----------- SystemSettingsPage 类声明 (新增) -----------
class SystemSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SystemSettingsPage(QWidget *parent = nullptr);
    ~SystemSettingsPage();
    bool getTrayClickOpenMainCheckBox() const{return trayClickOpenMainCheckBox->isChecked();}
    bool getDoubleClickMinimizedCheckBox() const{return doubleClickMinimizedCheckBox->isChecked();}
    bool getCloseToTrayCheckBox() const{return closeToTrayCheckBox->isChecked();}
    DispositionMiddle *dispositionMiddle;
private slots:
    // 启动设置
    void on_launchOnBootCheckBox_toggled(bool checked);
    void on_trayClickOpenMainCheckBox_toggled(bool checked); // 仅处理互斥逻辑

    // 文件路径选择
    void on_browseChatLogPathButton_clicked();
    void on_openChatLogPathButton_clicked();

    // 底部按钮
    void on_confirmButton_clicked();
    void on_cancelButton_clicked();
signals:
    void confirm();
    void cancle();
private:
    void setupUi();
    void setupConnections();

    QLabel* pageTitleLabel;

    // DispositionMiddle *dispositionMiddle;
    // 启动设置
    QGroupBox* launchSettingsGroupBox;
    QCheckBox* launchOnBootCheckBox;
    QCheckBox* launchMinimizedOnBootCheckBox;
    QCheckBox* trayClickOpenMainCheckBox;
    QCheckBox* doubleClickMinimizedCheckBox;
    QCheckBox* closeToTrayCheckBox;

    // 保存聊天记录
    QGroupBox* chatLogGroupBox;
    QLineEdit* chatLogPathLineEdit;
    QPushButton* browseChatLogPathButton;
    QPushButton* openChatLogPathButton;

    // 底部按钮
    QPushButton* confirmButton;
    QPushButton* cancelButton;
};

// ----------- NetworkSettingsPage 类声明 (新增) -----------
class NetworkSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkSettingsPage(QWidget *parent = nullptr);
    ~NetworkSettingsPage();
    DispositionMiddle *dispositionMiddle;
private slots:
    void on_macIpComboBox_currentIndexChanged(const QString &text); // 处理选择或输入变化
    void on_enableBreakpointResumeCheckBox_toggled(bool checked);

    void on_confirmButton_clicked();
    void on_cancelButton_clicked();
signals:
    void confirm();
    void cancle();
private:
    void setupUi();
    void setupConnections();

    QLabel* pageTitleLabel;

    // DispositionMiddle *dispositionMiddle;
    // MAC/IP 地址设置
    QGroupBox* macIpGroupBox;
    QComboBox* macIpComboBox; // 更改为 QComboBox
    QString currentMacIp;

    // 文件断点续传
    QGroupBox* fileTransferGroupBox;
    QCheckBox* enableBreakpointResumeCheckBox;
    QLineEdit* breakpointResumeSizeLineEdit;

    // 底部按钮
    QPushButton* confirmButton;
    QPushButton* cancelButton;
};

// ----------- SendReceiveSettingsPage 类声明 (新增) -----------
class FunctionSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionSettingsPage(QWidget *parent = nullptr);
    ~FunctionSettingsPage();
    DispositionMiddle *dispositionMiddle;
private slots:
    void on_browseReceivePathButton_clicked();
    void on_openReceivePathButton_clicked();
    void on_autoRefreshCheckBox_toggled(bool checked);
    void on_doNotDisturbCheckBox_toggled(bool checked);
    void on_confirmButton_clicked();
    void on_cancelButton_clicked();
signals:
    void confirm();
    void cancle();
private:
    void setupUi();
    void setupConnections();

    //DispositionMiddle *dispositionMiddle;
    // UI 元素
    QLabel* pageTitleLabel;

    // 文件接收设置
    QGroupBox* fileReceiveGroupBox;
    QLineEdit* receivePathLineEdit;
    QPushButton* browseReceivePathButton;
    QPushButton* openReceivePathButton;

    // 通知设置
    QGroupBox* notificationGroupBox;
    QCheckBox* onlineNotifyCheckBox;
    QCheckBox* offlineNotifyCheckBox;

    // 自动刷新设置
    QGroupBox* autoRefreshGroupBox;
    QCheckBox* autoRefreshCheckBox;
    QLineEdit* autoRefreshIntervalLineEdit;
    QLabel* autoRefreshUnitLabel; // 用于显示“分钟”

    // 请勿打扰设置
    QGroupBox* doNotDisturbGroupBox;
    QCheckBox* doNotDisturbCheckBox;
    QTextEdit* defaultReplyTextEdit; // 默认回复消息

    // 底部按钮
    QPushButton* confirmButton;
    QPushButton* cancelButton;
};

// ----------- SettingsWidget 类声明 -----------
class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();
    PersonalInfoSettingsPage* getPersonalInfoSettingsPage() { return personalInfoPage; }
    SystemSettingsPage* getSystemSettingsPage() { return systemSettingsPage; }
    NetworkSettingsPage* getNetworkSettingsPage() { return networkSettingsPage; }
    FunctionSettingsPage* getFunctionSettingsPage() { return functionSettingsPage; }
private slots:
    void on_listWidget_currentRowChanged(int currentRow);

private:
    void setupUi();
    void setupConnections();

    // 配置中间模型
    DispositionMiddle dispositionMiddle;

    QListWidget* navigationListWidget;
    QStackedWidget* stackedWidget;
    // 配置文件中间模型

    // 各个设置页面实例
    PersonalInfoSettingsPage* personalInfoPage;
    SystemSettingsPage* systemSettingsPage;
    NetworkSettingsPage* networkSettingsPage;
    FunctionSettingsPage* functionSettingsPage;
    // TODO: 如果有其他页面，可以在这里添加指针
    // QWidget* systemSettingsPage;
    // QWidget* sendReceiveSettingsPage;
    // ...
};

#endif // SETTINGSWIDGET_H