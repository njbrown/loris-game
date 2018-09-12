#https://trenki2.github.io/blog/2017/06/02/using-sdl2-with-cmake/
cmake_minimum_required(VERSION 3.7)

project(lorisgame)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${lorisgame_SOURCE_DIR}/cmake")
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
#find_package(SDL2_ttf REQUIRED)


include_directories(${SDL2_INCLUDE_DIR}
                    ${SDL2_IMAGE_INCLUDE_DIR}
                    #${SDL2_TTF_INCLUDE_DIR}
					)

add_executable(Game main.cpp)

target_link_libraries(Game ${SDL2_LIBRARY}
                             ${SDL2_IMAGE_LIBRARIES}
                             #${SDL2_TTF_LIBRARIES}
							 )