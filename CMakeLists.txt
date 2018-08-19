cmake_minimum_required (VERSION 3.8)
project(jsonlib)

# Set up the output directory for builds
set(JSONLIB_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")
message("Current output directory ${JSONLIB_OUTPUT_DIR}")

include_directories(.)

set(JSONLIB_SOURCES "jsonlib.h" "jsonlib.cpp")
add_library(jsonlib STATIC ${JSONLIB_SOURCES})
set_target_properties(jsonlib PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${JSONLIB_OUTPUT_DIR}")

enable_testing()

set(JSONLIB_TEST_SOURCES "jsonlib.h" "test/catch.hpp" "test/tests.cpp")
add_executable(testjsonlib ${JSONLIB_TEST_SOURCES})
target_link_libraries(testjsonlib jsonlib)
set_target_properties(testjsonlib PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${JSONLIB_OUTPUT_DIR}")

add_test(jsonlib-test ${JSONLIB_OUTPUT_DIR}/testjsonlib)