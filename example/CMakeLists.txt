include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME}_example_unused
        ${CMAKE_SOURCE_DIR}/include/unused.hpp
        unused.cpp)
if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    target_compile_options(${PROJECT_NAME}_example_unused PUBLIC -Werror)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME}_example_unused PUBLIC /WX)
endif()

add_executable(${PROJECT_NAME}_example_cpp_attributes
        ${CMAKE_SOURCE_DIR}/include/cpp_attributes.hpp
        cpp_attributes.cpp)
if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    target_compile_options(${PROJECT_NAME}_example_cpp_attributes PUBLIC -Werror)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME}_example_cpp_attributes PUBLIC /WX)
endif()
