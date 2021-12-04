cmake_minimum_required(VERSION 3.5)

project(example_http_server_listen LANGUAGES CXX)

set(example_http_server_listen_src
    main.cpp
)

add_executable(example_http_server_listen
    ${example_http_server_listen_src}
)

target_link_libraries(example_http_server_listen
    PRIVATE
        hamtaro
)

target_include_directories(example_http_server_listen
    PRIVATE
        hamtaro
)

set_target_properties(example_http_get_coroutine PROPERTIES OUTPUT_NAME "example")
