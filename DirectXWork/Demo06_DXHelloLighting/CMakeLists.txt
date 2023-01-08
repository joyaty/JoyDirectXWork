## 最小CMake版本要求
cmake_minimum_required(VERSION 3.0)

# 包含CMake宏定义文件
include("${CMAKE_SOURCE_DIR}/CMake/CMakeMacro.cmake")

# 添加头文件搜索路径
include_directories(${CMAKE_SOURCE_DIR}/DirectXWork/Lib_DXBaseApp/Source)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParts/DearIMGui/Source/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Source)

# 预编译头文件和源文件的路径
set(STDAFX_PCH_H ${CMAKE_CURRENT_SOURCE_DIR}/Source/stdafx.h)
set(STDAFX_PCH_C ${CMAKE_CURRENT_SOURCE_DIR}/Source/stdafx.cpp)

set(SUB_PROJECT_NAME Demo06_DXHelloLighting)

# 将所有源码设置到ALL_FILES变量上
set(ALL_FILES 
Source/Main.cpp
Source/stdafx.cpp
Source/stdafx.h
Source/DXWorkHelloLighting/DXHelloLighting.cpp
Source/DXWorkHelloLighting/DXHelloLighting.h
Source/DXWorkHelloLighting/IMGuiHelloLighting.cpp
Source/DXWorkHelloLighting/IMGuiHelloLighting.h
Source/DXWorkHelloLighting/HelloLightingFrameResource.cpp
Source/DXWorkHelloLighting/HelloLightingFrameResource.h
Source/DXWorkHelloLighting/HelloLightingRenderItem.cpp
Source/DXWorkHelloLighting/HelloLightingRenderItem.h
Source/DXWorkHelloLighting/HelloLighting.hlsl
Source/DXWorkHelloLighting/LightingUtil.hlsl
)

# 将所有Shader文件设置到SHADER_FILES变量上
set(SHADER_FILES
Source/DXWorkHelloLighting/HelloLighting.hlsl
Source/DXWorkHelloLighting/LightingUtil.hlsl
)

# 排除编译HLSL文件
set_source_files_properties(${SHADER_FILES} PROPERTIES VS_TOOL_OVERRIDE "None")

# 设置链接的依赖库
set(COMMON_LIB 
DearIMGui
Lib_DXBaseApp
)

## 调用外部cmake定义的宏，将源文件按文件加分组
source_group_by_dir("${ALL_FILES}")

## 添加编译可执行文件
add_executable(${SUB_PROJECT_NAME} ${ALL_FILES})
## 添加DX链接库
target_link_libraries(${SUB_PROJECT_NAME} ${COMMON_LIB})
# 项目添加在JoyDirectXSample文件夹下
set_target_properties(${SUB_PROJECT_NAME} PROPERTIES FOLDER JoyDirectXSample)
## 设置项目的属性
## "/SUBSYSTEM:WINDOWS /ENTRY:winMainCRTStartup"：窗口程序，不显示控制台窗口
set_target_properties(${SUB_PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup")
# 调用定义在CMakeMacro.cmake中的宏，使用预编译头
USE_MSVC_PCH(${SUB_PROJECT_NAME} stdafx.h ${STDAFX_PCH_C})