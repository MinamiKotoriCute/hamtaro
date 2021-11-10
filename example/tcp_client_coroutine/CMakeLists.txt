cmake_minimum_required(VERSION 3.5)

project(example_tcp_client_coroutine LANGUAGES CXX)

set(example_tcp_client_coroutine_src
    main.cpp
)

add_executable(example_tcp_client_coroutine
    ${example_tcp_client_coroutine_src}
)

target_link_libraries(example_tcp_client_coroutine
    PRIVATE
        hamtaro
        glog
)

target_include_directories(example_tcp_client_coroutine
    PRIVATE
        hamtaro
)
