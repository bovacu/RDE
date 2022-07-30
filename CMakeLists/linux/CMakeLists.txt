cmake_minimum_required(VERSION 3.18.1)
project(GDE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(PROJECT_NAME GDE)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_definitions("-DGDE_EXPORT")
add_compile_options("-Werror")

execute_process(COMMAND "echo $XDG_SESSION_TYPE" OUTPUT_VARIABLE result_display_server)
if (NOT ${resulting_display_server} STREQUAL "x11")
    message("Wayland detected, linking to LibDecor")
    add_library(libdecor SHARED IMPORTED )
    set_target_properties(libdecor PROPERTIES IMPORTED_LOCATION /usr/local/lib/x86_64-linux-gnu/libdecor-0.so)
    LIST(APPEND TARGET_LINKS libdecor)
else()
    message("x11 detected, skipping link to LibDecor")
endif()

set(GL_PROFILES "GL_PROFILE_GL3")

set(IMGUI_PATH  "submodules/imgui")
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp ${IMGUI_PATH}/backends/imgui_impl_sdl.cpp ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp ${IMGUI_PATH}/imgui_demo.cpp)

set(BOX2D_PATH  "submodules/box2d")
file(GLOB BOX2D_SOURCES ${BOX2D_PATH}/src/*/*.cpp)

set(YAMLCPP_PATH  "submodules/yaml-cpp")
file(GLOB YAMLCPP_SOURCES ${YAMLCPP_PATH}/src/*/*.cpp ${YAMLCPP_PATH}/src/*.cpp)

file(GLOB_RECURSE GDE_CPP_HEADERS engine/include/*.h* submodules/glad/include/glad/glad.h)
file(GLOB_RECURSE GDE_CPP_SOURCES engine/src/*.cpp submodules/glad/src/glad.c ${IMGUI_SOURCES} ${BOX2D_SOURCES} ${YAMLCPP_SOURCES})

set(CPP_HEADERS)
set(CPP_SOURCES)

add_library(
        sdl2
        STATIC
        IMPORTED
)
set_target_properties(
        sdl2
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL/lib/libSDL2.a
)

add_library(
        sdl2_image
        STATIC
        IMPORTED
)
set_target_properties(
        sdl2_image
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL_image/lib/libSDL2_image.a
)

add_library(
        sdl2_mixer
        STATIC
        IMPORTED
)
set_target_properties(
        sdl2_mixer
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL_mixer/lib/libSDL2_mixer.a
)

add_library(
        Freetype
        STATIC
        IMPORTED
)
set_target_properties(
        Freetype
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/freetype/lib/libfreetype.a
)

add_library(
        png
        STATIC
        IMPORTED
)
set_target_properties(
        png
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libpng/lib/libpng.a
)

SET(TARGET_LINKS sdl2 sdl2_image sdl2_mixer png freetype)

find_package(Freetype REQUIRED)
message("${FREETYPE_LIBRARIES}")

include_directories(.)
include_directories(engine/include)
include_directories(editor)

include_directories(libs/freetype/include/freetype2)
include_directories(libs/SDL/include/SDL2)
include_directories(libs/SDL_mixer/include/SDL2)
include_directories(libs/SDL_image/include/SDL2)

include_directories(submodules)
include_directories(submodules/glm)
include_directories(submodules/entt/single_include)
include_directories(submodules/box2d/include)
include_directories(submodules/yaml-cpp/include)
include_directories(submodules/glad/include)
include_directories(submodules/imgui)

#GDEFirebase

option(GDE_LIBRARY "Build library instead of executable" OFF)
if(GDE_LIBRARY)
    message("Building lib")
    add_library(GDE STATIC
            ${GDE_CPP_HEADERS}
            ${GDE_CPP_SOURCES}

            ${CPP_SOURCES}
            ${CPP_HEADERS}
            )
    target_link_libraries(GDE ${TARGET_LINKS})
    target_compile_definitions(GDE PRIVATE ${GL_PROFILES})

    execute_process(COMMAND bash -c "cd ${CMAKE_CURRENT_SOURCE_DIR}/output/lib && ar -x ../../libs/SDL/lib/libSDL2.a && ar -x ../../libs/SDL_image/lib/libSDL2_image.a && ar -x ../../libs/SDL_mixer/lib/libSDL2_mixer.a && ar -x libGDE.a && rm libGDE.a && ar -qc libGDE.a *.o && rm *.o" OUTPUT_VARIABLE merge_libs)

else()
    message("Building executable")
    add_executable(GDE engine/main.cpp editor/Editor.cpp

            ${GDE_CPP_HEADERS}
            ${GDE_CPP_SOURCES}

            ${CPP_SOURCES}
            ${CPP_HEADERS}
            )
    target_link_libraries(GDE ${TARGET_LINKS})
    target_compile_definitions(GDE PRIVATE ${GL_PROFILES})
endif()
unset(GDE_LIBRARY CACHE)