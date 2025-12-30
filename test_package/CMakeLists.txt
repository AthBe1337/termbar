cmake_minimum_required(VERSION 3.14)
project(test_client LANGUAGES CXX)

# 引入顶层项目 (这里为了演示方便，直接添加上级目录)
# 在实际项目中，通常是用 find_package(termbar)
add_subdirectory(.. build_lib) 

# ==========================================
# 场景 A: 链接动态库 (Shared)
# ==========================================
add_executable(client_shared main.cpp)
target_link_libraries(client_shared PRIVATE termbar::shared)

# Windows 特殊处理：运行需要 DLL
# 编译后自动将 termbar.dll 复制到 exe 旁边
if(WIN32)
    add_custom_command(TARGET client_shared POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:termbar_shared>
        $<TARGET_FILE_DIR:client_shared>
    )
endif()

# ==========================================
# 场景 B: 链接静态库 (Static)
# ==========================================
add_executable(client_static main.cpp)
target_link_libraries(client_static PRIVATE termbar::static)