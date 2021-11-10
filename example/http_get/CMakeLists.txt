cmake_minimum_required(VERSION 3.5)

project(example_http_get LANGUAGES CXX)

set(example_http_get_src
    main.cpp
)

add_executable(example_http_get
    ${example_http_get_src}
)

target_link_libraries(example_http_get
    PRIVATE
        hamtaro
)

target_include_directories(example_http_get
    PRIVATE
        hamtaro
)
