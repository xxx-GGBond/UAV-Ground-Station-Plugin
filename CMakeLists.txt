cmake_minimum_required(VERSION 3.10)

project(UAVGroundStation
    VERSION  1.0.0
    LANGUAGES CXX
)

# ── C++ 标准 ──────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Qt 自动处理 moc/uic/rcc ─────────────────────────
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ── 查找 Qt5 ─────────────────────────────────────────
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets SerialPort Network)

# ── 全局包含路径（所有 target 都能 #include "interfaces/xxx.h"） ──
include_directories(${CMAKE_SOURCE_DIR})

# ═══════════════════════════════════════════════════════
#  接口层（仅头文件，在 IDE 中可见）
# ═══════════════════════════════════════════════════════
add_custom_target(interfaces
    SOURCES
        interfaces/IPlugin.h
        interfaces/ILink.h
        interfaces/ILinkManager.h
        interfaces/IMavlinkService.h
        interfaces/IParameterProvider.h
)

# ═══════════════════════════════════════════════════════
#  主程序 —— UAVGroundStation.exe
# ═══════════════════════════════════════════════════════
add_executable(UAVGroundStation
    # ── 接口（含 Q_OBJECT 的头文件必须加入目标，AUTOMOC 才能生成 meta-object 代码） ──
    interfaces/ILink.h
    interfaces/ILinkManager.h
    interfaces/IMavlinkService.h
    # ── 核心实现 ──
    core/main.cpp
    core/MainWindow.h
    core/MainWindow.cpp
    core/PluginManager.h
    core/PluginManager.cpp
    core/LinkManager.h
    core/LinkManager.cpp
    core/SerialLink.h
    core/SerialLink.cpp
    core/UdpLink.h
    core/UdpLink.cpp
    core/MavlinkService.h
    core/MavlinkService.cpp
)

target_link_libraries(UAVGroundStation PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::SerialPort
    Qt5::Network
)

# ═══════════════════════════════════════════════════════
#  参数插件 —— UAVParameterPlugin.dll
# ═══════════════════════════════════════════════════════
add_library(UAVParameterPlugin MODULE
    interfaces/IPlugin.h
    interfaces/IParameterProvider.h
    plugins/UAVParameterPlugin/UAVParameterPlugin.h
    plugins/UAVParameterPlugin/UAVParameterPlugin.cpp
)

target_link_libraries(UAVParameterPlugin PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

# 插件 DLL 输出到 build/plugins/ 目录
set_target_properties(UAVParameterPlugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
)

# 构建主程序前必须先构建插件
add_dependencies(UAVGroundStation UAVParameterPlugin)

# 插件不需要自动添加 lib 前缀
set_target_properties(UAVParameterPlugin PROPERTIES
    PREFIX ""
)
