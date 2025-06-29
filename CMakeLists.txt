cmake_minimum_required(VERSION 3.10)

project(complete-template)

add_executable(${PROJECT_NAME} src/main.cpp src/sdl_starter.cpp)

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)
pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)
pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)

target_link_libraries(${PROJECT_NAME} PRIVATE
	${SDL2_LIBRARIES}
	${SDL2_IMAGE_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
	${SDL2_TTF_LIBRARIES}
	SDL2main
)

if(PLATFORM_PSP)
	create_pbp_file(
		TARGET ${PROJECT_NAME}
		ICON_PATH NULL
		BACKGROUND_PATH NULL
		PREVIEW_PATH NULL
		TITLE ${PROJECT_NAME}
	)
elseif(VITA)
	set(VITA_TITLEID "INPUT0001") #title length == 9

	include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

	vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
	vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
		VERSION "01.00"
		NAME ${PROJECT_NAME}
		FILE platform/vita/sce_sys sce_sys
		FILE res/fonts/square_sans_serif_7.ttf square_sans_serif_7.ttf
		FILE res/sounds/laser.wav laser.wav
		FILE res/music/music.wav music.wav
		FILE res/sprites/alien_1.png alien_1.png 
	)
endif()

file(COPY res/fonts/square_sans_serif_7.ttf DESTINATION ${CMAKE_BINARY_DIR})
file(COPY res/sounds/laser.wav DESTINATION ${CMAKE_BINARY_DIR})
file(COPY res/music/music.wav DESTINATION ${CMAKE_BINARY_DIR})
file(COPY res/sprites/alien_1.png DESTINATION ${CMAKE_BINARY_DIR})

