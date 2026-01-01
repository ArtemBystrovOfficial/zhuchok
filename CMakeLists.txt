cmake_minimum_required(VERSION 3.10)
project(zhuchok)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    GameRunner.cpp
)

set(HEADERS
    common.hpp
)

add_executable(GameProject ${SOURCES} ${HEADERS})