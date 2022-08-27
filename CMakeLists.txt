## 最小CMake版本要求
cmake_minimum_required(VERSION 3.0)

## 项目名
project(JoyDirectXWork)

## 包含cmake文件
include("${PROJECT_SOURCE_DIR}/CMake/CMakeMacro.cmake")

include_directories(${PROJECT_SOURCE_DIR}/Source/)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # 预编译头文件和源文件的路径
    set(STDAFX_PCH_H ${CAMKE_SOURCE_DIR}/Source/stdafx.h)
    set(STDAFX_PCH_C ${CMAKE_SOURCE_DIR}/Source/stdafx.cpp)
endif()

## 将所有源码设置到ALL_FILES变量上
set(ALL_FILES 
Source/Main.cpp
Source/stdafx.cpp
Source/stdafx.h
Source/Application/Win32Application.cpp
Source/Application/Win32Application.h)

## 调用外部cmake定义的宏，将源文件按文件加分组
source_group_by_dir("${ALL_FILES}")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    ## 添加编译可执行文件
    add_executable(${PROJECT_NAME} ${ALL_FILES})

    ## 设置项目的属性
    ## "/SUBSYSTEM:WINDOWS /ENTRY:winMainCRTStartup"：窗口程序，不显示控制台窗口
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup")
    # 调用定义在CMakeMacro.cmake中的宏，使用预编译头
    USE_MSVC_PCH(${PROJECT_NAME} stdafx.h ${STDAFX_PCH_C})
endif()