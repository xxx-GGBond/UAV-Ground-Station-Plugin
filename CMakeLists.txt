cmake_minimum_required(VERSION 3.10)

# ── 项目基本信息 ───────────────────────────────────────────
project(UAV-Ground-Station
    VERSION  1.0.0
    LANGUAGES CXX
)

# ── C++ 标准 ───────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Qt 配置 ────────────────────────────────────────────────
set(CMAKE_AUTOMOC ON)       # 自动运行 moc 元对象编译器
set(CMAKE_AUTORCC ON)       # 自动编译 .qrc 资源文件
set(CMAKE_AUTOUIC ON)       # 自动编译 .ui 界面文件

# 查找 Qt5 组件
# 为什么用 find_package 而非硬编码路径？
#   Qt 可以安装在任何位置，find_package 通过 CMAKE_PREFIX_PATH
#   自动定位 Qt 的头文件和库，保证跨平台和跨环境的一致性
find_package(Qt5 REQUIRED COMPONENTS
    Core        # Qt5Core —— 事件循环、信号槽、QObject 等基础
    Gui         # Qt5Gui  —— 窗口系统、OpenGL、字体
    Widgets     # Qt5Widgets —— QMainWindow、QDockWidget 等 UI 组件
)

# ── 全局 include 目录 ──────────────────────────────────────
# 所有目标都可以 #include "interfaces/IPlugin.h" 这样的路径
# 而不需要写相对路径 "../../interfaces/IPlugin.h"
include_directories(${CMAKE_SOURCE_DIR})

# ── 接口层（仅头文件，不需要编译） ──────────────────────────
# 将接口头文件加入自定义 target，使其在 IDE 中可见但不参与编译
add_custom_target(interfaces
    SOURCES
        interfaces/IPlugin.h
        interfaces/ILinkManager.h
        interfaces/ITelemetryProvider.h
        interfaces/IMissionManager.h
        interfaces/IUiComponent.h
)

# ── SDK 公共库（静态库） ────────────────────────────────────
# 编译为静态库（.a / .lib），被 core 和 plugins 链接
# 为什么是静态库？
#   - SDK 代码量小，静态链接避免额外的 .dll 分发
#   - 编译期可见，方便 LTO（链接时优化）
add_library(sdk STATIC
    sdk/common/GlobalDefines.h
    sdk/mavlink/MavLinkProtocol.h
    sdk/mavlink/MavLinkProtocol.cpp
)

target_link_libraries(sdk PUBLIC
    Qt5::Core       # 使用了 QByteArray、QMap 等 Qt 类型
)

# ── 核心宿主程序（可执行文件） ──────────────────────────────
add_executable(UAVGroundStation
    core/main.cpp
    core/MainWindow.h
    core/MainWindow.cpp
    core/CommunicationHub.h
    core/CommunicationHub.cpp
    core/PluginManager.h
    core/PluginManager.cpp
)

target_link_libraries(UAVGroundStation PRIVATE
    sdk                         # 链接 SDK 静态库
    Qt5::Core                   # QObject、信号槽、事件循环
    Qt5::Gui                    # 窗口系统
    Qt5::Widgets                # QMainWindow 等 UI 组件
)

# ── 插件构建说明 ────────────────────────────────────────────
#
# 每个插件独立编译为动态库（.dll/.so），示例 CMakeLists 片段：
#
#   add_library(TelemetryPlugin SHARED
#       plugins/TelemetryPlugin/TelemetryPlugin.h
#       plugins/TelemetryPlugin/TelemetryPlugin.cpp
#   )
#   target_link_libraries(TelemetryPlugin PRIVATE sdk Qt5::Core Qt5::Widgets)
#
# 插件编译后需复制到 build/plugins/ 目录，PluginManager 会自动扫描。

# ── 安装规则（可选） ───────────────────────────────────────
install(TARGETS UAVGroundStation RUNTIME DESTINATION bin)
install(TARGETS sdk ARCHIVE DESTINATION lib)
