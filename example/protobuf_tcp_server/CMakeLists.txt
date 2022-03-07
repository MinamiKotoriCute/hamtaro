cmake_minimum_required(VERSION 3.5)

project(example_ptorobuf_tcp_server LANGUAGES CXX)

set(example_ptorobuf_tcp_server_src
    main.cpp
)

add_executable(example_ptorobuf_tcp_server
    ${example_ptorobuf_tcp_server_src}
)

target_link_libraries(example_ptorobuf_tcp_server
    PRIVATE
        hamtaro
)

target_include_directories(example_ptorobuf_tcp_server
    PRIVATE
        hamtaro
)
