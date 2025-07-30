#include "settingswidget.h"
#include <QHeaderView>
#include <QDebug>
#include <QFileDialog>
#include <QPixmap>
#include <QIcon>
#include <QDir>
#include <QButtonGroup>
#include <QDesktopServices>
#include <QUrl>
#include <QIntValidator>

// ----------- PersonalInfoSettingsPage 类实现 (保持不变) -----------
// ... (PersonalInfoSettingsPage 的实现代码) ...
PersonalInfoSettingsPage::PersonalInfoSettingsPage(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    setupConnections();
}

PersonalInfoSettingsPage::~PersonalInfoSettingsPage()
{
}

void PersonalInfoSettingsPage::setupUi()
{
    // 页面标题
    pageTitleLabel = new QLabel("个人资料设置", this);
    pageTitleLabel->setAlignment(Qt::AlignLeft);
    QFont font = pageTitleLabel->font();
    font.setPointSize(14);
    font.setBold(true);
    pageTitleLabel->setFont(font);
    pageTitleLabel->setStyleSheet("QLabel { padding: 5px; border-bottom: 1px solid #CCC; margin-bottom: 10px; }");

    // 用户名和组名
    QFormLayout* userInfoLayout = new QFormLayout();
    usernameLineEdit = new QLineEdit(this);
    groupNameComboBox = new QComboBox(this);
    groupNameComboBox->addItem("我的好友");
    groupNameComboBox->addItem("家人");
    groupNameComboBox->addItem("同事");
    userInfoLayout->addRow("用户名:", usernameLineEdit);
    userInfoLayout->addRow("组名:", groupNameComboBox);

    // 头像设置区域
    avatarGroupBox = new QGroupBox("头像", this);
    QLabel* commonAvatarText = new QLabel("常用头像:", avatarGroupBox);
    commonAvatarComboBox = new QComboBox(avatarGroupBox);
    for(int i = 1; i <= 3; ++i) { // 假设有3个常用头像资源
        commonAvatarComboBox->addItem(QIcon(QString(":/images/common_avatar_%1.png").arg(i)), QString("常用头像%1").arg(i));
    }
    commonAvatarDisplay = new QLabel(avatarGroupBox);
    commonAvatarDisplay->setFixedSize(64, 64);
    commonAvatarDisplay->setPixmap(QIcon(":/images/common_avatar_1.png").pixmap(64, 64)); // 初始显示第一个
    commonAvatarDisplay->setScaledContents(true);
    connect(commonAvatarComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        commonAvatarDisplay->setPixmap(commonAvatarComboBox->itemIcon(index).pixmap(64, 64));
    });


    QLabel* customAvatarText = new QLabel("个性头像:", avatarGroupBox);
    customAvatarDisplay = new QLabel(avatarGroupBox);
    customAvatarDisplay->setFixedSize(64, 64);
    customAvatarDisplay->setPixmap(QPixmap(":/images/default_custom_avatar.png").scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 默认占位符
    customAvatarDisplay->setScaledContents(true);
    customAvatarSetButton = new QPushButton("设置", avatarGroupBox);
    customAvatarCancelButton = new QPushButton("取消", avatarGroupBox);

    QHBoxLayout* commonAvatarLayout = new QHBoxLayout();
    commonAvatarLayout->addWidget(commonAvatarText);
    commonAvatarLayout->addWidget(commonAvatarComboBox);
    commonAvatarLayout->addWidget(commonAvatarDisplay);
    commonAvatarLayout->addStretch();

    QVBoxLayout* customAvatarButtonsLayout = new QVBoxLayout();
    customAvatarButtonsLayout->addWidget(customAvatarSetButton);
    customAvatarButtonsLayout->addWidget(customAvatarCancelButton);

    QHBoxLayout* customAvatarLayout = new QHBoxLayout();
    customAvatarLayout->addWidget(customAvatarText);
    customAvatarLayout->addWidget(customAvatarDisplay);
    customAvatarLayout->addLayout(customAvatarButtonsLayout);
    customAvatarLayout->addStretch();

    QVBoxLayout* avatarMainLayout = new QVBoxLayout(avatarGroupBox);
    avatarMainLayout->addLayout(commonAvatarLayout);
    avatarMainLayout->addLayout(customAvatarLayout);

    // 个人形象照片和个性签名
    personalPhotoGroupBox = new QGroupBox("个人形象照片", this);
    personalPhotoDisplay = new QLabel(personalPhotoGroupBox);
    personalPhotoDisplay->setFixedSize(128, 128); // 图片大小
    personalPhotoDisplay->setPixmap(QPixmap(":/images/default_personal_photo.png").scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 默认占位符
    personalPhotoDisplay->setScaledContents(true);
    personalPhotoSetButton = new QPushButton("设置", personalPhotoGroupBox);
    personalPhotoCancelButton = new QPushButton("取消", personalPhotoGroupBox);

    QVBoxLayout* personalPhotoButtonsLayout = new QVBoxLayout();
    personalPhotoButtonsLayout->addWidget(personalPhotoSetButton);
    personalPhotoButtonsLayout->addWidget(personalPhotoCancelButton);

    QHBoxLayout* personalPhotoLayout = new QHBoxLayout(personalPhotoGroupBox);
    personalPhotoLayout->addWidget(personalPhotoDisplay);
    personalPhotoLayout->addLayout(personalPhotoButtonsLayout);
    personalPhotoLayout->addStretch();

    signatureGroupBox = new QGroupBox("个性签名", this);
    signatureTextEdit = new QTextEdit(signatureGroupBox);
    signatureTextEdit->setPlaceholderText("在这里输入您的个性签名...");
    QVBoxLayout* signatureLayout = new QVBoxLayout(signatureGroupBox);
    signatureLayout->addWidget(signatureTextEdit);

    QHBoxLayout* imageAndSignatureLayout = new QHBoxLayout();
    imageAndSignatureLayout->addWidget(personalPhotoGroupBox, 1);
    imageAndSignatureLayout->addWidget(signatureGroupBox, 1);

    // 其他信息
    viewAllUserInfoButton = new QPushButton("查看所有用户信息资料", this);
    shareInfoCheckBox = new QCheckBox("以下个人信息共享", this);
    shareInfoCheckBox->setChecked(true); // 默认选中

    // 联系方式
    QFormLayout* contactLayout = new QFormLayout();
    officePhoneLineEdit = new QLineEdit(this);
    homePhoneLineEdit = new QLineEdit(this);
    mobilePhoneLineEdit = new QLineEdit(this);
    emailLineEdit = new QLineEdit(this);
    addressLineEdit = new QLineEdit(this);
    qqLineEdit = new QLineEdit(this);

    contactLayout->addRow("办公电话:", officePhoneLineEdit);
    contactLayout->addRow("住宅电话:", homePhoneLineEdit);
    contactLayout->addRow("手机:", mobilePhoneLineEdit);
    contactLayout->addRow("邮箱:", emailLineEdit);
    contactLayout->addRow("地址:", addressLineEdit);
    contactLayout->addRow("其他(QQ):", qqLineEdit);


    // 底部按钮
    confirmButton = new QPushButton("确定", this);
    cancelButton = new QPushButton("取消", this);

    QHBoxLayout* bottomButtonLayout = new QHBoxLayout();
    bottomButtonLayout->addStretch();
    bottomButtonLayout->addWidget(confirmButton);
    bottomButtonLayout->addWidget(cancelButton);
    bottomButtonLayout->addStretch();


    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(pageTitleLabel);
    mainLayout->addLayout(userInfoLayout);
    mainLayout->addWidget(avatarGroupBox);
    mainLayout->addLayout(imageAndSignatureLayout);
    mainLayout->addWidget(viewAllUserInfoButton);
    mainLayout->addWidget(shareInfoCheckBox);
    mainLayout->addLayout(contactLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(bottomButtonLayout);

    setLayout(mainLayout);
}

void PersonalInfoSettingsPage::setupConnections()
{
    // 连接按钮点击事件
    connect(customAvatarSetButton, &QPushButton::clicked, this, &PersonalInfoSettingsPage::on_customAvatarSet_clicked);
    connect(customAvatarCancelButton, &QPushButton::clicked, this, &PersonalInfoSettingsPage::on_customAvatarCancel_clicked);
    connect(personalPhotoSetButton, &QPushButton::clicked, this, &PersonalInfoSettingsPage::on_personalPhotoSet_clicked);
    connect(personalPhotoCancelButton, &QPushButton::clicked, this, &PersonalInfoSettingsPage::on_personalPhotoCancel_clicked);
    connect(viewAllUserInfoButton, &QPushButton::clicked, this, &PersonalInfoSettingsPage::on_viewAllUserInfo_clicked);
    connect(confirmButton, &QPushButton::clicked, this, &PersonalInfoSettingsPage::on_confirm_clicked);
    connect(cancelButton, &QPushButton::clicked, this, &PersonalInfoSettingsPage::on_cancel_clicked);

    // 示例：头像选择对话框 (需要 QApplication)
    connect(customAvatarSetButton, &QPushButton::clicked, this, [this](){
        QString fileName = QFileDialog::getOpenFileName(this, tr("选择个性头像"), QDir::homePath(), tr("图像文件 (*.png *.jpg *.bmp)"));
        if (!fileName.isEmpty()) {
            QPixmap pixmap(fileName);
            if (!pixmap.isNull()) {
                customAvatarDisplay->setPixmap(pixmap.scaled(customAvatarDisplay->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                qDebug() << "已设置个性头像：" << fileName;
            } else {
                qDebug() << "无法加载图像文件：" << fileName;
            }
        }
    });

    connect(personalPhotoSetButton, &QPushButton::clicked, this, [this](){
        QString fileName = QFileDialog::getOpenFileName(this, tr("选择个人形象照片"), QDir::homePath(), tr("图像文件 (*.png *.jpg *.bmp)"));
        if (!fileName.isEmpty()) {
            QPixmap pixmap(fileName);
            if (!pixmap.isNull()) {
                personalPhotoDisplay->setPixmap(pixmap.scaled(personalPhotoDisplay->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                qDebug() << "已设置个人形象照片：" << fileName;
            } else {
                qDebug() << "无法加载图像文件：" << fileName;
            }
        }
    });
}

// 槽函数实现 (目前只做调试输出)
void PersonalInfoSettingsPage::on_commonAvatarSet_clicked() { /* 实际上，这个槽通常由 commonAvatarComboBox 的 currentIndexChanged 信号触发 */ }
void PersonalInfoSettingsPage::on_customAvatarSet_clicked() { /* 实际逻辑由 lambda 表达式处理 */ }
void PersonalInfoSettingsPage::on_customAvatarCancel_clicked() {
    qDebug() << "个性头像取消";
    customAvatarDisplay->setPixmap(QPixmap(":/images/default_custom_avatar.png").scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
void PersonalInfoSettingsPage::on_personalPhotoSet_clicked() { /* 实际逻辑由 lambda 表达式处理 */ }
void PersonalInfoSettingsPage::on_personalPhotoCancel_clicked() {
    qDebug() << "个人形象照片取消";
    personalPhotoDisplay->setPixmap(QPixmap(":/images/default_personal_photo.png").scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
void PersonalInfoSettingsPage::on_viewAllUserInfo_clicked() { qDebug() << "查看所有用户信息资料 按钮被点击！"; }
void PersonalInfoSettingsPage::on_confirm_clicked() { 
    qDebug() << "确定 按钮被点击！"; 
    // 读取中间层的值
    dispositionMiddle->getPersonalSetting().isSetting = true;
    dispositionMiddle->getPersonalSetting().username = usernameLineEdit->text().toStdString();
    emit confirm();
}
void PersonalInfoSettingsPage::on_cancel_clicked() { 
    qDebug() << "取消 按钮被点击！";
    // 读取中间层的值
    dispositionMiddle->getPersonalSetting().isSetting = false;
    usernameLineEdit->setText(QString::fromStdString(dispositionMiddle->getPersonalSetting().username));
    
    emit cancle();

 }


// ----------- SystemSettingsPage 类实现 (保持不变) -----------

SystemSettingsPage::SystemSettingsPage(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    setupConnections();
}

SystemSettingsPage::~SystemSettingsPage()
{
}

void SystemSettingsPage::setupUi()
{
    // 页面标题
    pageTitleLabel = new QLabel("系统设置", this);
    pageTitleLabel->setAlignment(Qt::AlignLeft);
    QFont font = pageTitleLabel->font();
    font.setPointSize(14);
    font.setBold(true);
    pageTitleLabel->setFont(font);
    pageTitleLabel->setStyleSheet("QLabel { padding: 5px; border-bottom: 1px solid #CCC; margin-bottom: 10px; }");


    // 主布局，只包含启动设置和保存聊天记录
    QVBoxLayout* mainContentLayout = new QVBoxLayout();

    // 启动设置
    launchSettingsGroupBox = new QGroupBox("启动设置", this);
    QVBoxLayout* launchLayout = new QVBoxLayout(launchSettingsGroupBox);
    // launchOnBootCheckBox = new QCheckBox("开机启动运行此程序", launchSettingsGroupBox);
    // launchMinimizedOnBootCheckBox = new QCheckBox("开机启动时，程序最小化", launchSettingsGroupBox);
    trayClickOpenMainCheckBox = new QCheckBox("左键单击托盘图标打开主界面", launchSettingsGroupBox);
    doubleClickMinimizedCheckBox = new QCheckBox("双击运行程序时，程序最小化", launchSettingsGroupBox);
    closeToTrayCheckBox = new QCheckBox("关闭主面板时，不退出程序，最小化到任务栏", launchSettingsGroupBox);


    // 托盘图标打开主界面和开机启动最小化互斥
    trayClickOpenMainCheckBox->setChecked(true); // 默认选中

    //launchLayout->addWidget(launchOnBootCheckBox);
    //launchLayout->addWidget(launchMinimizedOnBootCheckBox);
    launchLayout->addWidget(trayClickOpenMainCheckBox);
    launchLayout->addWidget(doubleClickMinimizedCheckBox);
    launchLayout->addWidget(closeToTrayCheckBox);
    mainContentLayout->addWidget(launchSettingsGroupBox);

    // 保存聊天记录
    chatLogGroupBox = new QGroupBox("保存聊天记录", this);
    QHBoxLayout* chatLogPathLayout = new QHBoxLayout();
    chatLogPathLineEdit = new QLineEdit(this);
    chatLogPathLineEdit->setText("D:\\Program Files\\FeiQ\\feiql.fql"); // 示例路径
    browseChatLogPathButton = new QPushButton("...", this);
    browseChatLogPathButton->setFixedWidth(30);
    openChatLogPathButton = new QPushButton("打开", this);
    openChatLogPathButton->setFixedWidth(60);

    chatLogPathLayout->addWidget(chatLogPathLineEdit);
    chatLogPathLayout->addWidget(browseChatLogPathButton);
    chatLogPathLayout->addWidget(openChatLogPathButton);
    QVBoxLayout* chatLogVLayout = new QVBoxLayout(chatLogGroupBox);
    chatLogVLayout->addLayout(chatLogPathLayout);
    mainContentLayout->addWidget(chatLogGroupBox);

    mainContentLayout->addStretch();


    // 底部确认和取消按钮
    confirmButton = new QPushButton("确定", this);
    cancelButton = new QPushButton("取消", this);

    QHBoxLayout* bottomButtonLayout = new QHBoxLayout();
    bottomButtonLayout->addStretch();
    bottomButtonLayout->addWidget(confirmButton);
    bottomButtonLayout->addWidget(cancelButton);
    bottomButtonLayout->addStretch();


    // 整个页面的主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(pageTitleLabel);
    mainLayout->addLayout(mainContentLayout);
    mainLayout->addLayout(bottomButtonLayout);

    setLayout(mainLayout);
}

void SystemSettingsPage::setupConnections()
{
    //connect(launchOnBootCheckBox, &QCheckBox::toggled, launchMinimizedOnBootCheckBox, &QCheckBox::setEnabled);

    // connect(trayClickOpenMainCheckBox, &QCheckBox::toggled, this, &SystemSettingsPage::on_trayClickOpenMainCheckBox_toggled);
    //connect(launchMinimizedOnBootCheckBox, &QCheckBox::toggled, this, &SystemSettingsPage::on_launchOnBootCheckBox_toggled);

    // connect(browseChatLogPathButton, &QPushButton::clicked, this, &SystemSettingsPage::on_browseChatLogPathButton_clicked);
    // connect(openChatLogPathButton, &QPushButton::clicked, this, &SystemSettingsPage::on_openChatLogPathButton_clicked);

    connect(confirmButton, &QPushButton::clicked, this, &SystemSettingsPage::on_confirmButton_clicked);
    connect(cancelButton, &QPushButton::clicked, this, &SystemSettingsPage::on_cancelButton_clicked);
}

// void SystemSettingsPage::on_launchOnBootCheckBox_toggled(bool checked) {
//     if (sender() == launchMinimizedOnBootCheckBox) {
//         if (checked) {
//             trayClickOpenMainCheckBox->setChecked(false);
//         }
//     } else if (sender() == launchOnBootCheckBox) {
//         launchMinimizedOnBootCheckBox->setEnabled(checked);
//     }
// }

// void SystemSettingsPage::on_trayClickOpenMainCheckBox_toggled(bool checked) {
//     if (checked) {
//         launchMinimizedOnBootCheckBox->setChecked(false);
//     }
// }

// void SystemSettingsPage::on_browseChatLogPathButton_clicked() {
//     QString dir = QFileDialog::getExistingDirectory(this, tr("选择聊天记录保存路径"), chatLogPathLineEdit->text());
//     if (!dir.isEmpty()) {
//         chatLogPathLineEdit->setText(dir);
//         qDebug() << "聊天记录路径设置为：" << dir;
//     }
// }

// void SystemSettingsPage::on_openChatLogPathButton_clicked() {
//     QString path = chatLogPathLineEdit->text();
//     if (!path.isEmpty()) {
//         QDesktopServices::openUrl(QUrl::fromLocalFile(path));
//         qDebug() << "打开聊天记录路径：" << path;
//     }
// }

void SystemSettingsPage::on_confirmButton_clicked() { 
    qDebug() << "系统设置 - 确定 按钮被点击！"; 
    dispositionMiddle->getSystemSetting().trayClickOpenMain = trayClickOpenMainCheckBox->isChecked();
    dispositionMiddle->getSystemSetting().doubleClickMinimized = doubleClickMinimizedCheckBox->isChecked();
    dispositionMiddle->getSystemSetting().closeToTray = closeToTrayCheckBox->isChecked();
    dispositionMiddle->getSystemSetting().chatLogPath = chatLogPathLineEdit->text().toStdString();
    emit confirm();
}
void SystemSettingsPage::on_cancelButton_clicked() { 
    qDebug() << "系统设置 - 取消 按钮被点击！"; 
    // 读中间层的值
    trayClickOpenMainCheckBox->setChecked(dispositionMiddle->getSystemSetting().trayClickOpenMain);
    doubleClickMinimizedCheckBox->setChecked(dispositionMiddle->getSystemSetting().doubleClickMinimized);
    closeToTrayCheckBox->setChecked(dispositionMiddle->getSystemSetting().closeToTray);
    chatLogPathLineEdit->setText(QString::fromStdString(dispositionMiddle->getSystemSetting().chatLogPath));
    emit cancle();
}

// ----------- NetworkSettingsPage 类实现 (更新，使用 QComboBox) -----------
NetworkSettingsPage::NetworkSettingsPage(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    setupConnections();
}

NetworkSettingsPage::~NetworkSettingsPage()
{
}

void NetworkSettingsPage::setupUi()
{
    // 页面标题
    pageTitleLabel = new QLabel("网络设置", this);
    pageTitleLabel->setAlignment(Qt::AlignLeft);
    QFont font = pageTitleLabel->font();
    font.setPointSize(14);
    font.setBold(true);
    pageTitleLabel->setFont(font);
    pageTitleLabel->setStyleSheet("QLabel { padding: 5px; border-bottom: 1px solid #CCC; margin-bottom: 10px; }");

    // 主布局
    QVBoxLayout* mainContentLayout = new QVBoxLayout();

    // MAC/IP 地址设置
    macIpGroupBox = new QGroupBox("指定 MAC/IP 地址", this);
    QFormLayout* macIpLayout = new QFormLayout(macIpGroupBox);
    macIpComboBox = new QComboBox(this);
    macIpComboBox->addItem("默认自动检测"); // 默认选项
    macIpComboBox->addItem("00:11:22:33:44:55,192.168.1.100"); // 示例预设值1
    macIpComboBox->addItem("AA:BB:CC:DD:EE:FF,192.168.1.101"); // 示例预设值2
    macIpComboBox->setPlaceholderText("选择或输入MAC地址和IP地址（例如：AA:BB:CC:DD:EE:FF,192.168.1.100）"); // 占位符提示

    macIpLayout->addRow("指定MAC/IP:", macIpComboBox);
    mainContentLayout->addWidget(macIpGroupBox);

    // 文件断点续传
    fileTransferGroupBox = new QGroupBox("文件传输设置", this);
    QFormLayout* fileTransferLayout = new QFormLayout(fileTransferGroupBox);
    enableBreakpointResumeCheckBox = new QCheckBox("启用文件断点续传功能", this);
    breakpointResumeSizeLineEdit = new QLineEdit(this);
    breakpointResumeSizeLineEdit->setPlaceholderText("例如：1024 (KB)");
    breakpointResumeSizeLineEdit->setValidator(new QIntValidator(1, 999999, this)); // 限制输入为整数
    breakpointResumeSizeLineEdit->setText("1024"); // 默认值
    enableBreakpointResumeCheckBox->setChecked(true); // 默认启用

    fileTransferLayout->addRow(enableBreakpointResumeCheckBox);
    fileTransferLayout->addRow("文件大小(KB)大于:", breakpointResumeSizeLineEdit);
    mainContentLayout->addWidget(fileTransferGroupBox);

    mainContentLayout->addStretch(); // 将上面的内容推到顶部

    // 底部按钮
    confirmButton = new QPushButton("确定", this);
    cancelButton = new QPushButton("取消", this);

    QHBoxLayout* bottomButtonLayout = new QHBoxLayout();
    bottomButtonLayout->addStretch();
    bottomButtonLayout->addWidget(confirmButton);
    bottomButtonLayout->addWidget(cancelButton);
    bottomButtonLayout->addStretch();

    // 整个页面的主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(pageTitleLabel);
    mainLayout->addLayout(mainContentLayout);
    mainLayout->addLayout(bottomButtonLayout);

    setLayout(mainLayout);
}

void NetworkSettingsPage::setupConnections()
{
    // 连接到 QComboBox 的 currentIndexChanged 信号，用QString版本，它在用户选择或输入新文本时都会触发
    
    
    connect(macIpComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_macIpComboBox_currentIndexChanged(QString)));
    connect(enableBreakpointResumeCheckBox, &QCheckBox::toggled, this, &NetworkSettingsPage::on_enableBreakpointResumeCheckBox_toggled);

    connect(confirmButton, &QPushButton::clicked, this, &NetworkSettingsPage::on_confirmButton_clicked);
    connect(cancelButton, &QPushButton::clicked, this, &NetworkSettingsPage::on_cancelButton_clicked);
}

void NetworkSettingsPage::on_macIpComboBox_currentIndexChanged(const QString &text)
{
    // 这里可以添加逻辑来解析 MAC 地址和 IP 地址，并进行验证
    qDebug() << "QComboBox 选中或输入的值：" << text;
    if (text == "默认自动检测" || text.isEmpty()) {
        qDebug() << "选择了默认自动检测或输入为空。";
        // 在实际应用中，这里可能需要清除保存的MAC/IP设置
    } else if (text.contains(",") && text.split(",").size() == 2) {
        QString mac = text.split(",").at(0).trimmed();
        QString ip = text.split(",").at(1).trimmed();
        qDebug() << "解析到的 MAC：" << mac << ", IP：" << ip;
        // 实际应用中需要更严格的MAC和IP格式验证，并保存
    } else {
        qDebug() << "MAC/IP 格式不正确，请使用 'MAC地址,IP地址' 格式。";
    }
}

void NetworkSettingsPage::on_enableBreakpointResumeCheckBox_toggled(bool checked)
{
    breakpointResumeSizeLineEdit->setEnabled(checked);
    qDebug() << "文件断点续传功能设置为：" << (checked ? "启用" : "禁用");
}

void NetworkSettingsPage::on_confirmButton_clicked()
{
    qDebug() << "网络设置 - 确定 按钮被点击！";
    // 在这里获取并保存所有设置
    dispositionMiddle->getNetworkSetting().breakPointResume = enableBreakpointResumeCheckBox->isChecked();
    dispositionMiddle->getNetworkSetting().breakpointResumeSize = breakpointResumeSizeLineEdit->text().toInt();
    emit confirm();
}

void NetworkSettingsPage::on_cancelButton_clicked()
{
    qDebug() << "网络设置 - 取消 按钮被点击！";
    // 恢复到上次保存的状态或默认状态
    
}

//FunctionSettingsPage 类实现
FunctionSettingsPage::FunctionSettingsPage(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    setupConnections();
}

FunctionSettingsPage::~FunctionSettingsPage()
{
}

void FunctionSettingsPage::setupUi()
{
    // 页面标题
    pageTitleLabel = new QLabel("功能设置", this);
    pageTitleLabel->setAlignment(Qt::AlignLeft);

    QFont font = pageTitleLabel->font();
    font.setPointSize(14);
    font.setBold(true);
    pageTitleLabel->setFont(font);
    pageTitleLabel->setStyleSheet("QLabel { padding: 5px; border-bottom: 1px solid #CCC; margin-bottom: 10px; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(pageTitleLabel);

    // 1. 文件接收设置
    fileReceiveGroupBox = new QGroupBox("文件接收设置", this);
    QHBoxLayout* fileReceivePathLayout = new QHBoxLayout();
    receivePathLineEdit = new QLineEdit(this);
    receivePathLineEdit->setPlaceholderText("D:\\FeiQ_Received_Files"); // 示例默认路径
    browseReceivePathButton = new QPushButton("...", this);
    browseReceivePathButton->setFixedWidth(30);
    openReceivePathButton = new QPushButton("打开", this);
    openReceivePathButton->setFixedWidth(60);

    fileReceivePathLayout->addWidget(receivePathLineEdit);
    fileReceivePathLayout->addWidget(browseReceivePathButton);
    fileReceivePathLayout->addWidget(openReceivePathButton);
    QVBoxLayout* fileReceiveVLayout = new QVBoxLayout(fileReceiveGroupBox);
    fileReceiveVLayout->addLayout(fileReceivePathLayout);
    mainLayout->addWidget(fileReceiveGroupBox);

    // 2. 通知设置
    notificationGroupBox = new QGroupBox("通知设置", this);
    QVBoxLayout* notificationLayout = new QVBoxLayout(notificationGroupBox);
    onlineNotifyCheckBox = new QCheckBox("其他用户上线时进行通知", this);
    offlineNotifyCheckBox = new QCheckBox("其他用户下线时进行通知", this);
    onlineNotifyCheckBox->setChecked(true); // 默认选中
    offlineNotifyCheckBox->setChecked(true); // 默认选中

    notificationLayout->addWidget(onlineNotifyCheckBox);
    notificationLayout->addWidget(offlineNotifyCheckBox);
    mainLayout->addWidget(notificationGroupBox);

    // 3. 自动刷新设置
    autoRefreshGroupBox = new QGroupBox("自动刷新设置", this);
    QHBoxLayout* autoRefreshLayout = new QHBoxLayout();
    autoRefreshCheckBox = new QCheckBox("自动刷新", this);
    autoRefreshCheckBox->setChecked(true); // 默认选中
    autoRefreshIntervalLineEdit = new QLineEdit(this);
    autoRefreshIntervalLineEdit->setValidator(new QIntValidator(1, 999, this)); // 限制输入为1-999的整数
    autoRefreshIntervalLineEdit->setText("10"); // 默认10分钟
    autoRefreshIntervalLineEdit->setFixedWidth(50);
    autoRefreshUnitLabel = new QLabel("分钟", this);

    autoRefreshLayout->addWidget(autoRefreshCheckBox);
    autoRefreshLayout->addWidget(autoRefreshIntervalLineEdit);
    autoRefreshLayout->addWidget(autoRefreshUnitLabel);
    autoRefreshLayout->addStretch(); // 将内容推到左侧
    QVBoxLayout* autoRefreshVLayout = new QVBoxLayout(autoRefreshGroupBox);
    autoRefreshVLayout->addLayout(autoRefreshLayout);
    mainLayout->addWidget(autoRefreshGroupBox);

    // 4. 请勿打扰设置
    doNotDisturbGroupBox = new QGroupBox("请勿打扰模式", this);
    QVBoxLayout* doNotDisturbLayout = new QVBoxLayout(doNotDisturbGroupBox);
    doNotDisturbCheckBox = new QCheckBox("启用请勿打扰模式", this);
    defaultReplyTextEdit = new QTextEdit(this);
    defaultReplyTextEdit->setPlaceholderText("请输入默认回复消息（例如：我正在忙，稍后回复。）");
    defaultReplyTextEdit->setMinimumHeight(60); // 设置一个最小高度
    defaultReplyTextEdit->setEnabled(false); // 默认禁用，当请勿打扰模式启用时才启用

    doNotDisturbLayout->addWidget(doNotDisturbCheckBox);
    doNotDisturbLayout->addWidget(defaultReplyTextEdit);
    mainLayout->addWidget(doNotDisturbGroupBox);

    mainLayout->addStretch(); // 填充剩余空间

    // 底部按钮
    confirmButton = new QPushButton("确定", this);
    cancelButton = new QPushButton("取消", this);

    QHBoxLayout* bottomButtonLayout = new QHBoxLayout();
    bottomButtonLayout->addStretch();
    bottomButtonLayout->addWidget(confirmButton);
    bottomButtonLayout->addWidget(cancelButton);
    bottomButtonLayout->addStretch();
    mainLayout->addLayout(bottomButtonLayout);

    setLayout(mainLayout);

    // 初始状态根据复选框设置
    on_autoRefreshCheckBox_toggled(autoRefreshCheckBox->isChecked());
    on_doNotDisturbCheckBox_toggled(doNotDisturbCheckBox->isChecked());
}

void FunctionSettingsPage::setupConnections()
{
    // 文件接收路径
    connect(browseReceivePathButton, &QPushButton::clicked, this, &FunctionSettingsPage::on_browseReceivePathButton_clicked);
    connect(openReceivePathButton, &QPushButton::clicked, this, &FunctionSettingsPage::on_openReceivePathButton_clicked);

    // 自动刷新联动
    connect(autoRefreshCheckBox, &QCheckBox::toggled, this, &FunctionSettingsPage::on_autoRefreshCheckBox_toggled);

    // 请勿打扰联动
    connect(doNotDisturbCheckBox, &QCheckBox::toggled, this, &FunctionSettingsPage::on_doNotDisturbCheckBox_toggled);

    // 底部按钮
    connect(confirmButton, &QPushButton::clicked, this, &FunctionSettingsPage::on_confirmButton_clicked);
    connect(cancelButton, &QPushButton::clicked, this, &FunctionSettingsPage::on_cancelButton_clicked);
}

void FunctionSettingsPage::on_browseReceivePathButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("选择文件接收路径"), receivePathLineEdit->text());
    if (!dir.isEmpty()) {
        receivePathLineEdit->setText(dir);
        qDebug() << "文件接收路径设置为：" << dir;
    }
}

void FunctionSettingsPage::on_openReceivePathButton_clicked()
{
    QString path = receivePathLineEdit->text();
    if (!path.isEmpty() && QDir(path).exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        qDebug() << "打开文件接收路径：" << path;
    } else {
        qDebug() << "文件接收路径不存在或为空：" << path;
    }
}

void FunctionSettingsPage::on_autoRefreshCheckBox_toggled(bool checked)
{
    autoRefreshIntervalLineEdit->setEnabled(checked);
    autoRefreshUnitLabel->setEnabled(checked);
    qDebug() << "自动刷新功能设置为：" << (checked ? "启用" : "禁用");
}

void FunctionSettingsPage::on_doNotDisturbCheckBox_toggled(bool checked)
{
    defaultReplyTextEdit->setEnabled(checked);
    qDebug() << "请勿打扰模式设置为：" << (checked ? "启用" : "禁用");
}

void FunctionSettingsPage::on_confirmButton_clicked()
{
    qDebug() << "功能设置 - 确定 按钮被点击！";
    // 收集并保存所有设置
    QString receivePath = receivePathLineEdit->text();
    bool onlineNotify = onlineNotifyCheckBox->isChecked();
    bool offlineNotify = offlineNotifyCheckBox->isChecked();
    bool autoRefresh = autoRefreshCheckBox->isChecked();
    int autoRefreshInterval = autoRefreshIntervalLineEdit->text().toInt();
    bool doNotDisturb = doNotDisturbCheckBox->isChecked();
    QString defaultReply = defaultReplyTextEdit->toPlainText();

    qDebug() << "保存设置：";
    qDebug() << "  文件接收路径：" << receivePath;
    qDebug() << "  上线通知：" << onlineNotify;
    qDebug() << "  下线通知：" << offlineNotify;
    qDebug() << "  自动刷新：" << autoRefresh;
    qDebug() << "  自动刷新间隔：" << autoRefreshInterval << "分钟";
    qDebug() << "  请勿打扰模式：" << doNotDisturb;
    qDebug() << "  默认回复消息：" << defaultReply;

    // 在实际应用中，这里会将这些设置保存到配置文件或数据库中
}

void FunctionSettingsPage::on_cancelButton_clicked()
{
    qDebug() << "功能设置 - 取消 按钮被点击！";
    // 在实际应用中，这里会恢复到上次保存的状态或默认状态
}

// ----------- SettingsWidget 类实现 (保持不变) -----------

SettingsWidget::SettingsWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    setupConnections();
}

SettingsWidget::~SettingsWidget()
{
}

void SettingsWidget::setupUi()
{
    setWindowTitle("设置");
    resize(800, 600);

    // 左侧导航栏
    navigationListWidget = new QListWidget(this);
    navigationListWidget->addItem("个人资料设置");
    navigationListWidget->addItem("系统设置");

    navigationListWidget->addItem("功能设置");
    navigationListWidget->addItem("网络设置");

    navigationListWidget->setFixedWidth(150);
    navigationListWidget->setStyleSheet(
        "QListWidget { background-color: #E0FFE0; border: 1px solid #C0DCC0; }"
        "QListWidget::item { padding: 5px; }"
        "QListWidget::item:selected { background-color: #A0D0A0; color: white; }"
        "QListWidget::item:hover { background-color: #C0E0C0; }"
    );

    // 右侧堆叠窗口
    stackedWidget = new QStackedWidget(this);

    // 创建并添加各个设置页面
    personalInfoPage = new PersonalInfoSettingsPage(this);
    stackedWidget->addWidget(personalInfoPage);
    personalInfoPage->dispositionMiddle = &this->dispositionMiddle;

    systemSettingsPage = new SystemSettingsPage(this);
    stackedWidget->addWidget(systemSettingsPage);
    systemSettingsPage->dispositionMiddle = &this->dispositionMiddle;
    // 添加 "功能设置" 页面
    functionSettingsPage = new FunctionSettingsPage(this); // 使用新创建的 FunctionSettingsPage 实例
    stackedWidget->addWidget(functionSettingsPage);
    functionSettingsPage->dispositionMiddle = &this->dispositionMiddle;

    // Add the NetworkSettingsPage
    networkSettingsPage = new NetworkSettingsPage(this);
    stackedWidget->addWidget(networkSettingsPage);
    networkSettingsPage->dispositionMiddle = &this->dispositionMiddle;


    // 主布局
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(navigationListWidget);
    mainLayout->addWidget(stackedWidget);
    setLayout(mainLayout);

    // 默认选中第一个
    navigationListWidget->setCurrentRow(0);
}

void SettingsWidget::setupConnections()
{
    connect(navigationListWidget, &QListWidget::currentRowChanged,
            this, &SettingsWidget::on_listWidget_currentRowChanged);
}

void SettingsWidget::on_listWidget_currentRowChanged(int currentRow)
{
    stackedWidget->setCurrentIndex(currentRow);
    qDebug() << "切换到设置页面索引：" << currentRow << " (" << navigationListWidget->currentItem()->text() << ")";
}
