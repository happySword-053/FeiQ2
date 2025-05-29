# 设置 CMake 的最低版本要求，确保使用的 CMake 版本支持后续的功能
cmake_minimum_required(VERSION 3.16)
# 添加平台检测
if(NOT WIN32)
    message(FATAL_ERROR "这个项目只能在windows下构建")
endif()
# 定义项目名称、版本号和使用的编程语言
project(FeiQ2 VERSION 0.1 LANGUAGES CXX)



# 开启 Qt 的自动 UI 处理功能，CMake 会自动将 .ui 文件转换为对应的 C++ 代码
set(CMAKE_AUTOUIC ON)
# 开启 Qt 的自动元对象编译器（MOC）功能，处理 Qt 的元对象系统
set(CMAKE_AUTOMOC ON)
# 开启 Qt 的自动资源编译器（RCC）功能，处理 Qt 的资源文件
set(CMAKE_AUTORCC ON)

# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
# 要求编译器必须支持指定的 C++ 标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 这句很重要！！！！！！！解决找不到 ui 文件源代码的问题
# 将 "ui" 目录添加到自动 UI 文件搜索路径中
list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS "ui")

include_directories(
    ${PROJECT_SOURCE_DIR}
)
# 以下两行代码被注释掉，原代码可能存在拼写错误，正确的应该是 CMAKE_CURRENT_BINARY_DIR
# 可以将 CMake 生成文件的目录和当前 CMakeLists.txt 所在目录的二进制目录添加到包含路径中
# include_directories(${CMAKE_BINARY_PATH})
# include_directories(${CMAKE_CURRENT_BINRAY_PATH})

# 查找 Qt 库，优先查找 Qt6，如果找不到则查找 Qt5，并且要求必须找到 Widgets 组件
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
# 根据前面找到的 Qt 版本，再次查找对应的 Widgets 组件
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)


# 递归查找 "code" 目录下的所有 .cpp 文件，并将其路径存储在 SOURCES 变量中
file(GLOB_RECURSE SOURCES "code/*.cpp")
# 递归查找 "code" 目录下的所有 .h 文件，并将其路径存储在 HEADERS 变量中
file(GLOB_RECURSE HEADERS "code/*.h")
# 递归查找 "ui" 目录下的所有 .ui 文件，并将其路径存储在 UI_FILE 变量中
file(GLOB UI_FILE "ui/*.ui")

# 定义项目的源文件列表，包含前面找到的 .cpp、.h 和 .ui 文件
set(PROJECT_SOURCES
        ${SOURCES}
        ${HEADERS}
        ${UI_FILE}
)
# 添加宏定义，指定config文件的路径，用于配置编译时的宏定义
# 用于dispositionReader类获取config文件夹下的配置文件
add_definitions(-DCONFIG_PATH=${CMAKE_CURRENT_SOURCE_DIR}/config/)

# 判断当前使用的 Qt 版本是否大于等于 6
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    # 使用 Qt6 的 qt_add_executable 函数创建可执行文件，指定手动完成最终配置
    qt_add_executable(FeiQ2
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        source/image.qrc
    )
else()
    # 仅保留Windows平台的可执行文件配置
    add_executable(FeiQ2
        ${PROJECT_SOURCES}
        source/image.qrc  # 为Qt5添加资源文件
    )
endif()

# 将目标可执行文件或库与对应的 Qt Widgets 组件库进行链接
target_link_libraries(FeiQ2 PRIVATE 
                            Qt${QT_VERSION_MAJOR}::Widgets
                            Crypt32  # 新增加密API库  
                    )

# 包含头文件路径
include_directories(${Boost_INCLUDE_DIRS})
# 链接Boost库
target_link_libraries(FeiQ2 ${Boost_LIBRARIES})
# # Qt 6.1 及以后版本会自动为 iOS 设置 MACOSX_BUNDLE_GUI_IDENTIFIER
# # 如果开发 iOS 或 macOS 应用，建议手动设置一个明确的、固定的捆绑标识符
# if(${QT_VERSION} VERSION_LESS 6.1.0)
#     # 设置 macOS 应用程序包的 GUI 标识符
#     set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FeiQ2)
# endif()

# 设置目标的属性，包括 macOS 应用程序包的版本信息、是否为 macOS 应用程序包以及是否为 Windows 可执行文件
set_target_properties(FeiQ2 PROPERTIES
    ${BUNDLE_ID_OPTION}
    WIN32_EXECUTABLE TRUE
)

# 包含 GNU 标准安装目录的定义
include(GNUInstallDirs)

# 定义如何安装目标，包括应用程序包、共享库和可执行文件的安装路径
install(TARGETS FeiQ2
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 如果使用的是 Qt6，调用 qt_finalize_executable 函数完成可执行文件的最终配置
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FeiQ2)
endif()
