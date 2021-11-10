cmake_minimum_required(VERSION 3.5)

project(example_http_get_coroutine LANGUAGES CXX)

set(example_http_get_coroutine_src
    main.cpp
)

add_executable(example_http_get_coroutine
    ${example_http_get_coroutine_src}
)

target_link_libraries(example_http_get_coroutine
    PRIVATE
        hamtaro
)

target_include_directories(example_http_get_coroutine
    PRIVATE
        hamtaro
)
