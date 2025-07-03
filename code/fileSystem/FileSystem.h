#pragma once
#include"uploader/uploader.h"
#include"downloader/downloader.h"
#include<QObject>
#include"../threadPool/ThreadPool.h"
#include<vector>

class FileSystem : public QObject {
    Q_OBJECT
private:
    Uploader uploader; // 上传器对象
    Downloader downloader; // 下载器对象


signals:
    

public slots:

};