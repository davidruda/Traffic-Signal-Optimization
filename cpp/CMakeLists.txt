cmake_minimum_required(VERSION 3.22)
project(cpp)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    # all warnings and all warnings as errors
    add_compile_options(/Wall)# /WX)
else ()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -Wpedantic)# -Werror)
endif ()

add_executable(
        main
        main.cpp
        src/intersection.cpp
        src/street.cpp
        src/car.cpp
        src/simulation.cpp
        src/event.cpp
        src/schedule.cpp
)

target_include_directories(main PUBLIC include)

add_executable(
        test_time
        test_time.cpp
        src/intersection.cpp
        src/street.cpp
        src/car.cpp
        src/simulation.cpp
        src/event.cpp
        src/schedule.cpp
)

target_include_directories(test_time PUBLIC include)
