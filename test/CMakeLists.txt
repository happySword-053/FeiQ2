#顶级cmake文件，管理子测试模块
#设置cmake最低版本
cmake_minimum_required(VERSION 3.10)
#设置项目名称
project(FeiQ2_TestDemo)
#设置项目编译语言为C++ 17
set(CMAKE_CXX_STANDARD 17)
#jsoncpp测试
option(BUILD_JSONCPP OFF)
#boostAsio测试
option(BUILD_BOOST_ASIO OFF)
#获取本地网络适配器信息测试
option(BUILD_GET_NETWORK_ADAPTER_INFO OFF)
#boostUdp广播测试
option(BUILD_BOOST_UDP_BROADCAST OFF)
#openSSL测试
option(BUILD_OPENSSL OFF)

if(BUILD_BOOST_UDP_BROADCAST)
    add_subdirectory(BoostUdp)
endif()

if(BUILD_JSONCPP)
    add_subdirectory(jsonCpp)
endif()

if(BUILD_BOOST_ASIO)
    add_subdirectory(BoostAsio)
endif()

if(BUILD_GET_NETWORK_ADAPTER_INFO)
    add_subdirectory(GetLocalNet)
endif()

if(BUILD_OPENSSL)
    add_subdirectory(OpenSSLtest)
endif() 