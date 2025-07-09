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
std::vector<char> DataProcess::compress_vector(const std::vector<char>& input) {
    uLong src_len = static_cast<uLong>(input.size());
    // 1. 计算最大可能的压缩后长度
    uLong bound = compressBound(src_len);

    // 2. 准备输出缓冲区，初始大小为 bound
    std::vector<char> out(bound);

    // 3. 调用 zlib 的一次性压缩接口 compress2
    int ret = compress2(
        reinterpret_cast<Bytef*>(out.data()),   // 输出缓冲的指针
        &bound,                                  // 输入时代表缓冲区大小，输出后为实际写入字节数
        reinterpret_cast<const Bytef*>(input.data()), // 输入数据指针
        src_len,                                 // 输入数据长度
        Z_BEST_COMPRESSION                       // 压缩级别：最高压缩比
    );

    // 4. 错误处理：非 Z_OK 则抛异常
    if (ret != Z_OK) {
        throw std::runtime_error("compress2 failed: error code " + std::to_string(ret));
    }

    // 5. 将输出 vector 大小缩到真实写入的 bound（由 compress2 更新）
    out.resize(bound);
    return out;
}


std::vector<char> DataProcess::decompress_vector(const std::vector<char>& compressed_data) {
    // 1. 初始化 z_stream 结构体
    z_stream strm;
    std::memset(&strm, 0, sizeof(strm));
    if (inflateInit(&strm) != Z_OK) {
        throw std::runtime_error("zlib inflateInit failed");
    }

    // 2. 准备输出 vector，按块增长
    std::vector<char> out;
    const size_t chunk_size = 16384;  // 每次解压 16 KB
    out.reserve(chunk_size);

    // 3. 将输入数据指向 z_stream
    strm.next_in  = reinterpret_cast<Bytef*>(const_cast<char*>(compressed_data.data()));
    strm.avail_in = static_cast<uInt>(compressed_data.size());

    int ret;
    do {
        // 4. 每次循环前在 out 末尾增加 chunk_size 空间
        size_t old_size = out.size();
        out.resize(old_size + chunk_size);

        // 5. 设置 z_stream 的输出指针和可用空间
        strm.next_out  = reinterpret_cast<Bytef*>(out.data() + old_size);
        strm.avail_out = static_cast<uInt>(chunk_size);

        // 6. 调用 inflate 解压一块数据
        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
            inflateEnd(&strm);
            throw std::runtime_error("inflate failed with error code " + std::to_string(ret));
        }

        // 7. 计算本次实际写入了多少字节，并把 out 大小缩到正确位置
        size_t have = chunk_size - strm.avail_out;
        out.resize(old_size + have);

    } while (ret != Z_STREAM_END);  // 直到遇到流结束标志

    // 8. 清理状态
    inflateEnd(&strm);
    return out;
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
