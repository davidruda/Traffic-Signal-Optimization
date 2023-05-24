cmake_minimum_required(VERSION 3.25)
project(cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(cpp src/main.cpp src/intersection_instance.cpp src/street_instance.cpp src/car_instance.cpp src/simulation_instance.cpp src/plan_instance.cpp src/simulation_shared.cpp)

target_include_directories(cpp PUBLIC include)

if (MSVC)
    # all warnings and all warnings as errors
    add_compile_options(/Wall /WX)
else ()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif ()