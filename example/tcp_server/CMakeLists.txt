cmake_minimum_required(VERSION 3.5)

project(example_tcp_server LANGUAGES CXX)

set(example_tcp_server_src
    main.cpp
)

add_executable(example_tcp_server
    ${example_tcp_server_src}
)

target_link_libraries(example_tcp_server
    PRIVATE
        hamtaro
)

target_include_directories(example_tcp_server
    PRIVATE
        hamtaro
)
