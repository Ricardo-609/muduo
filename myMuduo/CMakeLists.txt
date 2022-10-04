#cmake版本
cmake_minimum_required(VERSION 2.5)

#工程名称
project(myMuduo)

#myMuduo最终编译成so动态库，下面是设置动态库的路径, 放在根目录lib文件夹下
set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

#设置调试信息, 以及启动C++11语言标准
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

#定义参与编译的源文件
aux_source_directory(. SRC_LIST)

#编译动态库
add_library(myMuduo SHARED ${SRC_LIST})

