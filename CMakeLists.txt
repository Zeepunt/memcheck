cmake_minimum_required(VERSION 3.22)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

project(memcheck LANGUAGES C CXX)

file(GLOB_RECURSE memcheck_src CONFIGURE_DEPENDS src/*.c)
file(GLOB_RECURSE example_src CONFIGURE_DEPENDS example/Linux/*.c)

add_executable(example)

target_sources(example PRIVATE ${memcheck_src} ${example_src})
target_include_directories(example PUBLIC inc)
