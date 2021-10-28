cmake_minimum_required(VERSION 3.5)

project(example_http LANGUAGES CXX)

set(example_http_src
    main.cpp
)

add_executable(example_http
    ${example_http_src}
)

target_link_libraries(example_http
    PRIVATE
        hamtaro
)

target_include_directories(example_http
    PRIVATE
        hamtaro
)
