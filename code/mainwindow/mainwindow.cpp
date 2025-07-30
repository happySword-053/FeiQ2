#include "mainwindow.h"


// MainWindow 构造函数
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) // 初始化父类QMainWindow
    //, ui(new Ui::MainWindow) // 注释掉的UI初始化
{
    // 设置icon
    this->setWindowIcon(QIcon(":/main_icon/mainIcon.png")); // 设置窗口图标
    // 初始化系统托盘
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/main_icon/mainIcon.png"));
    trayIcon->setToolTip("FeiQ2");
    // === 新增托盘菜单初始化 ===
    trayMenu = new QMenu(this);
    
    // 添加"显示主窗口"菜单项
    QAction* showAction = new QAction("显示主窗口", this);
    connect(showAction, &QAction::triggered, this, &MainWindow::show);
    
    // 添加"退出程序"菜单项
    QAction* exitAction = new QAction("退出", this);
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);
    
    // 将菜单项添加到菜单
    trayMenu->addAction(showAction);
    trayMenu->addSeparator(); // 添加分隔线
    trayMenu->addAction(exitAction);
    
    // 设置托盘图标上下文菜单
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
    //ui->setupUi(this); // 注释掉的UI设置
    QWidget *central = new QWidget(this); // 创建中央部件
    setCentralWidget(central); // 设置中央部件

    // 创建水平分割器：左侧好友列表 + 右侧内容区
    QSplitter *splitter = new QSplitter(Qt::Horizontal, central);

    // 创建文件共享管理界面
    this->shareFileManagerWidget = new ShareFileManagerWidget();
    // 创建设置窗口
    this->settingsWidget = new SettingsWidget();

    // -----------------------
    // 1) 左侧：好友列表
    // -----------------------
    //添加一个刷新按钮
    QPushButton *refreshBtn = new QPushButton(this); // 创建刷新按钮
    refreshBtn->setIcon(QIcon(":/main_icon/refrush.png")); // 设置图标
    
    m_friendList = new QListWidget(splitter); // 创建好友列表控件
    m_friendList->setMinimumWidth(160); // 设置最小宽度
    m_friendList->setFrameShape(QFrame::NoFrame); // 设置无边框
    m_friendList->setStyleSheet(
        "QListWidget { background: #fafafa; }"
        "QListWidget::item:selected { background: #d0e7ff; }"
    ); // 设置样式表
    /*--------------可以后续添加加载好友逻辑，以下只是示例---------------------*/
    // // 示例好友数据    
    // QStringList names = { "Alice", "Bob", "Charlie", "David", "Eve" };
    // for (const QString &name : names) { // 遍历好友名称列表
    //     QListWidgetItem *item = new QListWidgetItem(m_friendList); // 创建列表项
    //     item->setSizeHint(QSize(0, 48)); // 设置项高度
    //     m_friendList->addItem(item); // 添加项到列表

    //     FriendItemWidget *w = new FriendItemWidget(name); // 创建自定义好友项控件
    //     m_friendList->setItemWidget(item, w); // 为列表项设置控件
    // }
    // m_friendList->setCurrentRow(0); // 设置默认选中第一行

    // -----------------------
    // 2) 右侧整体：容器 rightContainer
    //    — 上：contentArea（水平布局：聊天主区 + 侧栏）
    //    — 下：输入区域
    // -----------------------
    QWidget *rightContainer = new QWidget(splitter); // 创建右侧容器
    QVBoxLayout *rightLayout = new QVBoxLayout(rightContainer); // 创建垂直布局
    rightLayout->setContentsMargins(0, 0, 0, 0); // 设置内边距
    rightLayout->setSpacing(0); // 设置间距

    // 2.1) 上半部分：contentArea（聊天主区 + 侧栏）
    QWidget *contentArea = new QWidget; // 创建内容区域
    QHBoxLayout *contentHL = new QHBoxLayout(contentArea); // 创建水平布局
    contentHL->setContentsMargins(0, 0, 0, 0); // 设置内边距
    contentHL->setSpacing(0); // 设置间距

    // — 2.1.1) 聊天主区 ——
    QWidget *chatPanel = new QWidget; // 创建聊天面板
    QVBoxLayout *chatPanelLayout = new QVBoxLayout(chatPanel); // 创建垂直布局
    chatPanelLayout->setContentsMargins(0, 0, 0, 0); // 设置内边距
    chatPanelLayout->setSpacing(0); // 设置间距

    // —— 聊天消息滚动区 修改只需用m_scrollArea->setWidget就行---------------------------------—— 
    m_chatArea = new QWidget; // 创建聊天区域部件
    m_chatLayout = new QVBoxLayout(m_chatArea); // 创建垂直布局
    m_chatLayout->setContentsMargins(8, 8, 8, 8); // 设置内边距
    m_chatLayout->setSpacing(12); // 设置控件间距
    m_chatLayout->setAlignment(Qt::AlignTop); // 设置对齐方式为顶部对齐

    // 示例：插入一条对方示例消息（靠左，黄色气泡）
    //insertIncomingMessage("Alice", "👋 Hi there! 这是 Alice 的示例消息。");

    m_scrollArea = new QScrollArea; // 创建滚动区域
    m_scrollArea->setWidget(m_chatArea); // 设置滚动区域的部件
    m_scrollArea->setWidgetResizable(true); // 设置部件可调整大小
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded); // 垂直滚动条按需显示
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 水平滚动条始终关闭
    m_scrollArea->setFrameShape(QFrame::NoFrame); // 设置无边框

    chatPanelLayout->addWidget(m_scrollArea, 1); // 将滚动区域添加到聊天面板布局

    // — 2.1.2) 侧栏 ——
    QWidget *sidebar = new QWidget; // 创建侧栏部件
    sidebar->setFixedWidth(120); // 设置固定宽度
    sidebar->setStyleSheet("background: #f5f5f5;"); 
    QVBoxLayout *sbLayout = new QVBoxLayout(sidebar);
    sbLayout->setContentsMargins(8, 8, 8, 8);
    sbLayout->setSpacing(12);

    // 侧栏标题
    QLabel *sbTitle = new QLabel("功能");
    sbTitle->setStyleSheet("font-weight: bold; font-size: 14px;");
    sbLayout->addWidget(sbTitle);

    // 功能按钮示例
    QPushButton *btnFile    = new QPushButton("📁 文件");
    QPushButton *btnDetail  = new QPushButton("👤 好友详情");
    QPushButton *btnDelete  = new QPushButton("⚙ 设置");
    btnFile->setFixedHeight(32);
    btnDetail->setFixedHeight(32);
    btnDelete->setFixedHeight(32);

    sbLayout->addWidget(btnFile);
    sbLayout->addWidget(btnDetail);
    sbLayout->addWidget(btnDelete);
    sbLayout->addStretch(1); // 把按钮推到上边，底部留白

    // 将“聊天主区”和“侧栏”按比例放进contentHL
    contentHL->addWidget(chatPanel, 1);
    contentHL->addWidget(sidebar, 0);

    // 把 contentArea 放到 rightLayout 上层
    rightLayout->addWidget(contentArea, 1);

    // 2.2) 下半部分：输入区域（消息输入框 + 按钮）
    QWidget *bottomWidget = new QWidget;
    bottomWidget->setMinimumHeight(80);
    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomWidget);
    bottomLayout->setContentsMargins(8, 8, 8, 8);
    bottomLayout->setSpacing(6);

    m_inputEdit   = new QTextEdit;
    m_inputEdit->setPlaceholderText("请输入消息...");
    m_inputEdit->setFixedHeight(60);

    m_sendBtn     = new QPushButton("发送");
    m_sendBtn->setFixedWidth(60);

    m_simulateBtn = new QPushButton("模拟接收");
    m_simulateBtn->setFixedWidth(80);

    bottomLayout->addWidget(m_inputEdit);
    bottomLayout->addWidget(m_sendBtn);
    bottomLayout->addWidget(m_simulateBtn);

    rightLayout->addWidget(bottomWidget, 0);

    // -----------------------
    // 3) 将左右两侧加入顶层 splitter
    // -----------------------
    splitter->addWidget(m_friendList);
    splitter->addWidget(rightContainer);
    splitter->setStretchFactor(1, 1);

    QHBoxLayout *mainLay = new QHBoxLayout(central);
    mainLay->setContentsMargins(0, 0, 0, 0);
    mainLay->addWidget(splitter);

    setWindowTitle("简易聊天窗口（带侧栏）");
    resize(1000, 600);

    // ========== 信号 & 槽 ==========

    // 切换好友时清除未读气泡
    connect(m_friendList, &QListWidget::currentItemChanged,
            this, &MainWindow::onFriendSelectionChanged);

    // 点击“发送”：插入我方消息
    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::onSendClicked);

    // 点击“模拟接收”：模拟接收到 Alice 的消息
    connect(m_simulateBtn, &QPushButton::clicked, this, &MainWindow::onSimulateReceive);

    // 点击侧栏按钮只是示例：这里仅打印到控制台，实际可根据需要扩展
    connect(btnFile, &QPushButton::clicked, this, [=](){
        
        //qDebug("点击了“文件共享列表”");
        // 打开文件共享管理界面
        this->shareFileManagerWidget->show();
    });
    connect(btnDetail, &QPushButton::clicked, this, [=](){
        if(currentFriendItemWidget == nullptr){
            // qDebug("当前没有选中好友");
            return;
        }
        qDebug("点击了“好友详情”");
        LOG("点击了“好友详情",INFO);
        // 创建普通变量对话框（替换指针版本）
        QDialog dialog(this);  // 指定父窗口为 this，确保模态行为正确
        dialog.setWindowTitle("好友详情");
        dialog.setFixedSize(300, 200);
        
        auto layout = new QVBoxLayout(&dialog);  // 直接关联到 dialog
        auto lblIP   = new QLabel(QString("IP: %1").arg(QString::fromStdString(currentFriendItemWidget->getFriendInfo().getIp())), &dialog);
        auto lblMAC  = new QLabel(QString("MAC: %1").arg(QString::fromStdString(currentFriendItemWidget->getFriendInfo().getMac())), &dialog);
        auto lblUser = new QLabel(QString("用户名: %1").arg(QString::fromStdString(currentFriendItemWidget->getFriendInfo().getUserInfo().userName)), &dialog);
        auto lblPC   = new QLabel(QString("计算机名: %1").arg(QString::fromStdString(currentFriendItemWidget->getFriendInfo().getUserInfo().PCName)), &dialog);
    
        layout->addWidget(lblIP);
        layout->addWidget(lblMAC);
        layout->addWidget(lblUser);
        layout->addWidget(lblPC);
        dialog.exec();  // 替换原代码中的 setLayout 等错误调用
    });
    connect(btnDelete, &QPushButton::clicked, this, [=](){
        qDebug("点击了“设置”");
        // 打开设置窗口
        this->settingsWidget->show();
    });
    // 链接托盘点击信号
     connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
}

