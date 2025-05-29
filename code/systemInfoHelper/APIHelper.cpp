#include "APIHelper.h"

std::string APIHelper::WCharToString(const wchar_t *wstr)
{
    if (!wstr) return {};  // 空指针直接返回空字符串
    // 计算转换所需的字节数（含终止符）
    int size_needed = WideCharToMultiByte(
        CP_ACP,          // 使用系统默认ANSI代码页（如GBK）
        0,               // 转换标志（无特殊处理）
        wstr,            // 输入宽字符指针
        -1,              // 自动计算字符串长度（含终止符）
        nullptr,         // 输出缓冲区（先获取长度）
        0,               // 缓冲区大小（0表示仅获取长度）
        nullptr,         // 未转换字符的默认替代（不处理）
        nullptr          // 是否使用默认替代字符（不处理）
    );
    // 分配足够大小的缓冲区
    std::vector<char> buf(size_needed);
    // 实际执行宽字符到多字节的转换
    WideCharToMultiByte(
        CP_ACP,          // 代码页同上
        0,               // 转换标志同上
        wstr,            // 输入宽字符指针
        -1,              // 自动计算长度
        buf.data(),      // 输出缓冲区
        size_needed,     // 缓冲区大小
        nullptr,         // 未转换字符的默认替代
        nullptr          // 是否使用默认替代字符
    );
    return std::string(buf.data());  // 转换为std::string返回
}
