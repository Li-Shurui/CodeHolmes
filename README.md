# CodeHolmes

全仓库 C/C++ 代码质量扫描工具：clang-tidy（静态分析 / 漏洞）+ clang-format（代码风格），Qt 6 界面。

## 目录结构

代码、文档、图片三者隔离：

```
CodeHolmes/
├── CMakeLists.txt        ← 构建入口
├── README.md             ← 本文件
├── engine/               ← 引擎（仅依赖 Qt Core，可独立编译成 CLI 调试）
├── ui/                   ← 界面（Qt Widgets）
├── docs/                 ← 文档（设计、规划）
├── images/               ← 原型图
└── tools/                ← 绘图脚本（生成 images/ 下的原型图，非 C++ 工程一部分）
```

## 当前进度

**阶段 0（环境与骨架）已完成** —— 详见 [docs/实现规划.md](docs/实现规划.md)

已就绪：
- CMake 工程：`engine`（静态库，仅依赖 Qt Core）+ `codeholmes-cli` + `codeholmes-ui`
- `Issue.h` 统一问题数据模型（clang-tidy 与 clang-format 的产出都归一到它）
- `Toolchain` 外部工具探测（`doctor` 子命令）
- 引擎保持可独立编译成 CLI —— **UI 出问题先用 CLI 复现，立刻能分清是引擎错还是界面错**

尚未实现：`scan` 子命令（阶段 1 开始填充）、配置系统（阶段 6）、界面功能（阶段 4 起）。

## 本机构建环境

| 组件 | 路径 / 版本 |
|---|---|
| Qt | `E:\software\Qt\6.11.2\mingw_64`（仅 mingw_64 套件，无 MSVC） |
| MinGW g++ | `E:\software\Qt\Tools\mingw1310_64\bin`（13.1.0） |
| CMake | 4.4.3 |
| Ninja | `D:\program\mingw64\bin` |
| clang-tidy / clang-format | `D:\program\clang\bin`（22.1.8） |

## 构建

在**本目录**（`D:\my\CodeHolmes`）下执行：

```powershell
$env:PATH = "E:\software\Qt\6.11.2\mingw_64\bin;E:\software\Qt\Tools\mingw1310_64\bin;D:\program\clang\bin;" + $env:PATH

cmake -S . -B build -G Ninja `
  -DCMAKE_PREFIX_PATH="E:/software/Qt/6.11.2/mingw_64" `
  -DCMAKE_BUILD_TYPE=Debug

cmake --build build
```

> 注意：本项目只有 MinGW 套件，**必须用 MinGW 工具链**。
> 引擎通过 `QProcess` 调用外部 `clang-tidy.exe` / `clang-format.exe`，不链接 LLVM 库，
> 因此 MinGW 产物与 LLVM 原生 Windows 版二进制之间没有 ABI 问题。

## 使用

```powershell
# 工具链自检
build\bin\codeholmes-cli.exe doctor

# 帮助 / 版本
build\bin\codeholmes-cli.exe --help
build\bin\codeholmes-cli.exe version

# 启动界面（阶段 0 仅为占位窗口）
build\bin\codeholmes-ui.exe
```

## 关于输出字符集

**所有面向用户的字符串（CLI 输出、UI 文本）一律使用英文。**

原因：中文 Windows + MinGW 工具链下，UTF-8 源码字面量、GCC 的默认执行字符集、
以及 Qt Creator 输出面板/Windows 控制台的解码方式三方不一致，中文会显示为乱码。
用英文可从根上规避，代价是界面暂时只有英文。

源码**注释保持中文**（注释不参与编译，不影响任何输出）。

如果将来确需中文界面，正确做法是用 Qt 翻译机制而非硬编码字面量：

- 代码里写 `tr("Error")`，用 `QTranslator` 加载 `.qm`
- 或在 CMake 中为 MinGW 显式指定 `-finput-charset=UTF-8 -fexec-charset=UTF-8`，
  并确保运行环境是 UTF-8 代码页（`chcp 65001`）——但这三处必须同时成立才可靠

自检脚本（确认没有遗漏的中文字面量）：

```powershell
python tools\check_ascii_strings.py
```

## 引擎模块

```
engine/
  Issue.h                统一问题模型 ★ 最关键的头文件，只追加字段不改语义
  Config.h               配置模型（阶段 6 实现）
  CompilationDatabase.h  compile_commands.json 解析（阶段 2）
  ClangTidyRunner.h      clang-tidy 调用与诊断解析（阶段 1）
  ClangFormatRunner.h    clang-format 调用与偏移转换（阶段 3）
  ScanManager.h          线程池调度 / 取消 / 汇总（阶段 2）
  Toolchain.{h,cpp}      外部工具探测 ✓ 已实现
  cli/main.cpp           命令行入口 ✓ 已实现
```

## 相关文档与图片

| 文件 | 内容 |
|---|---|
| [docs/实现规划.md](docs/实现规划.md) | 11 个阶段的详细计划、验收标准、常见坑 |
| [docs/界面设计.md](docs/界面设计.md) | 界面设计与 Qt 控件映射 |
| images/roadmap.png | 路线图总览 |
| images/ui_mockup.png | 主界面原型（含检查项配置浮层） |
| images/ui_config_dialog.png | 检查项配置对话框原型 |

重新生成原型图：

```powershell
python tools\render_roadmap.py
python tools\render_ui.py
python tools\render_config_dialog.py
```
