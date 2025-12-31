cmake_minimum_required(VERSION 3.19)
project(PowerDAQ LANGUAGES CXX)

# =========================================================
# 1. 核心修正：开启 Qt 自动化处理
# =========================================================
set(CMAKE_AUTOUIC ON)  # 自动编译 .ui 文件 -> ui_*.h (解决 file not found 报错的关键!)
set(CMAKE_AUTOMOC ON)  # 自动处理信号与槽 (Q_OBJECT)
set(CMAKE_AUTORCC ON)  # 自动处理 .qrc 资源文件

# 设置C++标准（Qt5.15需要至少C++11）
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("-Wa,-mbig-obj")

# =========================================================
# 2. 查找库：包含 PrintSupport (QCustomPlot 需要)
# =========================================================
find_package(Qt5 5.15 REQUIRED COMPONENTS Core Widgets PrintSupport)

# 显示找到的Qt5版本（用于验证）
message(STATUS "Found Qt5 version: ${Qt5_VERSION}")
message(STATUS "Qt5 location: ${Qt5_DIR}")

# Qt5 需要手动设置自动处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(PowerDAQ
    main.cpp

    # --- 主窗口 ---
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui

    # --- 波形模块 ---
    src/modules/WaveformView/waveformwidget.h
    src/modules/WaveformView/waveformwidget.cpp
    src/modules/WaveformView/waveformwidget.ui

    # --- 登录模块 ---
    src/modules/Login/logindialog.h
    src/modules/Login/logindialog.cpp
    src/modules/Login/logindialog.ui

    # --- 第三方库 ---
    3rdparty/QCustomPlot/qcustomplot.cpp
    3rdparty/QCustomPlot/qcustomplot.h
)

target_include_directories(PowerDAQ PRIVATE
    ${CMAKE_SOURCE_DIR}/src                       # 让 main.cpp 能找到 src/mainwindow.h
    ${CMAKE_SOURCE_DIR}/src/modules/WaveformView  # 让其他文件能找到 WaveformWidget.h
    ${CMAKE_SOURCE_DIR}/src/modules/Login         # 让其他文件能找到 LoginDialog.h
    ${CMAKE_SOURCE_DIR}/src/modules/ConfigManager # 让编译器能找到 ConfigManagerDialog
    ${CMAKE_SOURCE_DIR}/3rdparty/QCustomPlot      # 让编译器能找到 qcustomplot.h
)

target_link_libraries(PowerDAQ
    PRIVATE
        Qt5::Core
        Qt5::Widgets
        Qt5::PrintSupport  # 必须链接，否则 QCustomPlot 报错
)

# =========================================================
# 3. 抑制第三方库的弃用警告
# =========================================================
# QCustomPlot 使用了 Qt 6.10.1 中已弃用的 API，这是第三方库的问题
set_source_files_properties(
    3rdparty/QCustomPlot/qcustomplot.cpp
    PROPERTIES
    COMPILE_FLAGS "-Wno-deprecated-declarations"
)

set_source_files_properties(
    src/modules/WaveformView/waveformwidget.cpp
    PROPERTIES
    COMPILE_FLAGS "-Wno-deprecated-declarations"
)

include(GNUInstallDirs)

install(TARGETS PowerDAQ
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Qt5 部署脚本（可选，也可以手动使用 windeployqt）
# 如果需要自动部署，可以取消下面的注释
# if(WIN32)
#     find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt5_DIR}/../../../bin)
#     if(WINDEPLOYQT_EXECUTABLE)
#         add_custom_command(TARGET PowerDAQ POST_BUILD
#             COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:PowerDAQ>
#         )
#     endif()
# endif()
