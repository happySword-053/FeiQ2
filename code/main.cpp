#include "mainwindow/mainwindow.h"
#include"task/TaskDispatcher.h"
#include<memory>
#include <QApplication>
void initFront(std::shared_ptr<MainWindow> w,std::shared_ptr<TaskDispatcher> taskDispatcher){
    taskDispatcher->setMainWindow(w);
    /*----设置界面conncet------*/
    // 个人设置界面
    QObject::connect(w->getSettingsWidget()->getPersonalInfoSettingsPage(),
                     &PersonalInfoSettingsPage::confirm,
                    taskDispatcher.get(),
                    &TaskDispatcher::personalInfoSettings);
    // 系统设置界面
    QObject::connect(w->getSettingsWidget()->getSystemSettingsPage(),
                     &SystemSettingsPage::confirm,
                    taskDispatcher.get(),
                    &TaskDispatcher::systemSettings);
    // 网络设置界面
    QObject::connect(w->getSettingsWidget()->getNetworkSettingsPage(),
                     &NetworkSettingsPage::confirm,
                    taskDispatcher.get(),
                    &TaskDispatcher::networkSettings);
    // 功能设置界面
    QObject::connect(w->getSettingsWidget()->getFunctionSettingsPage(),
                     &FunctionSettingsPage::confirm,
                    taskDispatcher.get(),
                    &TaskDispatcher::functionSettings);
}
int main(int argc, char *argv[])
{
    std::shared_ptr<TaskDispatcher> taskDispatcher = std::make_shared<TaskDispatcher>();
    QApplication a(argc, argv);
    std::shared_ptr<MainWindow> w(new MainWindow());
    w->show();
    return a.exec();
}
