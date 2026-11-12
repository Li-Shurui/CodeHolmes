cmake_minimum_required(VERSION 3.21)

# CodeHolmes —— 全仓库 C/C++ 代码质量扫描工具
#   引擎(engine) 只用 Qt Core，可独立编译成 CLI，便于无界面调试
#   界面(ui)     使用 Qt Widgets
#
# 目录约定：代码在本目录，文档在 docs/，图片在 images/，绘图脚本在 tools/

project(CodeHolmes VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 默认构建到 build/ 下，避免污染源码树
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

# Qt 6.11.2 (mingw_64)
find_package(Qt6 6.11 REQUIRED COMPONENTS Core)

# ---------------------------------------------------------------------------
# 引擎：仅依赖 Qt Core
# ---------------------------------------------------------------------------
add_library(engine STATIC
    engine/Issue.h
    engine/Config.h
    engine/CompilationDatabase.h
    engine/ClangTidyRunner.h
    engine/ClangFormatRunner.h
    engine/ScanManager.h
    engine/Toolchain.h
    engine/Toolchain.cpp
)
target_link_libraries(engine PUBLIC Qt6::Core)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# ---------------------------------------------------------------------------
# 命令行入口（调试主力：引擎问题先在终端复现）
# ---------------------------------------------------------------------------
add_executable(codeholmes-cli engine/cli/main.cpp)
target_include_directories(codeholmes-cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/engine)
target_link_libraries(codeholmes-cli PRIVATE engine)

# ---------------------------------------------------------------------------
# 图形界面
# ---------------------------------------------------------------------------
find_package(Qt6 6.11 REQUIRED COMPONENTS Widgets)
add_executable(codeholmes-ui
    ui/main.cpp
    ui/MainWindow.h
    ui/MainWindow.cpp
)
target_include_directories(codeholmes-ui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/engine)
target_link_libraries(codeholmes-ui PRIVATE Qt6::Widgets engine)

# Windows 下 GUI 程序不弹控制台；开发期保留控制台便于看 qDebug
if(WIN32)
  set_target_properties(codeholmes-ui PROPERTIES WIN32_EXECUTABLE FALSE)
endif()
