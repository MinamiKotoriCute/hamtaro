cmake_minimum_required(VERSION 3.5)


FIND_PACKAGE( Boost 1.78 )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

project(hamtaro LANGUAGES CXX)


set(hamtaro_common
    common/awaitable.h
    common/error_handle.h
    common/pack_coder.cpp
    common/pack_coder.h
    common/pack_maker.cpp
    common/pack_maker.h
    common/pack_parser.cpp
    common/pack_parser.h
    common/pack_tcp_reader.cpp
    common/pack_tcp_reader.h
    common/protobuf_tcp_client.h
    common/result.cpp
    common/result.h
    common/tcp_client.cpp
    common/tcp_client.h
    common/tcp_server.cpp
    common/tcp_server.h
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
        protobuf
)

target_include_directories(hamtaro PUBLIC .)

add_subdirectory(example)
add_subdirectory(pb)
