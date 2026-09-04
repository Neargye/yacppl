cmake_minimum_required(VERSION 3.25)

project(yacppl LANGUAGES CXX)

option(YACPPL_OPT_BUILD_EXAMPLES "Build yacppl examples" ${PROJECT_IS_TOP_LEVEL})
option(YACPPL_OPT_BUILD_TESTS "Build and perform yacppl tests" ${PROJECT_IS_TOP_LEVEL})

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

if(YACPPL_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(YACPPL_OPT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
