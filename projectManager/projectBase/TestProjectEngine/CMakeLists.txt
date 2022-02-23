cmake_minimum_required(VERSION 3.16)
project(PLACEHOLDER)

set(CMAKE_CXX_STANDARD 14)

SET(TARGET_LINKS Engine
        Threads::Threads glad freetype sdl2 SDL2_mixer sdl2_image sdl2_net ${CMAKE_DL_LIBS}
        imgui imPlot imNode imAddons
        )

set(GDE_PATH)

add_library(Engine STATIC IMPORTED code/include/TestLayer.h code/src/TestLayer.cpp)
set_target_properties(Engine PROPERTIES IMPORTED_LOCATION ${GDE_PATH}/libs_out/libEngine.a)

find_package(Threads REQUIRED)

add_library(sdl2 STATIC IMPORTED)
set_target_properties(sdl2 PROPERTIES IMPORTED_LOCATION ${GDE_PATH}/libs/SDL/lib/libSDL2.a)


add_library(sdl2_image STATIC IMPORTED)
set_target_properties(sdl2_image PROPERTIES IMPORTED_LOCATION ${GDE_PATH}/libs/SDL_image/lib/libSDL2_image.a)


add_library(sdl2_mixer STATIC IMPORTED)
set_target_properties(sdl2_mixer PROPERTIES IMPORTED_LOCATION ${GDE_PATH}/libs/SDL_mixer/lib/libSDL2_mixer.a)


add_library(sdl2_net STATIC IMPORTED)
set_target_properties(sdl2_net PROPERTIES IMPORTED_LOCATION ${GDE_PATH}/libs/SDL_net/lib/libSDL2_net.a)

add_library(imgui STATIC IMPORTED)
set_target_properties(imgui PROPERTIES IMPORTED_LOCATION ${GDE_PATH}/libs_out/libImGui.a)

add_library(imPlot STATIC IMPORTED)
set_target_properties(imPlot PROPERTIES IMPORTED_LOCATION ${GDE_PATH}/libs_out/libImGuiPlot.a)

add_library(imNode STATIC IMPORTED)
set_target_properties(imNode PROPERTIES IMPORTED_LOCATION ${GDE_PATH}/libs_out/libImGuiNodeEditor.a)

add_library(imAddons STATIC IMPORTED)
set_target_properties(imAddons PROPERTIES IMPORTED_LOCATION ${GDE_PATH}/libs_out/libImGuiAddons.a)

add_library(glad STATIC IMPORTED)
set_target_properties(glad PROPERTIES IMPORTED_LOCATION ${GDE_PATH}/libs_out/libglad.a)

include_directories(${GDE_PATH})
include_directories(${GDE_PATH}/engine/include)
include_directories(${GDE_PATH}/submodules/SDL/include)
include_directories(${GDE_PATH}/submodules/freetype/include)


include_directories(.)

add_executable(PLACEHOLDER main.cpp code/include/TestLayer.h code/src/TestLayer.cpp)

target_link_libraries(PLACEHOLDER ${TARGET_LINKS})
