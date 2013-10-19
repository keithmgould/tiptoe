cmake_minimum_required(VERSION 2.8)

project(tests)

include_directories(
  $ENV{GMOCK_HOME}/include 
  $ENV{GMOCK_HOME}/gtest/include 
  $ENV{EIGEN_HOME}
  $ENV{TIPTOE_HOME}/utilities/include
)

link_directories($ENV{GMOCK_HOME}/mybuild $ENV{GMOCK_HOME}/gtest/mybuild)

set(
    sources 
    main.cpp
    convert_test.cpp
    extract_test.cpp
    hamming_test.cpp
    $ENV{TIPTOE_HOME}/utilities/include/hamming.h
    transcoder_test.cpp
    unpacker_test.cpp
    untranscoder_test.cpp
   )

add_executable(suite ${sources})

target_link_libraries(suite gmock)
target_link_libraries(suite gtest)

