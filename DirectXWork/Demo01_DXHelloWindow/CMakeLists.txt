## 最小CMake版本要求
cmake_minimum_required(VERSION 3.0)

## 项目名
project(Demo01_DXHelloWindow)

# 测试输出项目名称
message(STATUS "DXHelloWorldWindow->CMakeList.txt : CMAKE_PROJECT_NAME = ${CMAKE_PROJECT_NAME}")
message(STATUS "DXHelloWorldWindow->CMakeList.txt : PROJECT_NAME = ${PROJECT_NAME}")

# 测试输入内置目录
message(STATUS "DXHelloWorldWindow->CMakeList.txt : CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
message(STATUS "DXHelloWorldWindow->CMakeList.txt : PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
message(STATUS "DXHelloWorldWindow->CMakeList.txt : CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
message(STATUS "DXHelloWorldWindow->CMakeList.txt : PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR}")
message(STATUS "DXHelloWorldWindow->CMakeList.txt : CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "DXHelloWorldWindow->CMakeList.txt : CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "DXHelloWorldWindow->CMakeList.txt : CMAKE_CURRENT_LIST_DIR = ${CMAKE_CURRENT_LIST_DIR}")

# 包含cmake文件
include("${CMAKE_SOURCE_DIR}/CMake/CMakeMacro.cmake")

# 设置头文件引用路径
include_directories(${CMAKE_SOURCE_DIR}/ThirdParts/DearIMGui/Source)
include_directories(${CMAKE_SOURCE_DIR}/DirectXWork/Lib_DXBaseApp/Source)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Source/)

# 预编译头文件和源文件的路径
set(STDAFX_PCH_H ${CMAKE_CURRENT_SOURCE_DIR}/Source/stdafx.h)
set(STDAFX_PCH_C ${CMAKE_CURRENT_SOURCE_DIR}/Source/stdafx.cpp)

# 将所有源码设置到ALL_FILES变量上
set(ALL_FILES 
Source/Main.cpp
Source/stdafx.cpp
Source/stdafx.h
Source/DXWorkHelloWindow/D3D12HelloWindow.cpp
Source/DXWorkHelloWindow/D3D12HelloWindow.h
Source/DXWorkHelloWindow/IMGuiHelloWindow.cpp
Source/DXWorkHelloWindow/IMGuiHelloWindow.h
)

# 设置链接的依赖库
set(COMMON_LIB 
DearIMGui
Lib_DXBaseApp
)

## 调用外部cmake定义的宏，将源文件按文件加分组
source_group_by_dir("${ALL_FILES}")

## 添加编译可执行文件
add_executable(${PROJECT_NAME} ${ALL_FILES})
## 添加DX链接库
target_link_libraries(${PROJECT_NAME} ${COMMON_LIB})
# 项目添加在ThirdParts文件夹下
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER JoyDirectXSample)
## 设置项目的属性
## "/SUBSYSTEM:WINDOWS /ENTRY:winMainCRTStartup"：窗口程序，不显示控制台窗口
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup")
# 调用定义在CMakeMacro.cmake中的宏，使用预编译头
USE_MSVC_PCH(${PROJECT_NAME} stdafx.h ${STDAFX_PCH_C})