#include "mylabel.h"
MyLabel::MyLabel(QWidget *parent):QLabel(parent)
{
    use_flag = false;
    // 初始化默认颜色
    normal_color = "lightblue";
    press_color = "#9FC2FF";
    hover_color = "#7FA1FF";
    // 设置基础样式
    setStyleSheet("background-color: lightblue;");
}

// 修改 enterEvent 和 leaveEvent
void MyLabel::enterEvent(QEnterEvent *event)
{
    if(use_flag) {
        setStyleSheet(QString("background-color: %1;")
            .arg(hover_color)); // 同时设置背景和文字颜色
    }
    QLabel::enterEvent(event);
}

void MyLabel::leaveEvent(QEvent *event)
{
    if(use_flag) {
        setStyleSheet(QString("background-color: %1;")
            .arg(normal_color));
    }
    QLabel::leaveEvent(event);
}

MyLabel::~MyLabel()
{
    //qDebug() << "mylabel del";
}


void MyLabel::mousePressEvent(QMouseEvent *ev)
{

    if(!use_flag) return;
    if(ev->button() == Qt::LeftButton ){
        //设置背景颜色
        setStyleSheet("background-color: " + press_color);
        emit def_signal();
    }
    //调用父类的鼠标按下事件
    QLabel::mousePressEvent(ev);
}

void MyLabel::mouseReleaseEvent(QMouseEvent *ev)
{

    if(!use_flag) return;
    //设置背景颜色  
    setStyleSheet("background-color: " + hover_color + ";");
    //调用父类的鼠标释放事件
    QLabel::mouseReleaseEvent(ev);
}

void MyLabel::mouseMoveEvent(QMouseEvent *ev)
{

    if(!use_flag) return;
    
    //设置背景颜色
    setStyleSheet("background-color: " + hover_color);
    //调用父类的鼠标移动事件
    QLabel::mouseMoveEvent(ev);
}

