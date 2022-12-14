cmake_minimum_required(VERSION 3.0)
# 当前项目名
# project(Lib_DXBaseApp)

# 测试输出项目名称
# message(STATUS "DXBaseApp->CMakeList.txt : CMAKE_PROJECT_NAME = ${CMAKE_PROJECT_NAME}")
# message(STATUS "DXBaseApp->CMakeList.txt : PROJECT_NAME = ${PROJECT_NAME}")

# 测试输入内置目录
# message(STATUS "DXBaseApp->CMakeList.txt : CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
# message(STATUS "DXBaseApp->CMakeList.txt : PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
# message(STATUS "DXBaseApp->CMakeList.txt : CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
# message(STATUS "DXBaseApp->CMakeList.txt : PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR}")
# message(STATUS "DXBaseApp->CMakeList.txt : CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
# message(STATUS "DXBaseApp->CMakeList.txt : CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")
# message(STATUS "DXBaseApp->CMakeList.txt : CMAKE_CURRENT_LIST_DIR = ${CMAKE_CURRENT_LIST_DIR}")

# 包含CMake宏定义文件
include("${CMAKE_SOURCE_DIR}/CMake/CMakeMacro.cmake")

# 添加头文件搜索路径
include_directories(${CMAKE_SOURCE_DIR}/ThirdParts/DearIMGui/Source/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Source/)

# 预编译头文件和源文件的路径
set(STDAFX_PCH_H ${CMAKE_CURRENT_SOURCE_DIR}/Source/stdafx.h)
set(STDAFX_PCH_C ${CMAKE_CURRENT_SOURCE_DIR}/Source/stdafx.cpp)

set(SUB_PROJECT_NAME Lib_DXBaseApp)

# 将所有源码设置到ALL_FILES变量上
set(ALL_FILES 
Source/stdafx.cpp
Source/stdafx.h
Source/Application/Win32Application.cpp
Source/Application/Win32Application.h
Source/Application/GameTimer.cpp
Source/Application/GameTimer.h
Source/DirectXBaseWork/DirectXBaseWork.cpp
Source/DirectXBaseWork/DirectXBaseWork.h
Source/DirectXBaseWork/d3dx12.h
Source/DirectXBaseWork/DXWorkHelper.h
Source/DirectXBaseWork/UploadBuffer.h
Source/DirectXBaseWork/D3D12Util.cpp
Source/DirectXBaseWork/D3D12Util.h
Source/DirectXBaseWork/MathUtil.cpp
Source/DirectXBaseWork/MathUtil.h
Source/DirectXBaseWork/GeometryGenerator.cpp
Source/DirectXBaseWork/GeometryGenerator.h
Source/DirectXBaseWork/RenderConstantsStruct.h
Source/DearIMGuiHelper/DearIMGuiBaseHelper.cpp
Source/DearIMGuiHelper/DearIMGuiBaseHelper.h
)
# 设置链接的依赖库
set(COMMON_LIBS
DearIMGui
)

## 调用外部cmake定义的宏，将源文件按文件加分组
source_group_by_dir("${ALL_FILES}")

## 添加编译静态库
add_library(${SUB_PROJECT_NAME} STATIC ${ALL_FILES})
## 添加DX链接库
target_link_libraries(${SUB_PROJECT_NAME} ${COMMON_LIBS} d3d12 dxgi d3dcompiler dxguid)
# 项目添加在JoyDirectXSample文件夹下
set_target_properties(${SUB_PROJECT_NAME} PROPERTIES FOLDER JoyDirectXSample)

# 调用定义在CMakeMacro.cmake中的宏，使用预编译头
USE_MSVC_PCH(${SUB_PROJECT_NAME} stdafx.h ${STDAFX_PCH_C})