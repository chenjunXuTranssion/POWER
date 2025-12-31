#!/bin/bash
# =========================================================
# Qt5.15 快速配置脚本 (Linux/Mac)
# =========================================================
# 使用方法：
#   1. 修改下面的Qt5路径为您的实际安装路径
#   2. 运行: chmod +x configure_qt515.sh
#   3. 运行: ./configure_qt515.sh
# =========================================================

# =========================================================
# 配置Qt5路径（请根据您的实际安装路径修改）
# =========================================================
QT5_PATH="/path/to/Qt/5.15.2/gcc_64"  # Linux
# QT5_PATH="/path/to/Qt/5.15.2/clang_64"  # Mac

# 检查Qt5是否存在
if [ ! -f "$QT5_PATH/bin/qmake" ]; then
    echo "错误：找不到Qt5.15！"
    echo "请修改脚本中的QT5_PATH变量为您的Qt5.15安装路径"
    echo "当前路径：$QT5_PATH"
    exit 1
fi

echo "========================================================="
echo "Qt5.15 配置脚本"
echo "========================================================="
echo "Qt5路径: $QT5_PATH"
echo ""

# 设置环境变量
export CMAKE_PREFIX_PATH="$QT5_PATH"
export PATH="$QT5_PATH/bin:$PATH"

echo "环境变量已设置："
echo "  CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH"
echo ""

# 清理旧的构建目录
if [ -d "build" ]; then
    echo "清理旧的构建目录..."
    rm -rf build
fi

# 创建新的构建目录
echo "创建构建目录..."
mkdir -p build-qt515
cd build-qt515

# 配置CMake
echo ""
echo "配置CMake..."
cmake .. -DCMAKE_PREFIX_PATH="$QT5_PATH"

if [ $? -ne 0 ]; then
    echo ""
    echo "CMake配置失败！"
    echo "请检查："
    echo "  1. CMake是否已安装"
    echo "  2. Qt5.15路径是否正确"
    echo "  3. 是否有足够的权限"
    exit 1
fi

echo ""
echo "========================================================="
echo "配置完成！"
echo "========================================================="
echo "下一步："
echo "  1. 在build-qt515目录中运行: cmake --build ."
echo "  2. 或在Qt Creator中打开项目并选择Qt5.15 Kit"
echo ""


