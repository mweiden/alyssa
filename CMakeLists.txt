cmake_minimum_required(VERSION 3.7)
project(practice)

include(DownloadProject.cmake)
download_project(
        PROJ googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG master
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})


set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES interpreter.h interpreter.cpp environment.h)

add_executable(tests test.cpp ${SOURCE_FILES})

add_executable(repl repl.cpp ${SOURCE_FILES})

target_link_libraries(tests gtest gmock_main)
add_test(NAME example_test COMMAND tests)
