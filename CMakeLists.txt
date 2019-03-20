cmake_minimum_required(VERSION 3.6)

project(yacppl LANGUAGES CXX)

option(YACPPL_OPT_BUILD_EXAMPLES "Build yaccpl examples" ON)

if(YACPPL_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
