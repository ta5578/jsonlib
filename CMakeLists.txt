cmake_minimum_required (VERSION 3.5.1)
project(jsonpp)

set (CMAKE_CXX_STANDARD 14)

# Set additional compiler flags and link directories
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
else()
    message(FATAL_ERROR "${CMAKE_CXX_COMPILER_ID} is not supported!")
endif()

# Set up the output directory for builds
set(JSONPP_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")

# Set the generic runtime output directory for single generators like "make"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${JSONPP_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${JSONPP_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${JSONPP_OUTPUT_DIR})

# Set the generic runtime output directory for multi generators like "msvc"
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${JSONPP_OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${JSONPP_OUTPUT_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${JSONPP_OUTPUT_DIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

message("Current output directory ${JSONPP_OUTPUT_DIR}")

# Set the build type (this only affects single output generators)
if(CMAKE_GENERATOR MATCHES "Unix Makefiles")
    if(NOT CMAKE_BUILD_TYPE)
        message(WARNING "CMAKE_BUILD_TYPE was not specified. The build will default to a 'release' build type. Note that this only affects single configuration generators like Make and Ninja.")
        set(CMAKE_BUILD_TYPE "release")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
    else()
        message(STATUS "CMAKE_BUILD_TYPE set to ${CMAKE_BUILD_TYPE}")
        if(CMAKE_BUILD_TYPE MATCHES "debug")
            set(CMAKE_BUILD_TYPE "debug")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
        elseif(CMAKE_BUILD_TYPE MATCHES "release")
            set(CMAKE_BUILD_TYPE "release")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
        else()
            message(FATAL_ERROR "${CMAKE_BUILD_TYPE} is not supported for this generator! The only accepted values are 'debug' or 'release'!")
        endif()
    endif()
endif()

message("Compiling with: ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS}")

include_directories(.)

# Build a library out of the sources
set(JSONPP_SOURCES "jsonpp.hpp" "jsonpp.cpp")
add_library(jsonpp STATIC ${JSONPP_SOURCES})

enable_testing()

# Build the test suite
set(JSONPP_TEST_SOURCES "jsonpp.hpp" "test/catch.hpp" "test/tests.cpp")
add_executable(testjsonpp ${JSONPP_TEST_SOURCES})
target_link_libraries(testjsonpp jsonpp)

add_test(jsonpp-test ${JSONPP_OUTPUT_DIR}/testjsonpp)

# Build the performance tests
message("Building the performance tests...")

set(JSONPP_PERF_SOURCES "jsonpp.hpp" "perf/perfjsonpp.cpp")
add_executable(perfjsonpp ${JSONPP_PERF_SOURCES})
target_link_libraries(perfjsonpp jsonpp)

set(JSONPP_PERF_FILES "${CMAKE_CURRENT_SOURCE_DIR}/perf/tiny.json" "${CMAKE_CURRENT_SOURCE_DIR}/perf/small.json" "${CMAKE_CURRENT_SOURCE_DIR}/perf/medium.json" "${CMAKE_CURRENT_SOURCE_DIR}/perf/large.json")

add_custom_command(
        TARGET perfjsonpp PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${JSONPP_PERF_FILES}
                ${JSONPP_OUTPUT_DIR})