// MainWindow 析构函数
MainWindow::~MainWindow()
{
    //delete ui; // 删除 UI 界面对象
    delete shareFileManagerWidget;
    delete settingsWidget;
}
void MainWindow::onScrollBarValueChanged(int value)
{
    // 如果滚动条在最顶部，并且当前没有在加载
    if (value == 0 ) {
        //qDebug() << "滚动到顶部，准备加载更多历史消息...";
        //loadMoreMessages(); // 这是我们将要实现的核心函数
        // 加载好友历史信息 
    }
}

void MainWindow::onFriendSelectionChanged(QListWidgetItem *current, QListWidgetItem *previous) {
        //Q_UNUSED(previous);  // 标记previous参数未使用，避免编译器警告
        if (!current) return; // 如果当前项为空，直接返回
        // 检查是否超过容量 超过容量将好友聊天widget删除
        if (m_friendWidgets.size() >= 10) { 
            // 获取m_friendWidgets第一个项的widget
            auto it = m_friendWidgets.begin();
            delete it.value(); // 删除widget
            m_friendWidgets.erase(it); // 从m_friendWidgets中移除
        }
        // 将当前项的widget转换为FriendItemWidget类型
        auto *fw = static_cast<FriendItemWidget*>(m_friendList->itemWidget(current));
        if (fw) {
            fw->clearUnread(); // 清除未读消息提示
        }
        this->currentFriendItemWidget = fw;
        // 创建一个 FriendChatWidget，为当前FriendItemWidget的value
        FriendChatWidget *chatWidget = new FriendChatWidget();
        m_friendWidgets[current] = chatWidget; // 将新的FriendChatWidget添加到m_friendWidgets中
        m_scrollArea->setWidget(chatWidget); // 将新的FriendChatWidget设置为m_scrollArea的widget

        //从sqlite中获取聊天记录
        
        
        
    }
