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

std::string APIHelper::calculate_broadcast(const std::string &cidr)
{
    size_t slash_pos = cidr.find('/');
    if (slash_pos == std::string::npos) {
        throw std::invalid_argument("Invalid CIDR format");
    }

    std::string ip_part = cidr.substr(0, slash_pos);
    std::string prefix_part = cidr.substr(slash_pos + 1);

    // 假设 parse_ipv4 是 APIHelper 类的成员函数，这里使用 this 指针来调用
    uint32_t ip = parse_ipv4(ip_part);
    int prefix = std::stoi(prefix_part);

    if (prefix < 0 || prefix > 32) {
        throw std::invalid_argument("Invalid prefix length");
    }

    // 计算广播地址
    uint32_t mask = 0xFFFFFFFF << (32 - prefix);
    uint32_t network = ip & mask;
    uint32_t broadcast = network | (~mask);

    return ip_to_string(broadcast);
}

std::string APIHelper::ip_to_string(uint32_t ip)
{
    return std::to_string((ip >> 24) & 0xFF) + "." +
           std::to_string((ip >> 16) & 0xFF) + "." +
           std::to_string((ip >> 8) & 0xFF) + "." +
           std::to_string(ip & 0xFF);
}

uint32_t APIHelper::parse_ipv4(const std::string &ip_str)
{
    uint32_t ip = 0;
    std::istringstream iss(ip_str);
    std::string part;
    int octet = 0;

    while (std::getline(iss, part, '.') && octet < 4) {
        try {
            int value = std::stoi(part);
            if (value < 0 || value > 255) {
                throw std::invalid_argument("Invalid octet value");
            }
            ip = (ip << 8) | value;
            octet++;
        } catch (...) {
            throw std::invalid_argument("Invalid IP address format");
        }
    }

    if (octet != 4) {
        throw std::invalid_argument("IPv4 address must have exactly 4 octets");
    }

    return ip;
}
