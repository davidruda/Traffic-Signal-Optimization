cmake_minimum_required(VERSION 3.25)
project(cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(cpp main.cpp stable_priority_queue.hpp intersection_shared.hpp intersection_instance.hpp intersection_instance.cpp intersection_shared.cpp)

if (MSVC)
    # all warnings and all warnings as errors
    add_compile_options(/Wall /WX)
else ()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif ()