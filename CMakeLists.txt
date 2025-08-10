cmake_minimum_required(VERSION 3.14)

project(alyssa VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: prevent overriding parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(alyssa_lib
  interpreter.cpp
  interpreter.h
  environment.h
)
target_include_directories(alyssa_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(repl repl.cpp)
target_link_libraries(repl PRIVATE alyssa_lib)

add_executable(tests test.cpp)
target_link_libraries(tests PRIVATE alyssa_lib GTest::gmock_main GTest::gtest)

enable_testing()
add_test(NAME all_tests COMMAND tests)
