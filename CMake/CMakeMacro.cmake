# 声明按文件夹分组源码的宏
# SOURCE_GROUP_BY_DIR: 宏名称
# SOURCE_FILES: 源码文件集合
macro(SOURCE_GROUP_BY_DIR SOURCE_FILES)
    foreach(FILE ${SOURCE_FILES})
        # 获取源文件路径
        get_filename_component(PARENT_DIR "${FILE}" PATH)
        # 正则表达式分组规则
        string(REGEX REPLACE "(\\./)?(Source)/?" "" GROUP "${PARENT_DIR}")
        string(REPLACE "/" "\\" GROUP "${GROUP}")
        source_group("${GROUP}" FILES "${FILE}")
    endforeach(FILE)
endmacro(SOURCE_GROUP_BY_DIR)

# 声明生成预编译头文件宏，参数如下
# USE_MSVC_PCH: 宏名称
# PCH_TARGET: 项目名称
# PCH_HEADER_FILE: 预编译头文件名称(stdafx.h)
# PCH_SOURCE_FILE: 预编译源文件名称(stdafx.cpp)
macro(USE_MSVC_PCH PCH_TARGET PCH_HEADER_FILE PCH_SOURCE_FILE)
    if(MSVC)
        # 获取预编译头文件的文件名，通常是stdafx
        get_filename_component(PCH_NAME ${PCH_HEADER_FILE} NAME_WE)
        # 生成预编译文件的路径
        if(CMAKE_CONFIGURATION_TYPES)
            # 如果有配置选项(Debug/Release)，路径添加以及配置选项
            SET(PCH_DIR "${CMAKE_CURRENT_BINARY_DIR}/PCH/${CMAKE_CFG_INTDIDR}")
        else(CMAKE_CONFIGURATION_TYPES)
            SET(PCH_DIR "${CMAKE_CURRENT_BINARY_DIR}/PCH")
        endif(CMAKE_CONFIGURATION_TYPES)

        # 创建预编译文件的路径
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/PCH)

        # 设置项目属性，使用预编译头文件
        set_target_properties(${PCH_TARGET} PROPERTIES COMPILE_FLAGS
            "/Yu${PCH_HEADER_FILE} /FI${PCH_HEADER_FILE} /Fp${PCH_DIR}${PCH_NAME}.pch")

        # 预编译源文件(stdafx.cpp)设置属性，创建预编译文件
        set_source_files_properties(${PCH_SOURCE_FILE} PROPERTIES COMPILE_FLAGS
            "/Yc${PCH_HEADER_FILE}")

        # 把预编译文件寄到清除列表
        set_directory_properties(PROPERTIES
            ADDITIONAL_MAKE_CLEAN_FILES $(PCH_DIR)${PCH_NAME}.pch)
    endif(MSVC)
endmacro(USE_MSVC_PCH)
