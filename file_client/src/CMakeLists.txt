cmake_minimum_required(VERSION 3.26)

project(file_client
        VERSION 0.1
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_subdirectory(client)

add_executable(file_client main.cpp)

find_package(spdlog REQUIRED)

target_link_libraries(file_client PRIVATE client spdlog::spdlog)