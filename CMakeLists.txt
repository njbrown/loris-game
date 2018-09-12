#https://trenki2.github.io/blog/2017/06/02/using-sdl2-with-cmake/
cmake_minimum_required(VERSION 3.7)

project(lorisgame)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Game main.cpp)
target_link_libraries(Game ${SDL2_LIBRARIES})