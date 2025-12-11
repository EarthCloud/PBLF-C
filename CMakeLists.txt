cmake_minimum_required(VERSION 3.10)

# 项目名称
project(Chatbot C)

# 设置 C 标准 (C99 或 C11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 1. 告诉 CMake 头文件在哪里
include_directories(include)

# 2. 定义源文件列表
set(SOURCES
        src/main.c
        src/chatbot.c
        src/user.c
        src/llm.c
        src/http.c
        src/game.c
        src/memo.c
        src/todo.c
        src/contact.c
)

# 3. 生成可执行文件 (CMake 会自动根据系统在 Windows 下生成 chatbot.exe，Linux 下生成 chatbot)
add_executable(chatbot ${SOURCES})

# 4. 编译选项 (CFLAGS)
# 对应: -Wall -g -finput-charset=UTF-8 -fexec-charset=UTF-8
if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(chatbot PRIVATE
            -Wall
            -g
            -finput-charset=UTF-8
    )
endif ()

# 5. Windows 下网络编程必须链接 ws2_32 库
if (WIN32)
    target_link_libraries(chatbot ws2_32)
endif ()

# 6. (可选) 设置输出目录
# 让编译出来的 chatbot.exe 直接生成在项目根目录，而不是深藏在 cmake-build-debug 里
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})