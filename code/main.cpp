#include "mainwindow/mainwindow.h"
#include"MyControl/MetaDataDefine.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<std::vector<char>>("std::vector<char>");
    qRegisterMetaType<std::vector<char> const &>("std::vector<char> const &");
    MainWindow w;
    w.show();
    return a.exec();
}
