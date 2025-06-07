#pragma once
#include<openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include<string>
#include<vector>
#include <cstring>  // for memcpy
#include <stdexcept>

// 错误处理宏
#define handle_openssl_error() \
    do { \
        ERR_print_errors_fp(stderr); \
        exit(EXIT_FAILURE); \
    } while (0)

//数据加密解密模块
class DataProcess {
private:
    const char* password = "MTM1NTI4NTAyN0BxcS5jb20=";

    void derive_key(unsigned char key[EVP_MAX_KEY_LENGTH]);//从密码派生出 Key（不覆盖 IV），这里只用 EVP_BytesToKey 生成密钥，EVP_BytesToKey 的 IV 输出部分我们不要用它来实际“解/加密”，而是由随机 IV 或串前缀来决定
public:
    std::vector<char> encrypt(const std::vector<char>& plaintext);//加密函数
    std::vector<char> decrypt(const std::vector<char>& ciphertext);//解密函数
};