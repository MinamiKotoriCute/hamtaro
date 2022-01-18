cmake_minimum_required(VERSION 3.5)

project(example_tcp_client LANGUAGES CXX)

set(example_tcp_client_src
    main.cpp
)

add_executable(example_tcp_client
    ${example_tcp_client_src}
)

target_link_libraries(example_tcp_client
    PRIVATE
        hamtaro
)

target_include_directories(example_tcp_client
    PRIVATE
        hamtaro
)
