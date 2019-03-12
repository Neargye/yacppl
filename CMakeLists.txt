cmake_minimum_required(VERSION 3.6)

include(GNUInstallDirs)

project(yal VERSION "0.3.0" LANGUAGES CXX)

option(YACPPL_OPT_BUILD_EXAMPLES "Build yaccpl examples" ON)

if(YACPPL_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
