add_library(server server_impl.cpp)

find_package(spdlog REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

target_include_directories(server PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(server 
                      PRIVATE ${Boost_LIBRARIES}
                      PUBLIC spdlog::spdlog )