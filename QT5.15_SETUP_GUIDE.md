# Qt 5.15 配置指南

本指南将帮助您配置Qt环境以支持Qt5.15项目。

## 一、安装Qt 5.15

### 方法1：使用Qt在线安装器（推荐）

1. **下载Qt安装器**
   - 访问：https://www.qt.io/download-open-source
   - 下载 Qt Online Installer

2. **运行安装器并选择组件**
   - 选择 Qt 5.15.2（或更高版本，如5.15.8）
   - 选择编译器：
     - Windows: MinGW 8.1.0 64-bit 或 MSVC 2019 64-bit
     - 确保勾选以下组件：
       - Qt 5.15.2 (或更高版本)
       - MinGW 8.1.0 64-bit (如果使用MinGW)
       - Qt Creator (IDE)

3. **安装路径**
   - 默认路径：`C:\Qt\5.15.2\mingw_64` (MinGW版本)
   - 或：`C:\Qt\5.15.2\msvc2019_64` (MSVC版本)

### 方法2：使用预编译包

- 下载地址：https://download.qt.io/archive/qt/5.15/
- 选择对应平台的安装包

## 二、配置Qt Creator

### 1. 添加Qt版本

1. 打开Qt Creator
2. 菜单：`工具(Tools)` → `选项(Options)` → `Kits`
3. 点击 `Qt Versions` 标签
4. 点击 `添加(Add)` 按钮
5. 浏览到Qt5.15的安装路径，选择 `qmake.exe`：
   - MinGW: `C:\Qt\5.15.2\mingw_64\bin\qmake.exe`
   - MSVC: `C:\Qt\5.15.2\msvc2019_64\bin\qmake.exe`
6. 点击 `应用(Apply)`

### 2. 配置编译器

如果使用MinGW：
1. 在 `编译器(Compilers)` 标签中
2. 确保MinGW编译器已自动检测
3. 如果没有，点击 `添加` → `MinGW` → 浏览到 `C:\Qt\Tools\mingw810_64\bin\g++.exe`

### 3. 配置Kit

1. 在 `Kits` 标签中
2. 点击 `添加(Add)` 创建新Kit
3. 配置：
   - **名称**: Desktop Qt 5.15.2 MinGW 64-bit
   - **设备类型**: Desktop
   - **编译器**: MinGW 8.1.0 64-bit (C++)
   - **Qt版本**: Qt 5.15.2
   - **调试器**: 自动检测或选择GDB
4. 点击 `确定(OK)`

## 三、配置CMake项目

### 方法1：在Qt Creator中配置

1. **打开项目**
   - 菜单：`文件(File)` → `打开文件或项目(Open File or Project)`
   - 选择 `CMakeLists.txt`

2. **配置CMake**
   - Qt Creator会自动检测CMake
   - 在配置对话框中：
     - **构建目录**: 选择或创建新的构建目录（如 `build-Qt5.15`）
     - **CMake**: 选择CMake可执行文件
     - **Kit**: 选择刚才配置的Qt5.15 Kit

3. **设置CMake变量（如果需要）**
   - 点击 `配置项目(Configure Project)`
   - 在CMake变量中，可以手动设置：
     ```
     CMAKE_PREFIX_PATH=C:/Qt/5.15.2/mingw_64
     ```

### 方法2：命令行配置

```bash
# 1. 清理旧的构建目录（如果存在）
rm -rf build
# Windows: rmdir /s /q build

# 2. 创建新的构建目录
mkdir build-qt515
cd build-qt515

# 3. 配置CMake（指定Qt5路径）
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/mingw_64"

# 4. 编译
cmake --build .
```

## 四、验证配置

### 1. 检查Qt版本

在CMake配置后，检查CMakeCache.txt中的Qt版本：
```bash
grep Qt5_VERSION CMakeCache.txt
# 应该显示：Qt5_VERSION:STRING=5.15.2
```

### 2. 检查链接库

编译后检查链接的Qt库：
```bash
# Windows (使用dumpbin或objdump)
dumpbin /dependents PowerDAQ.exe | findstr Qt5
```

### 3. 运行项目

如果项目能正常编译和运行，说明配置成功！

## 五、常见问题

### 问题1：CMake找不到Qt5

**解决方案**：
```bash
# 方法1：设置CMAKE_PREFIX_PATH环境变量
set CMAKE_PREFIX_PATH=C:\Qt\5.15.2\mingw_64

# 方法2：在CMakeLists.txt中添加
set(CMAKE_PREFIX_PATH "C:/Qt/5.15.2/mingw_64" ${CMAKE_PREFIX_PATH})
```

### 问题2：Qt Creator显示"没有有效的Kit"

**解决方案**：
1. 检查Qt版本是否正确添加
2. 检查编译器是否正确配置
3. 确保Kit中的Qt版本和编译器匹配

### 问题3：编译错误：找不到Qt5::Core等

**解决方案**：
- 确保CMakeLists.txt中使用的是 `Qt5::Core` 而不是 `Qt::Core`
- 检查 `find_package(Qt5 5.15 REQUIRED ...)` 是否正确

### 问题4：多个Qt版本冲突

**解决方案**：
- 在CMake配置时明确指定Qt5路径
- 使用环境变量 `CMAKE_PREFIX_PATH` 指向Qt5.15
- 在Qt Creator中为项目选择正确的Kit

## 六、项目已做的兼容性修改

项目代码已经修改为兼容Qt5.15：

1. ✅ CMakeLists.txt: `Qt6` → `Qt5`
2. ✅ 链接库: `Qt::Core` → `Qt5::Core`
3. ✅ 事件API: `wheelEvent->position()` → `wheelEvent->pos()`
4. ✅ 移除了Qt6特有的 `qt_standard_project_setup()`

## 七、快速检查清单

- [ ] Qt 5.15已安装
- [ ] Qt Creator中已添加Qt5.15版本
- [ ] 已配置Qt5.15 Kit
- [ ] CMake能找到Qt5.15（检查CMakeCache.txt）
- [ ] 项目能正常编译
- [ ] 项目能正常运行

## 八、环境变量设置（可选）

如果需要在命令行直接使用Qt5.15，可以设置环境变量：

**Windows (PowerShell)**:
```powershell
$env:CMAKE_PREFIX_PATH = "C:\Qt\5.15.2\mingw_64"
$env:PATH = "C:\Qt\5.15.2\mingw_64\bin;$env:PATH"
```

**Windows (CMD)**:
```cmd
set CMAKE_PREFIX_PATH=C:\Qt\5.15.2\mingw_64
set PATH=C:\Qt\5.15.2\mingw_64\bin;%PATH%
```

**Linux/Mac**:
```bash
export CMAKE_PREFIX_PATH=/path/to/Qt/5.15.2/gcc_64
export PATH=/path/to/Qt/5.15.2/gcc_64/bin:$PATH
```

---

**注意**：如果您的系统同时安装了Qt6和Qt5，确保在配置项目时明确指定使用Qt5.15，避免版本冲突。


