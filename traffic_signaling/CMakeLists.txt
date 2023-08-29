cmake_minimum_required(VERSION 3.24)
project(traffic_signaling VERSION 1.0)

# specify the C++ standard
add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_20)

# Create helper variables to determine which compiler is used
set(IS_GCC "$<COMPILE_LANG_AND_ID:CXX,GNU>")
set(IS_MSVC "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

# Distinction between Clang and Clang-cl due to inconsistent compiler flags
set(IS_CLANG_LIKE "$<COMPILE_LANG_AND_ID:CXX,Clang>")
set(IS_GNU_DRIVER "$<STREQUAL:${CMAKE_CXX_COMPILER_FRONTEND_VARIANT},GNU>")
set(IS_MSVC_DRIVER "$<STREQUAL:${CMAKE_CXX_COMPILER_FRONTEND_VARIANT},MSVC>")
set(IS_CLANG "$<AND:${IS_CLANG_LIKE},${IS_GNU_DRIVER}>")
set(IS_CLANG_CL "$<AND:${IS_CLANG_LIKE},${IS_MSVC_DRIVER}>")

# Add compile options to the interface library compiler_flags
target_compile_options(compiler_flags INTERFACE
    "$<$<OR:${IS_GCC},${IS_CLANG}>:$<BUILD_INTERFACE:$<IF:$<CONFIG:Debug>,,-O3>;-Wall;-Wextra;-Wpedantic>>"
    "$<${IS_CLANG_CL}:$<BUILD_INTERFACE:$<IF:$<CONFIG:Debug>,,/O2>;/W4;-Wpedantic>>"
    "$<${IS_MSVC}:$<BUILD_INTERFACE:$<IF:$<CONFIG:Debug>,,/O2>;/W4>>"
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

add_executable(test_basic tests/test_basic.cpp "${source_files}")
target_link_libraries(test_basic PUBLIC compiler_flags)
target_include_directories(test_basic PUBLIC include)

add_executable(test_time tests/test_time.cpp "${source_files}")
target_link_libraries(test_time PUBLIC compiler_flags)
target_include_directories(test_time PUBLIC include)

#find_package(Python COMPONENTS Interpreter Development REQUIRED)
#message("Python executable: ${Python_EXECUTABLE}")
#execute_process(COMMAND
#        "${Python_EXECUTABLE}" -m pybind11 --cmakedir
#        OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
#        OUTPUT_STRIP_TRAILING_WHITESPACE
#        )
#message("PYBIND11_CMAKE_DIR: ${PYBIND11_CMAKE_DIR}")
#set(CMAKE_PREFIX_PATH "${PYBIND11_CMAKE_DIR}")
#message("CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
#message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
#find_package(pybind11 REQUIRED)

include(FetchContent)

FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(pybind11)

pybind11_add_module(traffic_signaling src/bindings.cpp "${source_files}")
target_link_libraries(traffic_signaling PUBLIC compiler_flags)
target_include_directories(traffic_signaling PUBLIC include)

include(CTest)

function(do_test target arg result)
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/output/")
    add_test(NAME "${target} ${arg}" COMMAND "${target}" "${CMAKE_SOURCE_DIR}/data/${arg}" "${CMAKE_BINARY_DIR}/output/${arg}")
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

function(test_python)

endfunction()