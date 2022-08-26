## 最小CMake版本要求
cmake_minimum_required(VERSION 3.0)

## 项目名
project(JoyDirectXWork)

## 包含cmake文件
include("${PROJECT_SOURCE_DIR}/CMake/CMakeMacro.cmake")

## 将源码设置到ALL_FILES变量上
set(ALL_FILES 
Source/Main.cpp
Source/Utils/Win32Application.cpp
Source/Utils/Win32Application.h)

## 调用外部cmake定义的宏，将源文件按文件加分组
source_group_by_dir("${ALL_FILES}")

## 添加编译可执行文件
add_executable(${PROJECT_NAME} ${ALL_FILES})

## 设置项目的属性
## "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"：窗口程序，不显示控制台窗口
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")