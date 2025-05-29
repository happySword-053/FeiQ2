project("boost_udp广播测试")
# 修改这里：指向 boost 文件夹的上一级目录
message(STATUS "${CMAKE_SOURCE_DIR}/../../lib-common/src")
include_directories(${CMAKE_SOURCE_DIR}/../../lib-common/src)
add_executable(main test.cpp)
target_link_libraries(main PRIVATE Ws2_32)
# 包含头文件路径


# 链接Boost库
# 注意：您需要确保 Boost.System 和 Boost.Thread (如果需要) 已经被编译，
# 并且您需要链接到实际的库文件。
# 下面是一个示例，您可能需要调整库的名称和路径。
# 更好的方法是使用 find_package(Boost REQUIRED COMPONENTS system thread)
# 如果您的Boost库是以CMake可发现的方式安装的。
