#ifndef MYLABEL_H
#define MYLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QString>
//自定义标签类，用于处理鼠标事件
class MyLabel : public QLabel
{
    Q_OBJECT
private:
    bool use_flag;//是否使用鼠标事件
    QString normal_color;//正常颜色
    QString press_color;//按下颜色
    QString hover_color;//悬浮颜色
protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    //鼠标按下事件
    void mousePressEvent(QMouseEvent *ev) override;
    //鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *ev) override;
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *ev) override;
public:
    explicit MyLabel(QWidget *parent = nullptr);
    ~MyLabel();
    //设置是否使用鼠标事件
    void setUseFlag(bool flag){use_flag = flag;}
    //设置颜色
    void setColor(QString normal, QString press, QString hover){
        normal_color = normal;
        press_color = press;
        hover_color = hover;
    }
signals:
    //鼠标按下信号
    void def_signal();
};

#endif // MYLABEL_H