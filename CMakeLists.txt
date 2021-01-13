cmake_minimum_required(VERSION 3.16.0)

project(SplitRectangle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Catch2 CONFIG REQUIRED)

add_compile_options(-Wall -Wextra)

include_directories(include)
add_subdirectory(tests)
