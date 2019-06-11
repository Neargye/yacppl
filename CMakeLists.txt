cmake_minimum_required(VERSION 3.6)

project(yacppl LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOPLEVEL_PROJECT TRUE)
else()
    set(IS_TOPLEVEL_PROJECT FALSE)
endif()

option(YACPPL_OPT_BUILD_EXAMPLES "Build yaccpl examples" ${IS_TOPLEVEL_PROJECT})

if(YACPPL_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
