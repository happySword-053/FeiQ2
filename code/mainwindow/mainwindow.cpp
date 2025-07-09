#include "mainwindow.h"


// MainWindow 构造函数
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, ui(new Ui::MainWindow)
{
    //ui->setupUi(this); // 初始化UI界面 (由 .ui 文件生成)
    // 中央 Widget
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // 整体水平分割：左侧好友列表 + 右侧（再拆分为聊天主区 + 侧栏）
    QSplitter *splitter = new QSplitter(Qt::Horizontal, central);

    //文件共享管理界面
    this->shareFileManagerWidget = new ShareFileManagerWidget();
    //设置窗口
    this->settingsWidget = new SettingsWidget();
    
    // -----------------------
    // 1) 左侧：好友列表
    // -----------------------
    m_friendList = new QListWidget(splitter);
    m_friendList->setMinimumWidth(160);
    m_friendList->setFrameShape(QFrame::NoFrame);
    m_friendList->setStyleSheet(
        "QListWidget { background: #fafafa; }"
        "QListWidget::item:selected { background: #d0e7ff; }"
    );

    // 示例好友
    QStringList names = { "Alice", "Bob", "Charlie", "David", "Eve" };
    for (const QString &name : names) {
        QListWidgetItem *item = new QListWidgetItem(m_friendList);
        item->setSizeHint(QSize(0, 48));
        m_friendList->addItem(item);

        FriendItemWidget *w = new FriendItemWidget(name);
        m_friendList->setItemWidget(item, w);
    }
    m_friendList->setCurrentRow(0);

    // 切换好友时清空未读
    connect(m_friendList, &QListWidget::currentItemChanged,
            this, &MainWindow::onFriendSelectionChanged);

    // -----------------------
    // 2) 右侧整体：容器 rightContainer
    //    — 上：contentArea（水平布局：聊天主区 + 侧栏）
    //    — 下：输入区域
    // -----------------------
    QWidget *rightContainer = new QWidget(splitter);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    // 2.1) 上半部分：contentArea（聊天主区 + 侧栏）
    QWidget *contentArea = new QWidget;
    QHBoxLayout *contentHL = new QHBoxLayout(contentArea);
    contentHL->setContentsMargins(0, 0, 0, 0);
    contentHL->setSpacing(0);

    // — 2.1.1) 聊天主区 ——
    QWidget *chatPanel = new QWidget;                // 聊天主区容器
    QVBoxLayout *chatPanelLayout = new QVBoxLayout(chatPanel);
    chatPanelLayout->setContentsMargins(0, 0, 0, 0);
    chatPanelLayout->setSpacing(0);

    // —— 聊天消息滚动区 —— 
    m_chatArea = new QWidget;
    m_chatLayout = new QVBoxLayout(m_chatArea);
    m_chatLayout->setContentsMargins(8, 8, 8, 8);
    m_chatLayout->setSpacing(12);
    m_chatLayout->setAlignment(Qt::AlignTop);

    // 示例：插入一条对方示例消息（靠左，黄色气泡）
    insertIncomingMessage("Alice", "👋 Hi there! 这是 Alice 的示例消息。");

    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidget(m_chatArea);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    chatPanelLayout->addWidget(m_scrollArea, 1);

    // — 2.1.2) 侧栏 ——
    QWidget *sidebar = new QWidget;
    sidebar->setFixedWidth(120);
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
        qDebug("点击了“文件共享列表”");
        // 打开文件共享管理界面
        this->shareFileManagerWidget->show();
    });
    connect(btnDetail, &QPushButton::clicked, this, [](){
        qDebug("点击了“好友详情”");
    });
    connect(btnDelete, &QPushButton::clicked, this, [=](){
        qDebug("点击了“设置”");
        // 打开设置窗口
        this->settingsWidget->show();
    });
}

// MainWindow 析构函数
MainWindow::~MainWindow()
{
    //delete ui; // 删除 UI 界面对象
    delete shareFileManagerWidget;
    delete settingsWidget;
}
