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
    "$<${IS_CLANG_CL}:$<BUILD_INTERFACE:$<IF:$<CONFIG:Debug>,,/O2>;/W4;-Wpedantic;/EHsc>>"
    "$<${IS_MSVC}:$<BUILD_INTERFACE:$<IF:$<CONFIG:Debug>,,/O2>;/W4>>"
)

set(source_files
    src/city_plan/city_plan.cpp
    src/simulation/event.cpp
    src/simulation/schedule.cpp
    src/simulation/simulation.cpp
)

add_executable(test_io tests/test_io.cpp "${source_files}")
target_link_libraries(test_io PUBLIC compiler_flags)
target_include_directories(test_io PUBLIC include)

add_executable(test_score tests/test_score.cpp "${source_files}")
target_link_libraries(test_score PUBLIC compiler_flags)
target_include_directories(test_score PUBLIC include)

add_executable(test_time tests/test_time.cpp "${source_files}")
target_link_libraries(test_time PUBLIC compiler_flags)
target_include_directories(test_time PUBLIC include)

# Note that the pybind modules have to be built with the same (or compatible)
# compiler as Python. Otherwise, the Python interpreter will crash when
# importing the module with the following error:
# ImportError: DLL load failed while importing <module>: The specified module could not be found.
option(BUILD_PYBIND_MODULES "Build pybind modules via cmake" OFF)
if(BUILD_PYBIND_MODULES)
    include(FetchContent)

    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        FIND_PACKAGE_ARGS
    )
    FetchContent_MakeAvailable(pybind11)
        
    # If there are issues with building pybind11 modules (e.g. on WSL, cmake
    # keeps including the pybind sources from Windows), try to build and install
    # pybind from sources as described here:
    # https://pybind11.readthedocs.io/en/stable/compiling.html#find-package-vs-add-subdirectory
    pybind11_add_module(city_plan src/bindings/city_plan.cpp "${source_files}")
    target_link_libraries(city_plan PUBLIC compiler_flags)
    target_include_directories(city_plan PUBLIC include)

    pybind11_add_module(simulation src/bindings/simulation.cpp "${source_files}")
    target_link_libraries(simulation PUBLIC compiler_flags)
    target_include_directories(simulation PUBLIC include)
endif()

include(CTest)
set(PYTHON_PACKAGE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/traffic_signaling")
set(DATA_DIR "${PYTHON_PACKAGE_SOURCE_DIR}/data")
set(OUT_DIR "${CMAKE_BINARY_DIR}/out")

function(test_cpp target arg result)
    file(MAKE_DIRECTORY "${OUT_DIR}")
    add_test(NAME "${target} ${arg}" COMMAND "${target}" "${DATA_DIR}/${arg}.txt" "${OUT_DIR}/${arg}.txt")
    set_tests_properties("${target} ${arg}" PROPERTIES PASS_REGULAR_EXPRESSION ${result})
endfunction()

test_cpp(test_io a ".*")
test_cpp(test_io b ".*")
test_cpp(test_io c ".*")
test_cpp(test_io d ".*")
test_cpp(test_io e ".*")
test_cpp(test_io f ".*")

test_cpp(test_score a "1,001 points")
test_cpp(test_score b "4,566,576 points")
test_cpp(test_score c "1,299,357 points")
test_cpp(test_score d "1,573,100 points")
test_cpp(test_score e "684,769 points")
test_cpp(test_score f "819,083 points")

test_cpp(test_time a ".*")
test_cpp(test_time b ".*")
test_cpp(test_time c ".*")
test_cpp(test_time d ".*")
test_cpp(test_time e ".*")
test_cpp(test_time f ".*")

if(BUILD_PYBIND_MODULES)
    find_package(Python COMPONENTS Interpreter REQUIRED)
    set(PYTHON_TEST_DIR "${CMAKE_SOURCE_DIR}/tests")

    set(PYTHONPATH
        "$<TARGET_FILE_DIR:city_plan>"
        "$<TARGET_FILE_DIR:simulation>"
        "${PYTHON_PACKAGE_SOURCE_DIR}"
    )

    # Convert PYTHONPATH to a string and replace the list separator with the
    # platform specific separator
    if(WIN32)
        string(REPLACE ";" "\\;" PYTHONPATH "${PYTHONPATH}")
    else()
        string(REPLACE ";" ":" PYTHONPATH "${PYTHONPATH}")
    endif()
    message("PYTHONPATH: ${PYTHONPATH}")

    add_test(NAME test_python_unittest
        COMMAND "${CMAKE_COMMAND}" -E env PYTHONPATH=${PYTHONPATH}
        "${Python_EXECUTABLE}" -m unittest discover -s "${PYTHON_TEST_DIR}"
    )
endif()