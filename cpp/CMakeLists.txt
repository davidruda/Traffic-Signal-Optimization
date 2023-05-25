cmake_minimum_required(VERSION 3.25)
project(cpp)

set(CMAKE_CXX_STANDARD 20)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fexceptions -fcxx-exceptions")

if (MSVC)
    # all warnings and all warnings as errors
    add_compile_options(/Wall)# /WX)
else ()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -Wpedantic)# -Werror)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # enable exceptions for clang
        add_compile_options(-fexceptions )# -fcxx-exceptions)
    endif ()
endif ()

add_executable(
        cpp
        src/main.cpp
        src/intersection_shared.cpp
        src/intersection_instance.cpp
        src/street_shared.cpp
        src/street_instance.cpp
        src/car_shared.cpp
        src/car_instance.cpp
        src/simulation_shared.cpp
        src/simulation_instance.cpp
        src/schedule_instance.cpp
        src/event.cpp
        )

target_include_directories(cpp PUBLIC include)

