# 设置 CMake 的最低版本要求
cmake_minimum_required(VERSION 3.16)

# 检查是否为 Windows 平台
if(NOT WIN32)
    message(FATAL_ERROR "这个项目只能在 Windows 下构建")
endif()

# 定义项目名称、版本号和语言
project(FeiQ2 VERSION 0.1 LANGUAGES CXX)

# 设置第三方库路径
set(THIRD_PARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib-common)

# 开启 Qt 的三大自动工具
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 将 "ui" 目录添加到 AUTOUIC 的搜索路径中，解决找不到 .ui 文件对应头文件的问题
list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS "ui")

# 添加项目根目录到包含路径
include_directories(${PROJECT_SOURCE_DIR})

# 查找 Qt6 或 Qt5，并要求必须找到 Widgets 组件
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# 递归查找所有源文件、头文件和 UI 文件
file(GLOB_RECURSE SOURCES "code/*.cpp")
file(GLOB_RECURSE HEADERS "code/*.h")


# 将所有文件添加到项目源文件列表中
set(PROJECT_SOURCES
    ${SOURCES}
    ${HEADERS}

    source/image.qrc
)

# 为源码添加宏定义，用于指定配置文件的路径
add_definitions(-DCONFIG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/config/")

# 创建可执行文件
# 使用 WIN32 参数创建无控制台窗口的 GUI 应用
add_executable(FeiQ2 WIN32 ${PROJECT_SOURCES})

# 设置目标属性
set_target_properties(FeiQ2 PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# 为目标添加第三方库的包含目录
target_include_directories(FeiQ2 PRIVATE
    ${THIRD_PARTY_PATH}/include
)

# [注意] 为目标添加第三方库的链接目录
# 您当前的路径指向了为 Visual C++ (VC) 编译的库。
# 如果您使用的是 MinGW 编译器，这里应该链接为 MinGW 编译的 OpenSSL 库，否则会引发新的链接错误。
target_link_directories(FeiQ2 PRIVATE
    ${THIRD_PARTY_PATH}/lib/openssl/VC/x64/MTd
)
#添加zlib库路径
target_link_directories(FeiQ2 PRIVATE
    ${THIRD_PARTY_PATH}/lib/zlib
)

# 链接所有需要的库
target_link_libraries(FeiQ2 PRIVATE
    # 1. Qt 库
    # Qt::Widgets 会自动处理 Qt 核心库和入口点库的链接
    Qt::Widgets

    # 2. 显式链接 MinGW 运行时库
    # 这是本次修正的核心。某些 MinGW 环境下，需要手动链接此库来提供 __imp___argc 等符号。
    mingw32
    mingwex

    # 3. OpenSSL 库
    ssl
    crypto

    # 4. Windows 系统库
    ws2_32       # Windows Sockets API
    mswsock      # Microsoft Windows Sockets 2.0 扩展
    iphlpapi     # IP Helper API
    Crypt32      # Windows CryptoAPI
    kernel32     # 核心系统库

    #5.zlib
    zlibstatic
   
)

# 添加自定义命令，在构建后将 OpenSSL 的 DLL 复制到输出目录
add_custom_command(
    TARGET FeiQ2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${THIRD_PARTY_PATH}/bin/openssl"
        $<TARGET_FILE_DIR:FeiQ2>
    COMMENT "正在复制 OpenSSL DLL 到输出目录..."
)

# 如果使用的是 Qt6，调用 qt_finalize_executable 完成可执行文件的最终配置
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FeiQ2)
endif()

# (可选) 定义安装规则
include(GNUInstallDirs)
install(TARGETS FeiQ2
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
