#include "downloader.h"

std::vector<char> Downloader::download(const char* fileName, int blockId)
{
    
    DownloadFileBlock downloadFileBlock;
    downloadFileBlock.fileName = fileName;
    downloadFileBlock.blockId = blockId;
    std::vector<char> buffer = downloadFileBlock.serialize();
}
