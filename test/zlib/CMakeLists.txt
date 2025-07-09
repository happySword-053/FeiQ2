cmake_minimum_required(VERSION 3.10)
project(ZlibTest)

set(CMAKE_CXX_STANDARD 17)

find_library(
    ZLIB
    NAMES zlibstatic
    HINTS ${PROJECT_SOURCE_DIR}/../../lib-common/lib/zlib
    NO_DEFAULT_PATH
)
if(NOT ZLIB)
    
    #报错退出
    message(FATAL_ERROR "zlib not found")
else()
    message(STATUS "zlib found: ${ZLIB}")
endif()
include_directories(${PROJECT_SOURCE_DIR}/../../lib-common/include/zlib)


add_executable(ZlibTest test.cpp)

target_link_libraries(ZlibTest PRIVATE
    ${ZLIB}
)
