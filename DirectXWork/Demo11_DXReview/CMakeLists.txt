## CMake最小版本要求
cmake_minimum_required(VERSION 3.0)
## 包含CMake宏定义文件
include("${CMAKE_SOURCE_DIR}/CMake/CMakeMacro.cmake")

## 添加头文件搜索路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Source)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParts/DearIMGui/Source/)
## 设置预编译头文件路径
set(STDAFX_PCH_H ${CMAKE_CURRENT_SOURCE_DIR}/Source/stdafx.h)
set(STDAFX_PCH_C ${CMAKE_CURRENT_SOURCE_DIR}/Source/stdafx.cpp)
## 设置一个子项目名称，用于标识子项目
set(SUB_PROJECT_NAME Demo11_DXReview)

# 所有的代码文件
set(ALL_FILES
Source/WinApplication.cpp
Source/WinApplication.h
Source/Lighting.hlsl
Source/DXReviewEntry.hlsl
Source/stdafx.cpp
Source/stdafx.h
Source/Main.cpp
)
# 所有的Shader文件
set(SHADER_FILES
Source/Lighting.hlsl
Source/DXReviewEntry.hlsl
)
# Shader文件排除编译
set_source_files_properties($(SAHDER_FILES) PROPERTIES VS_TOOL_OVERRIDE "None")
## 设置链接的依赖库
set(COMMON_LIB 
DearIMGui
)
## 调用CMakeMacro中的宏，代码文件按文件夹目录分组
source_group_by_dir("${ALL_FILES}")

## 编译为可执行文件
add_executable(${SUB_PROJECT_NAME} ${ALL_FILES})
## 添加链接库
target_link_libraries(${SUB_PROJECT_NAME} ${COMMON_LIB})
## 项目放置在"JoyDirectXSample"文件夹下
set_target_properties(${SUB_PROJECT_NAME} PROPERTIES FOLDER JoyDirectXSample)
## "/SUBSYSTEM:WINDOWS /ENTRY:winMainCRTStartup"：窗口程序，不显示控制台窗口
set_target_properties(${SUB_PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup")
## 调用定义在CMakeMacro.cmake中的宏，使用预编译头
USE_MSVC_PCH(${SUB_PROJECT_NAME} stdafx.h ${STDAFX_PCH_C})