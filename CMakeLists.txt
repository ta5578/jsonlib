cmake_minimum_required (VERSION 3.5.1)
project(jsonlib)

set (CMAKE_CXX_STANDARD 14)

# Set additional compiler flags
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
endif()

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