void MainWindow::onSendClicked() {
        QString text = m_inputEdit->toPlainText().trimmed(); // 获取输入框文本并去除首尾空格
        if (text.isEmpty()) return; // 如果文本为空，直接返回

        // 获取当前选中的好友名称
        QListWidgetItem *currentItem = m_friendList->currentItem();
        QString friendName;
        if (currentItem) {
            auto *fiw = static_cast<FriendItemWidget*>(m_friendList->itemWidget(currentItem));
            friendName = fiw->getName(); // 获取好友名称
        } else {
            friendName = "Unknown"; // 如果没有选中好友，默认为"Unknown"
        }

        insertOutgoingMessage(friendName, text); // 插入发送的消息
        m_inputEdit->clear(); // 清空输入框

        // 滚动到底部
        QScrollBar *bar = m_scrollArea->verticalScrollBar();
        bar->setValue(bar->maximum()); // 设置滚动条值为最大值（底部）
}

void MainWindow::onSimulateReceive() {
        const QString simulatedSender = "Alice"; // 模拟发送者名称
        const QString simulatedText   = "这是来自 Alice 的新消息！"; // 模拟消息内容

        // 遍历好友列表，找到Alice对应的项
        for (int i = 0; i < m_friendList->count(); ++i) {
            QListWidgetItem *item = m_friendList->item(i);
            auto *fiw = static_cast<FriendItemWidget*>(m_friendList->itemWidget(item));
            if (fiw && fiw->getName() == simulatedSender) {
                if (item == m_friendList->currentItem()) {
                    // 如果Alice是当前选中的好友，直接显示消息
                    insertIncomingMessage(simulatedSender, simulatedText);
                    QScrollBar *bar = m_scrollArea->verticalScrollBar();
                    bar->setValue(bar->maximum()); // 滚动到底部
                } else {
                    // 如果不是当前选中的好友，增加未读气泡
                    fiw->incrementUnread();
                }
                break; // 找到后跳出循环
            }
        }
    }
