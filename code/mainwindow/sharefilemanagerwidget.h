#ifndef SHAREFILEMANAGERWIDGET_H
#define SHAREFILEMANAGERWIDGET_H

#include <QWidget>
#include <QTableWidget> // 用于显示文件列表
#include <QPushButton>  // 底部按钮
#include <QHBoxLayout>  // 水平布局
#include <QVBoxLayout>  // 垂直布局
#include <QLabel>       // 标题
#include <QHeaderView>  // 表格头部
#include <QDebug>       // 调试输出

// 假设的文件信息结构体
struct SharedFileInfo {
    QString name;
    QString type;
    QString size;
    QString modifyTime;
    int downloadCount;
    QString from;
};

class ShareFileManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ShareFileManagerWidget(QWidget *parent = nullptr);
    ~ShareFileManagerWidget();

private slots:
    void on_setMySharedFiles_clicked();
    void on_selectAll_clicked();
    void on_unselectAll_clicked();
    void on_refresh_clicked();
    void on_download_clicked();
    void on_exit_clicked();

private:
    void setupUi();
    void setupConnections();
    void populateFileList(const QList<SharedFileInfo>& files); // 填充文件列表数据

    QLabel* titleLabel;
    QTableWidget* fileTableWidget;
    QPushButton* setMySharedFilesButton;
    QPushButton* selectAllButton;
    QPushButton* unselectAllButton;
    QPushButton* refreshButton;
    QPushButton* downloadButton;
    QPushButton* exitButton;

    // 示例数据
    QList<SharedFileInfo> dummyFiles;
};

#endif // SHAREFILEMANAGERWIDGET_H