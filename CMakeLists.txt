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

add_executable(Game main.cpp)

target_link_libraries(Game ${SDL2_LIBRARY}
                             ${SDL2_IMAGE_LIBRARIES}
                             #${SDL2_TTF_LIBRARIES}
                             loris
							 )

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# COPY ASSETS TO OUTPUT FOLDER
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(DestDir ${CMAKE_BINARY_DIR})
else()
    set(DestDir ${CMAKE_BINARY_DIR})
endif()
add_custom_command(
		TARGET Game POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
				${PROJECT_SOURCE_DIR}/assets
				${DestDir}/assets)