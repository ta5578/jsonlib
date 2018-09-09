cmake_minimum_required (VERSION 3.5.1)
project(jsonpp)

set (CMAKE_CXX_STANDARD 14)

# Set additional compiler flags
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
endif()

# Set the build type (this only affects single output generators)
if(CMAKE_BUILD_TYPE MATCHES "")
    message(WARNING "CMAKE_BUILD_TYPE was not specified. The build will default to a RELEASE build type. Note that this only affects single configuration generators like Make and Ninja.")
    set(CMAKE_BUILD_TYPE "RELEASE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
else()
    message(STATUS "CMAKE_BUILD_TYPE set to ${CMAKE_BUILD_TYPE}")
    if(CMAKE_BUILD_TYPE MATCHES "debug")
        set(CMAKE_BUILD_TYPE "DEBUG")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
    elseif(CMAKE_BUILD_TYPE MATCHES "release")
        set(CMAKE_BUILD_TYPE "RELEASE")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
    else()
        message(FATAL_ERROR "${CMAKE_BUILD_TYPE} is not supported for this generator! The only accepted values are 'debug' or 'release'!")
    endif()
endif()
message("Compiling with ${CMAKE_CXX_FLAGS}")

# Set up the output directory for builds
set(JSONPP_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")
message("Current output directory ${JSONPP_OUTPUT_DIR}")

include_directories(.)

# Build a library out of the sources
set(JSONPP_SOURCES "jsonpp.hpp" "jsonpp.cpp")
add_library(jsonpp STATIC ${JSONPP_SOURCES})
set_target_properties(jsonpp PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${JSONPP_OUTPUT_DIR}")

enable_testing()

# Build the test suite
set(JSONPP_TEST_SOURCES "jsonpp.hpp" "test/catch.hpp" "test/tests.cpp")
add_executable(testjsonpp ${JSONPP_TEST_SOURCES})
target_link_libraries(testjsonpp jsonpp)
set_target_properties(testjsonpp PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${JSONPP_OUTPUT_DIR}")

add_test(jsonpp-test ${JSONPP_OUTPUT_DIR}/testjsonpp)

# Build the performance tests
message("Building the performance tests...")

set(JSONPP_PERF_SOURCES "jsonpp.hpp" "perf/perfjsonpp.cpp")
add_executable(perfjsonpp ${JSONPP_PERF_SOURCES})
target_link_libraries(perfjsonpp jsonpp)
set_target_properties(perfjsonpp PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${JSONPP_OUTPUT_DIR}")

set(JSONPP_PERF_FILES "${CMAKE_CURRENT_SOURCE_DIR}/perf/tiny.json" "${CMAKE_CURRENT_SOURCE_DIR}/perf/small.json" "${CMAKE_CURRENT_SOURCE_DIR}/perf/medium.json" "${CMAKE_CURRENT_SOURCE_DIR}/perf/large.json")

add_custom_command(
        TARGET perfjsonpp PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${JSONPP_PERF_FILES}
                ${JSONPP_OUTPUT_DIR})