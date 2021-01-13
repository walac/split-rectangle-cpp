cmake_minimum_required(VERSION 3.16.0)

project(SplitRectangle)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Catch2 CONFIG REQUIRED)

add_compile_options(-Wall -Wextra)

add_executable(test test.cc test_rect.cc test_split.cc)
target_link_libraries(test PRIVATE Catch2::Catch2)

add_executable(fuzzy_test fuzzy_test.cc)
target_link_libraries(fuzzy_test PRIVATE)
