cmake_minimum_required(VERSION 3.22)

project(file_server
        VERSION 0.1
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_subdirectory(server)

add_executable(file_server main.cpp)

target_link_libraries(file_server PRIVATE server)