void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    // 没有启用左键单击托盘打开主界面
    if(!this->getSettingsWidget()->getSystemSettingsPage()->getTrayClickOpenMainCheckBox()){
        return;
    }
    if (reason == QSystemTrayIcon::Trigger) {
        
        // 显示所有窗口
        foreach (QWidget* widget, widgetList) {
            if (widget && !widget->isVisible()) {
                widget->show();
                widget->setWindowState(widget->windowState() & ~Qt::WindowMinimized);
            }
        }
        
        // 将最后一个窗口设为顶级窗口
        if (!widgetList.isEmpty()) {
            QWidget* lastWidget = widgetList.last();
            lastWidget->raise();
            lastWidget->activateWindow();
            // 如果需要确保窗口置顶，可以添加
            // lastWidget->setWindowFlags(lastWidget->windowFlags() | Qt::WindowStaysOnTopHint);
            // lastWidget->show();
        }
        
    }
}
    void MainWindow::insertOutgoingMessage(const QString &friendName, const QString &content)
    {
        // 1) 创建头部标签 [时间] Me → 好友名
        QString timeStr = QDateTime::currentDateTime().toString("HH:mm:ss"); // 获取当前时间
        QLabel *header = new QLabel(QString("[%1] %2").arg(timeStr).arg("Me → " + friendName));
        header->setStyleSheet("color: #555; font-size:12px;"); // 设置样式

        QWidget *headerContainer = new QWidget; // 创建头部容器
        QHBoxLayout *headerHL = new QHBoxLayout(headerContainer); // 创建水平布局
        headerHL->setContentsMargins(0, 0, 0, 0); // 设置内边距
        headerHL->setSpacing(0); // 设置间距
        headerHL->addStretch(1);    // 添加伸缩项，使头部右对齐
        headerHL->addWidget(header); // 添加头部标签

        // 2) 创建气泡正文（蓝色背景）
        QLabel *bubble = new QLabel(content); // 创建内容标签
        bubble->setWordWrap(true); // 设置自动换行
        bubble->setStyleSheet(
            "background:#d0eaff; "
            "padding:6px; "
            "border-radius:4px; "
            "font-size:14px;"
        ); // 设置气泡样式
        QWidget *bubbleContainer = new QWidget; // 创建气泡容器
        QHBoxLayout *bubbleHL = new QHBoxLayout(bubbleContainer); // 创建水平布局
        bubbleHL->setContentsMargins(0, 0, 0, 0); // 设置内边距
        bubbleHL->setSpacing(0); // 设置间距
        bubbleHL->addStretch(1);      // 添加伸缩项，使气泡右对齐
        bubbleHL->addWidget(bubble); // 添加气泡标签

        // 将头部和气泡添加到聊天布局
        m_chatLayout->addWidget(headerContainer);
        m_chatLayout->addWidget(bubbleContainer);
    }

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) { // 检查是否是双击
        this->hide(); // 隐藏窗口
    }
}

