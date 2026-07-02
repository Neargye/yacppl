cmake_minimum_required(VERSION 3.25)

project(yacppl LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOPLEVEL_PROJECT TRUE)
else()
    set(IS_TOPLEVEL_PROJECT FALSE)
endif()

option(YACPPL_OPT_BUILD_EXAMPLES "Build yacppl examples" ${IS_TOPLEVEL_PROJECT})
option(YACPPL_OPT_BUILD_TESTS "Build and perform yacppl tests" ${IS_TOPLEVEL_PROJECT})

if(YACPPL_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(YACPPL_OPT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
