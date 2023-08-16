cmake_minimum_required(VERSION 3.22)
project(traffic_signaling VERSION 1.0)

# specify the C++ standard
add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_20)

# Create helper variables to determine which compiler is used
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,ARMClang,AppleClang>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

# Add compile options to the interface library compiler_flags.
target_compile_options(compiler_flags INTERFACE
        "$<${gcc_like_cxx}:$<BUILD_INTERFACE:$<IF:$<CONFIG:Debug>,,-O3>;-Wall;-Wextra;-Wpedantic>>"
        "$<${msvc_cxx}:$<BUILD_INTERFACE:$<IF:$<CONFIG:Debug>,,/O2>;/W4>>"
        )

set(source_files
        src/city_plan/city_plan.cpp
        src/city_plan/intersection.cpp
        src/city_plan/street.cpp
        src/city_plan/car.cpp
        src/simulation/event.cpp
        src/simulation/schedule.cpp
        src/simulation/simulation.cpp
        src/simulation/intersection.cpp
        src/simulation/street.cpp
        src/simulation/car.cpp
        )

add_executable(test_basic tests/test_basic.cpp ${source_files})
target_link_libraries(test_basic PUBLIC compiler_flags)
target_include_directories(test_basic PUBLIC include)

add_executable(test_time tests/test_time.cpp ${source_files})
target_link_libraries(test_time PUBLIC compiler_flags)
target_include_directories(test_time PUBLIC include)

include(CTest)

function(do_test target arg result)
    add_test(NAME "${target} ${arg}" COMMAND "${target}" "${CMAKE_SOURCE_DIR}/data/${arg}")
    set_tests_properties("${target} ${arg}" PROPERTIES PASS_REGULAR_EXPRESSION ${result})
endfunction()

do_test(test_basic a.txt "1,001 points")
do_test(test_basic b.txt "4,566,576 points")
do_test(test_basic c.txt "1,299,357 points")
do_test(test_basic d.txt "1,573,100 points")
do_test(test_basic e.txt "684,769 points")
do_test(test_basic f.txt "819,083 points")

do_test(test_time a.txt ".*")
do_test(test_time b.txt ".*")
do_test(test_time c.txt ".*")
do_test(test_time d.txt ".*")
do_test(test_time e.txt ".*")
do_test(test_time f.txt ".*")