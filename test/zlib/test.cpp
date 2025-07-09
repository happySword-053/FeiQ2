#include <zlib.h>
#include <vector>
#include <stdexcept>
#include <cstring>    // for std::memcpy
#include <iostream>
#include <iomanip>

// 一次性压缩整个 vector<char>
std::vector<char> compress_vector(const std::vector<char>& input) {
    uLong src_len = static_cast<uLong>(input.size());
    // 计算最大压缩后长度
    uLong bound = compressBound(src_len);

    std::vector<char> out(bound);
    int ret = compress2(
        reinterpret_cast<Bytef*>(out.data()), &bound,
        reinterpret_cast<const Bytef*>(input.data()), src_len,
        Z_BEST_COMPRESSION
    );
    if (ret != Z_OK) {
        throw std::runtime_error("compress2 failed: error code " + std::to_string(ret));
    }
    out.resize(bound);
    return out;
}

// 流式解压，不事先知道原始大小
std::vector<char> decompress_vector(const std::vector<char>& compressed_data) {
    z_stream strm;
    std::memset(&strm, 0, sizeof(strm));
    if (inflateInit(&strm) != Z_OK) {
        throw std::runtime_error("inflateInit failed");
    }

    std::vector<char> out;
    const size_t chunk_size = 16384;
    out.reserve(chunk_size);

    strm.next_in  = reinterpret_cast<Bytef*>(const_cast<char*>(compressed_data.data()));
    strm.avail_in = static_cast<uInt>(compressed_data.size());

    int ret;
    do {
        size_t old_size = out.size();
        out.resize(old_size + chunk_size);

        strm.next_out  = reinterpret_cast<Bytef*>(out.data() + old_size);
        strm.avail_out = static_cast<uInt>(chunk_size);

        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
            inflateEnd(&strm);
            throw std::runtime_error("inflate failed with error code " + std::to_string(ret));
        }

        size_t have = chunk_size - strm.avail_out;
        out.resize(old_size + have);
    } while (ret != Z_STREAM_END);

    inflateEnd(&strm);
    return out;
}

int main() {
    // 构造一个示例输入
    std::string text = "Hello, zlib compression and decompression in C++!";
    std::vector<char> original(text.begin(), text.end());

    std::cout << "原始大小: " << original.size() << " bytes\n";

    // 压缩
    auto compressed = compress_vector(original);
    std::cout << "压缩后大小: " << compressed.size() << " bytes\n";
    std::cout << "压缩后内容: ";
    for (char c : compressed) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    std::cout << std::dec << "\n";

    // 解压
    auto decompressed = decompress_vector(compressed);
    std::cout << "解压后大小: " << decompressed.size() << " bytes\n";
    std::cout << "解压后内容: " << std::string(decompressed.begin(), decompressed.end()) << "\n";
    // 验证
    std::string roundtrip(decompressed.begin(), decompressed.end());
    if (roundtrip == text) {
        std::cout << "解压成功，数据一致！\n";
    } else {
        std::cout << "解压失败，数据不一致！\n";
    }

    return 0;
}
