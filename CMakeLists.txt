cmake_minimum_required(VERSION 3.10)

project(UAV-Parameter-Plugin
    VERSION  1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

include_directories(${CMAKE_SOURCE_DIR})

# ── 接口层（仅头文件，IDE 可见） ──
add_custom_target(interfaces
    SOURCES
        interfaces/IPlugin.h
        interfaces/ILinkManager.h
        interfaces/IParameterProvider.h
)

# ── 宿主程序 ──
add_executable(UAVGroundStation
    core/main.cpp
    core/MainWindow.h
    core/MainWindow.cpp
    core/PluginManager.h
    core/PluginManager.cpp
)

target_link_libraries(UAVGroundStation PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)
