#include "sharefilemanagerwidget.h"

ShareFileManagerWidget::ShareFileManagerWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    setupConnections();

    // 填充一些模拟数据
    dummyFiles << SharedFileInfo{"文档A.pdf", "PDF", "2.5MB", "2024-05-01 10:30", 5, "张三"};
    dummyFiles << SharedFileInfo{"图片B.jpg", "JPG", "1.2MB", "2024-05-15 14:00", 12, "李四"};
    dummyFiles << SharedFileInfo{"代码C.zip", "ZIP", "5.8MB", "2024-06-01 09:00", 3, "王五"};
    populateFileList(dummyFiles);
}

ShareFileManagerWidget::~ShareFileManagerWidget()
{
}

void ShareFileManagerWidget::setupUi()
{
    // 设置窗口标题和大小
    setWindowTitle("文件共享管理");
    resize(800, 600); // 可以根据需要调整大小

    // 1. 标题标签
    titleLabel = new QLabel("文件共享列表", this);
    titleLabel->setAlignment(Qt::AlignCenter); // 居中显示
    // 可以设置字体样式
    QFont font = titleLabel->font();
    font.setPointSize(12);
    font.setBold(true);
    titleLabel->setFont(font);
    titleLabel->setStyleSheet("QLabel { background-color: #E0FFE0; padding: 5px; border: 1px solid #C0DCC0; }");


    // 2. 文件列表表格
    fileTableWidget = new QTableWidget(this);
    fileTableWidget->setColumnCount(6); // 6列
    fileTableWidget->setHorizontalHeaderLabels({"名称(可双击打开目录)", "类型", "大小", "修改时间", "下载次数", "来自于"});
    fileTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); // 整行选择
    fileTableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection); // 允许多选
    fileTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 不可编辑
    // 调整列宽以适应内容
    fileTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // 最后一列填充可用空间
    fileTableWidget->horizontalHeader()->setStretchLastSection(true);
    // 隐藏行号
    fileTableWidget->verticalHeader()->setVisible(false);

    // 3. 底部按钮
    setMySharedFilesButton = new QPushButton("设置我的共享文件", this);
    selectAllButton = new QPushButton("全选", this);
    unselectAllButton = new QPushButton("全不选", this);
    refreshButton = new QPushButton("刷新", this);
    downloadButton = new QPushButton("下载", this);
    exitButton = new QPushButton("退出", this);

    // 底部按钮布局
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(); // 伸缩器，将按钮推向左侧
    buttonLayout->addWidget(setMySharedFilesButton);
    buttonLayout->addWidget(selectAllButton);
    buttonLayout->addWidget(unselectAllButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(downloadButton);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addStretch(); // 伸缩器，将按钮推向右侧

    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(fileTableWidget);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
}

void ShareFileManagerWidget::setupConnections()
{
    connect(setMySharedFilesButton, &QPushButton::clicked, this, &ShareFileManagerWidget::on_setMySharedFiles_clicked);
    connect(selectAllButton, &QPushButton::clicked, this, &ShareFileManagerWidget::on_selectAll_clicked);
    connect(unselectAllButton, &QPushButton::clicked, this, &ShareFileManagerWidget::on_unselectAll_clicked);
    connect(refreshButton, &QPushButton::clicked, this, &ShareFileManagerWidget::on_refresh_clicked);
    connect(downloadButton, &QPushButton::clicked, this, &ShareFileManagerWidget::on_download_clicked);
    connect(exitButton, &QPushButton::clicked, this, &ShareFileManagerWidget::on_exit_clicked);

    // 双击表格项的信号处理
    connect(fileTableWidget, &QTableWidget::itemDoubleClicked, this, [](QTableWidgetItem* item){
        if (item) {
            qDebug() << "双击了文件：" << item->text();
            // 在这里可以实现打开文件所在目录的逻辑
        }
    });
}

void ShareFileManagerWidget::populateFileList(const QList<SharedFileInfo>& files)
{
    fileTableWidget->setRowCount(0); // 清空现有行
    fileTableWidget->setRowCount(files.size());

    for (int i = 0; i < files.size(); ++i) {
        const SharedFileInfo& info = files.at(i);
        fileTableWidget->setItem(i, 0, new QTableWidgetItem(info.name));
        fileTableWidget->setItem(i, 1, new QTableWidgetItem(info.type));
        fileTableWidget->setItem(i, 2, new QTableWidgetItem(info.size));
        fileTableWidget->setItem(i, 3, new QTableWidgetItem(info.modifyTime));
        fileTableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(info.downloadCount)));
        fileTableWidget->setItem(i, 5, new QTableWidgetItem(info.from));
    }
}

// 槽函数实现 (目前只做调试输出)
void ShareFileManagerWidget::on_setMySharedFiles_clicked()
{
    qDebug() << "设置我的共享文件 按钮被点击！";
    // 实现设置共享文件的逻辑，例如打开一个文件选择对话框
}

void ShareFileManagerWidget::on_selectAll_clicked()
{
    qDebug() << "全选 按钮被点击！";
    fileTableWidget->selectAll(); // 选择所有行
}

void ShareFileManagerWidget::on_unselectAll_clicked()
{
    qDebug() << "全不选 按钮被点击！";
    fileTableWidget->clearSelection(); // 清除所有选择
}

void ShareFileManagerWidget::on_refresh_clicked()
{
    qDebug() << "刷新 按钮被点击！";
    // 实现刷新文件列表的逻辑，例如重新加载数据
    // 模拟刷新：
    dummyFiles.clear();
    dummyFiles << SharedFileInfo{"新文档X.docx", "DOCX", "1.1MB", "2025-01-01 11:00", 1, "赵六"};
    dummyFiles << SharedFileInfo{"测试报告.xlsx", "XLSX", "3.0MB", "2025-01-05 16:30", 0, "孙七"};
    populateFileList(dummyFiles);
}

void ShareFileManagerWidget::on_download_clicked()
{
    qDebug() << "下载 按钮被点击！";
    // 实现下载逻辑，获取当前选中的文件
    QList<QTableWidgetItem*> selectedItems = fileTableWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        qDebug() << "没有选择任何文件进行下载。";
        return;
    }
    qDebug() << "准备下载以下文件：";
    // selectedItems 包含的是所有选中单元格，需要去重或按行处理
    QSet<int> selectedRows;
    for (QTableWidgetItem* item : selectedItems) {
        selectedRows.insert(item->row());
    }
    for (int row : selectedRows) {
        qDebug() << " - " << fileTableWidget->item(row, 0)->text();
    }
}

void ShareFileManagerWidget::on_exit_clicked()
{
    qDebug() << "退出 按钮被点击！";
    this->close(); // 关闭窗口
}