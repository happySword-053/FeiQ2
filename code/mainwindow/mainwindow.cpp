#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 初始化窗口
    //设置固定大小
    setFixedSize(300, 600);
    //设置窗口标题
    setWindowTitle("FeiQ");
    setWindowFlags(Qt::FramelessWindowHint);//无边框
    //设置窗口图标
    setWindowIcon(QIcon(":/main_icon/mainIcon.png"));
    // 设置关闭时自动删除
    // setAttribute(Qt::WA_DeleteOnClose);
    // 必须设置具体颜色值
    ui->close_mainwindow->setUseFlag(true);
    ui->mininum_mainwindow->setUseFlag(true);
    //初始化托盘
    initTrayIcon();
    //处理关闭按钮和最小化的鼠标事件
    connect(ui->close_mainwindow, &MyLabel::def_signal, this,[=](){
        //询问是否退出
        if(QMessageBox::question(this, "退出", "确定要退出程序吗？") == QMessageBox::Yes) {
            QApplication::quit(); // 或者 qApp->quit()
        }
    });
    connect(ui->mininum_mainwindow, &MyLabel::def_signal, this,QMainWindow::hide);
}

MainWindow::~MainWindow()
{
    //qDebug() << "maindow del";
    delete ui;
}

void MainWindow::initTrayIcon()
{
    // 初始化托盘
    this->trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/main_icon/mainIcon.png"));
    trayIcon->setToolTip("FeiQ2 V1.0");
    // 初始化托盘菜单
    this->trayMenu = new QMenu(this);
    // 添加菜单项
    QAction *showAction = new QAction("显示主界面", this);
    QAction *quitAction = new QAction("退出", this);
    trayMenu->addAction(showAction);
    trayMenu->addSeparator(); // 添加分隔线
    trayMenu->addAction(quitAction);
    // 设置托盘菜单
    trayIcon->setContextMenu(trayMenu);
    // 连接信号和槽
    connect(showAction, &QAction::triggered, this, [=]() {
        this->show();          // 1. 显示被隐藏的主窗口
        this->activateWindow();// 2. 激活窗口到前台（获取焦点）
        this->raise();         // 3. 置顶窗口防止被遮挡
    });
    connect(quitAction, &QAction::triggered, this, [=](){
        // 添加退出确认对话框
        if(QMessageBox::question(this, "退出", "确定要退出程序吗？") == QMessageBox::Yes) {
            QApplication::quit(); // 或者 qApp->quit()
        }
    });
    trayIcon->show(); // 必须显式调用show()，否则图标不会显示
}
