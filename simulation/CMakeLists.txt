cmake_minimum_required(VERSION 3.22)
project(simulation)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    # all warnings and all warnings as errors, /O2 for optimization
    add_compile_options(/std:c++20 /Wall)# /WX)
else ()
    # lots of warnings and all warnings as errors, -O3 for optimization
    add_compile_options(-std=c++20 -Wall -Wextra -Wpedantic)# -Werror)
endif ()

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
