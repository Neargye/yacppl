include_directories(${CMAKE_SOURCE_DIR}/include)

set(OPTIONS "")

if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    set(CMAKE_VERBOSE_MAKEFILE ON)
    set(OPTIONS -Wall -Wextra -pedantic-errors -Werror)
    set(OPTIONS ${OPTIONS} -std=c++11)

    if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") AND (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0.0))
        set(OPTIONS ${OPTIONS} -Wimplicit-fallthrough)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(OPTIONS ${OPTIONS} -Wimplicit-fallthrough)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(OPTIONS /W4 /WX)
    if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.11)
        set(OPTIONS ${OPTIONS} /permissive-)
    endif()
endif()

add_executable(unused_example
                    unused_example.cpp
                    ${CMAKE_SOURCE_DIR}/include/unused.hpp)
target_compile_options(unused_example PRIVATE ${OPTIONS})

add_executable(attributes_example
                    attributes_example.cpp
                    ${CMAKE_SOURCE_DIR}/include/attributes.hpp)
target_compile_options(attributes_example PRIVATE ${OPTIONS})

add_executable(nstd_example
                    nstd_example.cpp
                    ${CMAKE_SOURCE_DIR}/include/nstd.hpp
                    ${CMAKE_SOURCE_DIR}/include/unused.hpp)
target_compile_options(nstd_example PRIVATE ${OPTIONS})
