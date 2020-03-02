cmake_minimum_required (VERSION 2.8.11)
project (LGE)

set (CMAKE_VERBOSE_MAKEFILE ON)

find_package (PkgConfig)
find_package (OpenMP REQUIRED)

set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

include_directories (
    ${CMAKE_SOURCE_DIR}/src
    $ENV{BOOST_INC}
    $ENV{GTEST_INC}
)

link_libraries (
    $ENV{GTEST_MAIN_LIB}
    ${CMAKE_SOURCE_DIR}/src/parser/libbookshelfparser.a
)

file (GLOB SOURCES
    src/parser/*.h
    src/global/*.h    src/global/*.cpp
    src/cmdline/*.h   src/cmdline/*.cpp
    src/util/*.h      src/util/*.cpp
    src/db/*.h        src/db/*.cpp
)

file (GLOB PROJECT_SOURCES
    src/main/main.cpp
    ${SOURCES}
)

# Print out the used build type
message (STATUS "Using build type '${CMAKE_BUILD_TYPE}'")

set (CMAKE_CXX_FLAGS "-std=c++11 -Wall ${OpenMP_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -fno-inline")
set (CMAKE_CXX_FLAGS_RELEASE "-O3")
set (CMAKE_CXX_FLAGS_PROFILE "-O3 -pg")
set (CMAKE_EXE_LINKER_FLAGS "-pthread ${OpenMP_EXE_LINKER_FLAGS}")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS "-static")

