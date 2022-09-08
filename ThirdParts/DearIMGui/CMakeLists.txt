# 最小CMake版本需求
cmake_minimum_required(VERSION 3.0)
# 当前项目名
project(DearIMGui)

# 测试输出项目名称
message(STATUS "DearIMGui->CMakeList.txt : CMAKE_PROJECT_NAME = ${CMAKE_PROJECT_NAME}")
message(STATUS "DearIMGui->CMakeList.txt : PROJECT_NAME = ${PROJECT_NAME}")

# 测试输入内置目录
message(STATUS "DearIMGui->CMakeList.txt : CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
message(STATUS "DearIMGui->CMakeList.txt : PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
message(STATUS "DearIMGui->CMakeList.txt : CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
message(STATUS "DearIMGui->CMakeList.txt : PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR}")
message(STATUS "DearIMGui->CMakeList.txt : CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "DearIMGui->CMakeList.txt : CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "DearIMGui->CMakeList.txt : CMAKE_CURRENT_LIST_DIR = ${CMAKE_CURRENT_LIST_DIR}")

# 包含CMake宏定义文件
include("${CMAKE_SOURCE_DIR}/CMake/CMakeMacro.cmake")
# 添加预定义宏
add_definitions(-DImTextureID=ImU64)

# 代码文件
set(ALL_FILES 
Source/imconfig.h
Source/imgui.cpp
Source/imgui.h
Source/imgui_demo.cpp
Source/imgui_draw.cpp
Source/imgui_impl_dx12.cpp
Source/imgui_impl_dx12.h
Source/imgui_impl_win32.cpp
Source/imgui_impl_win32.h
Source/imgui_internal.h
Source/imgui_tables.cpp
Source/imgui_widgets.cpp
Source/imstb_rectpack.h
Source/imstb_textedit.h
Source/imstb_truetype.h
)

# 调用CMake宏定义文件定义的宏
source_group_by_dir("${ALL_FILES}")
# 项目添加为静态库
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
# 项目添加在ThirdParts文件夹下
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ThirdParts)