cmake_minimum_required(VERSION 3.22)
project(simulation)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    # lots of warnings and warnings as errors, /O2 for optimization
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "/O2")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "/O2")
    add_compile_options(/std:c++20 /W4)# /WX)
else ()
    # lots of warnings and warnings as errors, -O3 for optimization
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "-03")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-03")
    add_compile_options(-std=c++20 -Wall -Wextra -Wpedantic)# -Werror)
endif ()

#TODO: add paths to pybind
#include_directories(
#        ../venv/Lib/site-packages/pybind11/include/
#        ../venv/Lib/site-packages/pybind11/include/pybind11
#)

add_executable(
        test_basic
        tests/test_basic.cpp
        src/intersection.cpp
        src/street.cpp
        src/car.cpp
        src/simulation.cpp
        src/event.cpp
        src/schedule.cpp
)

target_include_directories(test_basic PUBLIC include)

add_executable(
        test_time
        tests/test_time.cpp
        src/intersection.cpp
        src/street.cpp
        src/car.cpp
        src/simulation.cpp
        src/event.cpp
        src/schedule.cpp
)

target_include_directories(test_time PUBLIC include)
