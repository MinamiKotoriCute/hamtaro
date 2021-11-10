cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

project(hamtaro LANGUAGES CXX)


set(hamtaro_error
    error/error_define.h
    error/error_define.cpp
)

set(hamtaro_http
    http/http_client.h
    http/tcp_client.h
    http/url_parser.h
    http/url_parser.cpp
)

add_library(hamtaro
    ${hamtaro_error}
    ${hamtaro_http}
)

target_link_libraries(hamtaro
    INTERFACE
        -pthread
)

target_include_directories(hamtaro PUBLIC .)

add_subdirectory(example)
