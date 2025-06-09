cmake_minimum_required(VERSION 3.10)
project(OpenSSLtest)

set(CMAKE_CXX_STANDARD 17)
# 修正路径：指向实际的 src/OpenSSL 目录
set(opensslPath "${CMAKE_SOURCE_DIR}/../../lib-common")

# 明确指向具体的库子目录（根据实际使用的构建类型选择，如 MTd、MDd 等）
link_directories(${opensslPath}/lib/openssl/VC/x64/MTd)  # 示例路径，根据实际情况调整
include_directories(${opensslPath}/include)
link_directories(${opensslPath})  # 示例路径，根据实际情况调整

add_executable(OpenSSLtest test.cpp)
target_link_libraries(OpenSSLtest ssl crypto)
target_include_directories(OpenSSLtest PUBLIC ${opensslPath})

# 添加动态库复制命令（关键修改）
add_custom_command(TARGET OpenSSLtest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${opensslPath}/bin/openssl/libcrypto-3-x64.dll"
        "${opensslPath}/bin/openssl/libssl-3-x64.dll"
        $<TARGET_FILE_DIR:OpenSSLtest>  # 复制到可执行文件输出目录
    COMMENT "Copying OpenSSL DLLs to build directory"
)