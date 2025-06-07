#include "DataProcess.h"

void DataProcess::derive_key(unsigned char key[EVP_MAX_KEY_LENGTH])
{
    // 注意：这里我们传一个“dummy_iv”给 EVP_BytesToKey，但不把它当作真正的 IV 用
    
    unsigned char dummy_iv[EVP_MAX_IV_LENGTH] = {0};
    
    const EVP_CIPHER* cipher = EVP_aes_256_ctr();
    if (!EVP_BytesToKey(
            cipher,                  // 使用 AES-256-CTR 算法的 Key 长度信息
            EVP_sha256(),            // 摘要算法
            nullptr,                 // 不使用 salt
            reinterpret_cast<const unsigned char*>(password),
            strlen(password),
            1,                       // 一次迭代
            key,                     // 输出：派生出的 key
            dummy_iv                 // 输出：派生出的 iv，但此处只是“丢弃”
        )) {
        handle_openssl_error();
    }
    // 派生完毕后，我们只用 key，dummy_iv 直接丢弃
    OPENSSL_cleanse(dummy_iv, sizeof(dummy_iv));
}

std::vector<char> DataProcess::encrypt(const std::vector<char> &plaintext)
{
    // 1. 派生密钥（不变）
    unsigned char key[EVP_MAX_KEY_LENGTH];
    derive_key(key);

    // 2. 随机生成一个真正的 IV（CTR 模式 IV 长度通常是 16 字节）
    int iv_len = EVP_CIPHER_iv_length(EVP_aes_256_ctr());
    std::vector<char> iv(iv_len);  // 改为 vector<char>
    if (!RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), iv_len)) {  // 转换指针类型
        handle_openssl_error();
    }

    // 3. 创建加密上下文（不变）
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handle_openssl_error();

    // 4. 初始化加密（不变）
    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key, 
                           reinterpret_cast<unsigned char*>(iv.data()))) {  // 转换指针类型
        handle_openssl_error();
    }

    // 5. 执行加密（密文类型改为 vector<char>）
    std::vector<char> ciphertext(plaintext.size());  // 改为 vector<char>
    int out_len = 0;
    if (!EVP_EncryptUpdate(
            ctx,
            reinterpret_cast<unsigned char*>(ciphertext.data()),  // 转换指针类型
            &out_len,
            reinterpret_cast<const unsigned char*>(plaintext.data()),
            static_cast<int>(plaintext.size()))) {
        handle_openssl_error();
    }

    // 6. 清理上下文（不变）
    EVP_CIPHER_CTX_free(ctx);

    // 7. 前缀 IV（合并为 vector<char>）
    std::vector<char> result;
    result.reserve(iv_len + ciphertext.size());
    result.insert(result.end(), iv.begin(), iv.end());  // iv 已为 vector<char>
    result.insert(result.end(), ciphertext.begin(), ciphertext.end());

    // 8. 清除敏感数据（不变）
    OPENSSL_cleanse(key, sizeof(key));
    OPENSSL_cleanse(iv.data(), iv.size());

    return result;
}

std::vector<char> DataProcess::decrypt(const std::vector<char>& input)
{
    int iv_len = EVP_CIPHER_iv_length(EVP_aes_256_ctr());
    if (static_cast<int>(input.size()) < iv_len) {
        handle_openssl_error();
        throw std::runtime_error("Invalid ciphertext format");
       // exit(EXIT_FAILURE);
    }

    // 1. 提取 IV（从 vector<char> 转换）
    std::vector<char> iv(input.begin(), input.begin() + iv_len);  // 直接截取 input 的前 iv_len 字节

    // 2. 提取密文（改为 vector<char>）
    std::vector<char> ciphertext(input.begin() + iv_len, input.end());  // 直接截取剩余部分

    // 3. 派生密钥（不变）
    unsigned char key[EVP_MAX_KEY_LENGTH];
    derive_key(key);

    // 4. 创建解密上下文（不变）
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handle_openssl_error();

    // 5. 初始化解密（转换 IV 指针类型）
    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key,
                           reinterpret_cast<unsigned char*>(iv.data()))) {  // 转换指针类型
        handle_openssl_error();
    }

    // 6. 执行解密（明文类型改为 vector<char>）
    std::vector<char> plaintext(ciphertext.size());  // 改为 vector<char>
    int out_len = 0;
    if (!EVP_DecryptUpdate(
            ctx,
            reinterpret_cast<unsigned char*>(plaintext.data()),  // 转换指针类型
            &out_len,
            reinterpret_cast<const unsigned char*>(ciphertext.data()),
            static_cast<int>(ciphertext.size()))) {
        handle_openssl_error();
    }

    // 7. 清理上下文（不变）
    EVP_CIPHER_CTX_free(ctx);
    plaintext.resize(out_len);

    // 8. 清除敏感数据（不变）
    OPENSSL_cleanse(key, sizeof(key));
    OPENSSL_cleanse(iv.data(), iv.size());

    return plaintext;
}