void MainWindow::insertIncomingMessage(const QString &sender, const QString &content) {
        // 1) 创建头部标签 [时间] 发送者
        QString timeStr = QDateTime::currentDateTime().toString("HH:mm:ss"); // 获取当前时间并格式化
        QLabel *header = new QLabel(QString("[%1] %2").arg(timeStr).arg(sender)); // 创建标签
        header->setStyleSheet("color: #555; font-size:12px;"); // 设置样式

        QWidget *headerContainer = new QWidget; // 创建头部容器
        QHBoxLayout *headerHL = new QHBoxLayout(headerContainer); // 创建水平布局
        headerHL->setContentsMargins(0, 0, 0, 0); // 设置内边距为0
        headerHL->setSpacing(0); // 设置控件间距为0
        headerHL->addWidget(header); // 添加头部标签
        headerHL->addStretch(1);  // 添加伸缩项，使头部左对齐

        // 2) 创建气泡正文（黄色背景）
        QLabel *bubble = new QLabel(content); // 创建内容标签
        bubble->setWordWrap(true); // 设置自动换行
        bubble->setStyleSheet(
            "background: #fff1a8; "
            "padding:6px; "
            "border-radius:4px; "
            "font-size:14px;"
        ); // 设置气泡样式
        QWidget *bubbleContainer = new QWidget; // 创建气泡容器
        QHBoxLayout *bubbleHL = new QHBoxLayout(bubbleContainer); // 创建水平布局
        bubbleHL->setContentsMargins(0, 0, 0, 0); // 设置内边距
        bubbleHL->setSpacing(0); // 设置间距
        bubbleHL->addWidget(bubble); // 添加气泡标签
        bubbleHL->addStretch(1);  // 添加伸缩项，使气泡左对齐

        // 3) 将头部和气泡添加到聊天布局
        m_chatLayout->addWidget(headerContainer);
        m_chatLayout->addWidget(bubbleContainer);
    }