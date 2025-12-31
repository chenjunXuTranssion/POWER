@echo off
REM =========================================================
REM Qt5.15 快速配置脚本 (Windows)
REM =========================================================
REM 使用方法：
REM   1. 修改下面的Qt5路径为您的实际安装路径
REM   2. 双击运行此脚本，或在命令行中执行
REM =========================================================

REM =========================================================
REM 配置Qt5路径（请根据您的实际安装路径修改）
REM =========================================================
set QT5_PATH=C:\Qt\5.15.2\mingw_64

REM 检查Qt5是否存在
if not exist "%QT5_PATH%\bin\qmake.exe" (
    echo 错误：找不到Qt5.15！
    echo 请修改脚本中的QT5_PATH变量为您的Qt5.15安装路径
    echo 当前路径：%QT5_PATH%
    pause
    exit /b 1
)

echo =========================================================
echo Qt5.15 配置脚本
echo =========================================================
echo Qt5路径: %QT5_PATH%
echo.

REM 设置环境变量
set CMAKE_PREFIX_PATH=%QT5_PATH%
set PATH=%QT5_PATH%\bin;%PATH%

echo 环境变量已设置：
echo   CMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%
echo.

REM 清理旧的构建目录
if exist build (
    echo 清理旧的构建目录...
    rmdir /s /q build
)

REM 创建新的构建目录
echo 创建构建目录...
mkdir build-qt515 2>nul
cd build-qt515

REM 配置CMake
echo.
echo 配置CMake...
cmake .. -DCMAKE_PREFIX_PATH="%QT5_PATH%"

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo CMake配置失败！
    echo 请检查：
    echo   1. CMake是否已安装
    echo   2. Qt5.15路径是否正确
    echo   3. 是否有足够的权限
    pause
    exit /b 1
)

echo.
echo =========================================================
echo 配置完成！
echo =========================================================
echo 下一步：
echo   1. 在build-qt515目录中运行: cmake --build .
echo   2. 或在Qt Creator中打开项目并选择Qt5.15 Kit
echo.
pause


