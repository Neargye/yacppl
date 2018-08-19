include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME}_example_unused
        ${CMAKE_SOURCE_DIR}/include/unused.hpp
        unused.cpp)
if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    target_compile_options(${PROJECT_NAME}_example_unused PUBLIC -Werror)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME}_example_unused PUBLIC /WX)
endif()

add_executable(${PROJECT_NAME}_example_attributes
        ${CMAKE_SOURCE_DIR}/include/attributes.hpp
        attributes.cpp)
if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    target_compile_options(${PROJECT_NAME}_example_attributes PUBLIC -Werror)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME}_example_attributes PUBLIC /WX)
endif()

add_executable(${PROJECT_NAME}_example_nstd
        ${CMAKE_SOURCE_DIR}/include/nstd.hpp
        ${CMAKE_SOURCE_DIR}/include/unused.hpp
        nstd.cpp)
if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    target_compile_options(${PROJECT_NAME}_example_nstd PUBLIC -Werror)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME}_example_nstd PUBLIC /WX)
endif()
