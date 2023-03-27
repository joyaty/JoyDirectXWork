## 最小CMake版本要求
cmake_minimum_required(VERSION 3.0)
# 包含CMake宏定义文件
include("${CMAKE_SOURCE_DIR}/CMake/CMakeMacro.cmake")
# 添加头文件搜索路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Source)
include_directories(${CMAKE_SOURCE_DIR}/DirectXWork/Lib_DXBaseApp/Source)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParts/DearIMGui/Source/)
# 添加预编译头文件和源文件路径
set(STDAFX_PCH_H ${CMAKE_CURRENT_SOURCE_DIR}/Source/stdafx.h)
set(STDAFX_PCH_C ${CMAKE_CURRENT_SOURCE_DIR}/Source/stdafx.cpp)
# 设置一个子项目名，用于标识项目
set(SUB_PROJECT_NAME Demo10_DXHelloGeometryShader)
# 所有源文件设置到ALL_FILES变量上
set(ALL_FILES
Source/Main.cpp
Source/stdafx.cpp
Source/stdafx.h
Source/DXWorkGeometryShader/DXHelloGeometryShader.cpp
Source/DXWorkGeometryShader/DXHelloGeometryShader.h
Source/DXWorkGeometryShader/IMGuiHelloGeometryShader.cpp
Source/DXWorkGeometryShader/IMGuiHelloGeometryShader.h
Source/DXWorkGeometryShader/HelloGeometryShaderFrameResource.cpp
Source/DXWorkGeometryShader/HelloGeometryShaderFrameResource.h
Source/DXWorkGeometryShader/HelloGeometryShaderRenderItem.cpp
Source/DXWorkGeometryShader/HelloGeometryShaderRenderItem.h
Source/DXWorkGeometryShader/HelloGeometryShader.hlsl
Source/DXWorkGeometryShader/Lighting.hlsl
)
# 所有Shader文件设置到SHADER_FILES变量上
set(SHADER_FILES
Source/DXWorkGeometryShader/HelloGeometryShader.hlsl
Source/DXWorkGeometryShader/Lighting.hlsl
)
# Shader文件排除编译
set_source_files_properties(${SHADER_FILES} PROPERTIES VS_TOOL_OVERRIDE "None")
# 设置链接的依赖库
set(COMMON_LIB
DearIMGui
Lib_DXBaseApp
)
# 调用定义在CMakeMacro.cmake的宏，按源文件目录分组
source_group_by_dir("${ALL_FILES}")
# 编译为可执行文件
add_executable(${SUB_PROJECT_NAME} ${ALL_FILES})
# 添加链接库
target_link_libraries(${SUB_PROJECT_NAME} ${COMMON_LIB})
# 项目添加在JoyDirectXSample文件夹下
set_target_properties(${SUB_PROJECT_NAME} PROPERTIES FOLDER JoyDirectXSample)
## "/SUBSYSTEM:WINDOWS /ENTRY:winMainCRTStartup"：窗口程序，不显示控制台窗口
set_target_properties(${SUB_PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup")
# 调用定义在CMakeMacro.cmake中的宏，使用预编译头
USE_MSVC_PCH(${SUB_PROJECT_NAME} stdafx.h ${STDAFX_PCH_C})