cmake_minimum_required(VERSION 3.2)

include(GNUInstallDirs)

project(yacppl LANGUAGES CXX)
set(package_name ${PROJECT_NAME})
set(include_source_dir "${CMAKE_SOURCE_DIR}/include")

option(YACPPL_OPT_BUILD_EXAMPLES "Build yacppl examples" ON)

add_library(${package_name} INTERFACE)
target_include_directories(${package_name} INTERFACE "$<BUILD_INTERFACE:${include_source_dir}>")

if(YACPPL_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
