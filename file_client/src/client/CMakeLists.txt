add_library(client client_impl.cpp)

find_package(spdlog REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

target_include_directories(client PRIVATE ${Boost_INCLUDE_DIR})

target_link_libraries(client 
                      PRIVATE ${Boost_LIBRARIES}
                      PUBLIC spdlog::spdlog )