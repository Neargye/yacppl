set(UNUSED_SOURCES unused.cpp ${CMAKE_SOURCE_DIR}/include/unused.hpp)
set(ATTRIBUTES_SOURCES attributes.cpp ${CMAKE_SOURCE_DIR}/include/attributes.hpp)
set(NSTD_SOURCES nstd.cpp ${CMAKE_SOURCE_DIR}/include/nstd.hpp ${CMAKE_SOURCE_DIR}/include/unused.hpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(OPTIONS "")

if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    set(OPTIONS -Wall -Wextra -pedantic-errors -Werror -std=c++11)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(OPTIONS /W4 /WX)
    if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.11)
        set(OPTIONS ${OPTIONS} /permissive-)
    endif()
endif()

add_executable(example_unused ${UNUSED_SOURCES})
target_compile_options(example_unused PRIVATE ${OPTIONS})

add_executable(example_attributes ${ATTRIBUTES_SOURCES})
target_compile_options(example_attributes PRIVATE ${OPTIONS})

add_executable(example_nstd ${NSTD_SOURCES})
target_compile_options(example_nstd PRIVATE ${OPTIONS})
