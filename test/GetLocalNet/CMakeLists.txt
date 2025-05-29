cmake_minimum_required(VERSION 3.10)
message(STATUS "当前构建项目：获取当前系统适配器信息")
project(get_adapter_info)
set(CMAKE_CXX_STANDARD 17)
add_executable(get_adapter_info test.cpp)
# 添加Windows网络库链接
target_link_libraries(get_adapter_info PRIVATE 
    ws2_32     # Windows Sockets API
    iphlpapi   # IP Helper API
)