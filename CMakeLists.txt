cmake_minimum_required(VERSION 3.14)
project(termbar VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # 确保生成的代码可以用于共享库/Python模块

# --- 选项配置 ---
option(TERMBAR_BUILD_SHARED "Build shared library (dynamic)" ON)
option(TERMBAR_BUILD_STATIC "Build static library" ON)
option(TERMBAR_BUILD_PYTHON "Build Python bindings (pybind11)" ON) # 新增
option(BUILD_EXAMPLES "Build example programs" OFF)

# 寻找依赖
find_package(Threads REQUIRED)

# 源文件列表
set(TERMBAR_SOURCES src/termbar.cpp)
set(TERMBAR_HEADERS 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# ==========================================
# 1. 构建动态库 (Shared Library)
# ==========================================
if(TERMBAR_BUILD_SHARED)
    add_library(termbar_shared SHARED ${TERMBAR_SOURCES})
    set_target_properties(termbar_shared PROPERTIES OUTPUT_NAME "termbar")
    target_compile_definitions(termbar_shared PRIVATE termbar_EXPORTS) # 定义导出宏
    target_include_directories(termbar_shared PUBLIC ${TERMBAR_HEADERS})
    target_link_libraries(termbar_shared PRIVATE Threads::Threads)
    add_library(termbar::shared ALIAS termbar_shared)
endif()

# ==========================================
# 2. 构建静态库 (Static Library)
# ==========================================
if(TERMBAR_BUILD_STATIC)
    add_library(termbar_static STATIC ${TERMBAR_SOURCES})
    # Windows 下静态库加后缀 _s，避免与 DLL 导入库重名
    if(WIN32)
        set_target_properties(termbar_static PROPERTIES OUTPUT_NAME "termbar_s")
    else()
        set_target_properties(termbar_static PROPERTIES OUTPUT_NAME "termbar")
    endif()
    target_compile_definitions(termbar_static PUBLIC TERMBAR_STATIC)
    target_include_directories(termbar_static PUBLIC ${TERMBAR_HEADERS})
    target_link_libraries(termbar_static PRIVATE Threads::Threads)
    add_library(termbar::static ALIAS termbar_static)
endif()

# ==========================================
# 3. 构建 Python 绑定 (Pybind11)
# ==========================================
if(TERMBAR_BUILD_PYTHON)
    message(STATUS "Configuring Python bindings...")
    
    # 使用 FetchContent 自动下载 pybind11 (无需手动安装)
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG        v2.11.1
    )

    set(PYBIND11_FINDPYTHON ON)

    # 这一步会引入 pybind11 的 CMake 函数
    FetchContent_MakeAvailable(pybind11)

    # 创建 Python 模块 target
    pybind11_add_module(termbar_py src/bindings.cpp ${TERMBAR_SOURCES})

    # 设置输出名字为 "termbar"，这样 Python 里直接 "import termbar"
    set_target_properties(termbar_py PROPERTIES OUTPUT_NAME "termbar")
    
    # 包含头文件
    target_include_directories(termbar_py PRIVATE include)
    
    # 链接线程库
    target_link_libraries(termbar_py PRIVATE Threads::Threads)

    # Windows 优化：将生成的 .pyd 文件输出到 build 根目录，方便测试
    if(WIN32)
        set_target_properties(termbar_py PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
    endif()
endif()

# ==========================================
# 4. 构建 C++ 示例 (Examples)
# ==========================================
if(BUILD_EXAMPLES)
    # 自动选择链接目标：优先动态，其次静态
    if(TERMBAR_BUILD_SHARED)
        set(TERMBAR_TARGET_LINK termbar::shared)
    elseif(TERMBAR_BUILD_STATIC)
        set(TERMBAR_TARGET_LINK termbar::static)
    else()
        message(FATAL_ERROR "Examples require either Shared or Static library to be built.")
    endif()

    message(STATUS "Building examples linking against: ${TERMBAR_TARGET_LINK}")

    # 定义一个宏方便添加示例
    macro(add_termbar_example name source)
        add_executable(${name} ${source})
        target_link_libraries(${name} PRIVATE ${TERMBAR_TARGET_LINK})
        # Windows 动态库拷贝逻辑
        if(WIN32 AND TERMBAR_BUILD_SHARED)
            add_custom_command(TARGET ${name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:termbar_shared> $<TARGET_FILE_DIR:${name}>)
        endif()
    endmacro()

    add_termbar_example(demo_download examples/01_download.cpp)
    add_termbar_example(demo_thread examples/02_thread_worker.cpp)
    add_termbar_example(demo_multistage examples/03_multi_stage.cpp)
endif()