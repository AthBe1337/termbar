cmake_minimum_required(VERSION 3.10)
project(termbar VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 构建库 ---
add_library(termbar src/termbar.cpp)

# 设置头文件路径，这样外部引用时只需 #include "termbar.h"
target_include_directories(termbar PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 链接线程库 (mutex 需要)
find_package(Threads REQUIRED)
target_link_libraries(termbar PRIVATE Threads::Threads)

# --- 构建示例 (可选) ---
option(BUILD_EXAMPLES "Build example programs" ON)

if(BUILD_EXAMPLES)
    # 示例 1: 基础下载
    add_executable(demo_download examples/01_download.cpp)
    target_link_libraries(demo_download PRIVATE termbar)

    # 示例 2: 多线程
    add_executable(demo_thread examples/02_thread_worker.cpp)
    target_link_libraries(demo_thread PRIVATE termbar)

    # 示例 3: 多阶段颜色
    add_executable(demo_multistage examples/03_multi_stage.cpp)
    target_link_libraries(demo_multistage PRIVATE termbar)
endif()
