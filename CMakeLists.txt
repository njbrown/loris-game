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
                    "loris/include"
					)

add_subdirectory(loris)

set(SCRIPTS
	assets/game.ls)

add_executable(Game main.cpp ${SCRIPTS})

target_link_libraries(Game ${SDL2_LIBRARY}
                             ${SDL2_IMAGE_LIBRARIES}
                             #${SDL2_TTF_LIBRARIES}
                             loris
							 )

add_custom_command(
		TARGET Game POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
				${PROJECT_SOURCE_DIR}/assets
				${CMAKE_BINARY_DIR}/assets)