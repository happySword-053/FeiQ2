cmake_minimum_required(VERSION 3.10)
project(sqlliteTest)
set(CMAKE_CXX_STANDARD 11)

set(SQLLITE_PATH ${CMAKE_SOURCE_DIR}/../../lib-common/include/sqllite)
include_directories(${SQLLITE_PATH})

# file(GLOB_RECURSE SQLITE_TEST_SOURCES CONFIGURE_DEPENDS "${SQLLITE_PATH}/sqlite3.c")
add_executable(sqlliteTest test.cpp ${SQLLITE_PATH}/sqlite3.c)