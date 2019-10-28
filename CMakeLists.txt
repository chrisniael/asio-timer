CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(asio-timer)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-g -std=c++11)
endif(CMAKE_COMPILER_IS_GNUCXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# ASIO 不使用 Boost 库
add_definitions(-DASIO_STANDALONE)

# 生成编译命令文件，给 YCM 使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SOURCE_FILES)

ADD_EXECUTABLE(main ${SOURCE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp)
target_link_libraries(main pthread)
