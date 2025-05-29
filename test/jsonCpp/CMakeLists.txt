set(CMAKE_MINIMUM_REQUIRED_VERSION 3.10)
# 设置编译器为MinGW
set(CMAKE_C_COMPILER "D:/qt/Tools/mingw1310_64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "D:/qt/Tools/mingw1310_64/bin/g++.exe")
project(jsonCppTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#设置jsoncpp的lujing
set(JSON_CPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../code/jsonCpp)
add_executable(jsonCppTest test.cpp ${JSON_CPP_DIR}/jsoncpp.cpp)

target_include_directories(jsonCppTest PRIVATE ${JSON_CPP_DIR}/json)

# 在add_executable之后添加
target_compile_options(jsonCppTest PRIVATE -g -O0)  #禁用优化并生成调试符号
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--enable-auto-import")  # 修复MinGW链接问题