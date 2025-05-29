#include <iostream>
#include <vector>
#include <string>
#include <windows.h>

int main() {
    // 获取计算机名（ANSI 版本）
    DWORD compNameLen = MAX_COMPUTERNAME_LENGTH + 1;
    std::vector<char> compName(compNameLen);
    if (GetComputerNameA(compName.data(), &compNameLen)) {
        std::string computerName(compName.data());
        std::cout << "Computer Name: " << computerName << std::endl;
    } else {
        std::cerr << "GetComputerName failed, error: " << GetLastError() << std::endl;
    }

    // 获取用户名（ANSI 版本）
    DWORD userNameLen = 256;
    std::vector<char> userName(userNameLen);
    if (GetUserNameA(userName.data(), &userNameLen)) {
        std::string userNameStr(userName.data());
        std::cout << "User Name: " << userNameStr << std::endl;
    } else {
        std::cerr << "GetUserName failed, error: " << GetLastError() << std::endl;
    }

    return 0;
}