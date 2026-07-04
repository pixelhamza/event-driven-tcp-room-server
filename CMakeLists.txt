cmake_minimum_required(VERSION 3.10)
project(tcp_chat_server CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chat_server
    src/main.cpp
    src/Socket.cpp
)

target_include_directories(chat_server PRIVATE include)