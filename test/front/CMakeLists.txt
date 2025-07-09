cmake_minimum_required(VERSION 3.10)
project(Fronts)
# 3. 启用自动MOC、UIC、RCC处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 查找 Qt6 或 Qt5，并要求必须找到 Widgets 组件
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

file(GLOB_RECURSE SOURCES "../../code/mainwindow/*.cpp")
file(GLOB_RECURSE HEADERS "../../code/mainwindow/*.h")
include_directories(${HEADERS})
add_executable(Front ../../code/main.cpp ${SOURCES} )
target_link_libraries(Front PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
# 如果使用的是 Qt6，调用 qt_finalize_executable 完成可执行文件的最终配置
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Front)
endif()