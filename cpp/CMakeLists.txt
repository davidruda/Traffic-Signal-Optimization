cmake_minimum_required(VERSION 3.25)
project(cpp)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    # all warnings and all warnings as errors
    add_compile_options(/Wall)# /WX)
else ()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -Wpedantic)# -Werror)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # enable exceptions for clang
        add_compile_options(-fexceptions)# -fcxx-exceptions)
    endif ()
endif ()

add_executable(
        cpp
        src/main.cpp
        src/intersection.cpp
        src/street.cpp
        src/car.cpp
        src/simulation.cpp
        src/plan.cpp
        src/event.cpp
)

target_include_directories(cpp PUBLIC include)

