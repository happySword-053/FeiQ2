#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMessageBox>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initTrayIcon();  // 初始化托盘所有操作
private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon = nullptr;  // 托盘图标对象
    QMenu *trayMenu = nullptr;            // 托盘右键菜单
};
#endif // MAINWINDOW_H
