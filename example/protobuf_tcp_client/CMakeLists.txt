cmake_minimum_required(VERSION 3.5)

project(example_ptorobuf_tcp_client LANGUAGES CXX)

set(example_ptorobuf_tcp_client_src
    main.cpp
)

add_executable(example_ptorobuf_tcp_client
    ${example_ptorobuf_tcp_client_src}
)

target_link_libraries(example_ptorobuf_tcp_client
    PRIVATE
        hamtaro
        pb
)

target_include_directories(example_ptorobuf_tcp_client
    PRIVATE
        hamtaro
        pb
)
