/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include "../../../code/MyControl/MyLabel/mylabel.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    MyLabel *close_mainwindow;
    MyLabel *mininum_mainwindow;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->setEnabled(true);
        MainWindow->resize(300, 600);
        MainWindow->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        close_mainwindow = new MyLabel(centralwidget);
        close_mainwindow->setObjectName("close_mainwindow");
        close_mainwindow->setGeometry(QRect(270, 0, 30, 30));
        close_mainwindow->setFrameShape(QFrame::Shape::Box);
        close_mainwindow->setAlignment(Qt::AlignmentFlag::AlignCenter);
        mininum_mainwindow = new MyLabel(centralwidget);
        mininum_mainwindow->setObjectName("mininum_mainwindow");
        mininum_mainwindow->setGeometry(QRect(240, 0, 30, 30));
        mininum_mainwindow->setFrameShape(QFrame::Shape::Box);
        mininum_mainwindow->setAlignment(Qt::AlignmentFlag::AlignCenter);
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        close_mainwindow->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p>X</p></body></html>", nullptr));
        mininum_mainwindow->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p>-</p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
