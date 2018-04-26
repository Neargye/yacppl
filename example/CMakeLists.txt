include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCE_EXAMPLE_UNUSED
    ${CMAKE_SOURCE_DIR}/include/unused.hpp
    unused.cpp)

add_executable(${PROJECT_NAME}_example_unused ${SOURCE_EXAMPLE_UNUSED})

if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    target_compile_options(${PROJECT_NAME}_example_unused PUBLIC -Werror)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME}_example_unused PUBLIC /WX)
endif()

set(SOURCE_EXAMPLE_CPP_ATTRIBUTES
    ${CMAKE_SOURCE_DIR}/include/cpp_attributes.hpp
    cpp_attributes.cpp)

add_executable(${PROJECT_NAME}_example_cpp_attributes ${SOURCE_EXAMPLE_CPP_ATTRIBUTES})

if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    target_compile_options(${PROJECT_NAME}_example_cpp_attributes PUBLIC -Werror)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME}_example_cpp_attributes PUBLIC /WX)
endif()
