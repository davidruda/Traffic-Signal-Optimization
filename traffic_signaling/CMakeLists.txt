cmake_minimum_required(VERSION 3.22)
project(traffic_signaling)

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
#        C:/Users/DAVID_/AppData/Local/Programs/Python/Python310/include
#)

add_executable(
        test_basic
        tests/test_basic.cpp
        src/event.cpp
        src/schedule.cpp
        src/city_plan.cpp
        src/city_plan_intersection.cpp
        src/city_plan_street.cpp
        src/city_plan_car.cpp
        src/simulation.cpp
        src/simulation_intersection.cpp
        src/simulation_street.cpp
        src/simulation_car.cpp
)

target_include_directories(test_basic PUBLIC include)

add_executable(
        test_time
        tests/test_time.cpp
        src/event.cpp
        src/schedule.cpp
        src/city_plan.cpp
        src/city_plan_intersection.cpp
        src/city_plan_street.cpp
        src/city_plan_car.cpp
        src/simulation.cpp
        src/simulation_intersection.cpp
        src/simulation_street.cpp
        src/simulation_car.cpp
)

target_include_directories(test_time PUBLIC include)
