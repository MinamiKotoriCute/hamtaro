cmake_minimum_required(VERSION 3.5)


FIND_PACKAGE( Boost 1.78 )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

project(hamtaro LANGUAGES CXX)


set(hamtaro_common
    common/error_handle.h
    common/result.cpp
    common/result.h
    common/tcp_client.cpp
    common/tcp_client.h
    common/url_parser.cpp
    common/url_parser.h
)

add_library(hamtaro
    ${hamtaro_common}
)

target_link_libraries(hamtaro
    INTERFACE
        -pthread
        glog
)

target_include_directories(hamtaro PUBLIC .)

add_subdirectory(example)
