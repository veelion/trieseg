project(dddictseg)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS " -std=c++11 -fPIC -Wall -D_GNU_SOURCE")
set(CMAKE_CXX_FLAGS_DEBUG " -g ")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2 -DNDEBUG")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)
message("* Current build type is : ${CMAKE_BUILD_TYPE}")
IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    message("** define DDEBUG **")
    set(CMAKE_CXX_FLAGS_DEBUG " -g -DDEBUG ")
ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")

include_directories(/usr/include/python2.7 )

add_executable(segger_test src/test.cpp src/trie_seg.cc)
add_executable(test-basic src/test-basic.cc src/utf8.cc)
add_executable(test-wordfinder src/test-wordfinder.cc src/wordfinder.cc src/utf8.cc src/trie_seg.cc)
