cmake_minimum_required(VERSION 3.18.1)
project(GDE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(PROJECT_NAME GDE)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" )

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

set(IMGUI_PATH  "libs/imgui")
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp ${IMGUI_PATH}/backends/imgui_impl_sdl.cpp ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp ${IMGUI_PATH}/imgui_demo.cpp)
add_library("ImGui" STATIC ${IMGUI_SOURCES})

file(GLOB_RECURSE GDE_CPP_HEADERS engine/include/*.h*)
file(GLOB_RECURSE GDE_CPP_SOURCES engine/src/*.cpp )

set(CPP_HEADERS)
set(CPP_SOURCES)

add_library(sdl2 SHARED IMPORTED)
set_target_properties(sdl2 PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL/lib/libSDL2.dylib)

add_library(sdl2_image SHARED IMPORTED)
set_target_properties(sdl2_image PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL_image/lib/libSDL2_image.dylib)

add_library(sdl2_mixer SHARED IMPORTED)
set_target_properties(sdl2_mixer PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL_mixer/lib/libSDL2_mixer.dylib)

add_library(Freetype SHARED IMPORTED)
set_target_properties(Freetype PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libs/freetype/build/lib/libfreetype.dylib)

add_library(yamlcpp SHARED IMPORTED)
set_target_properties(yamlcpp PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libs/yamlcpp/libyaml-cpp.dylib)

add_library(box2d SHARED IMPORTED)
set_target_properties(box2d PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libs/box2d/build/bin/libbox2d.2.dylib)

find_package(Threads REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)

include_directories("/opt/X11/include")
SET(TARGET_LINKS sdl2 sdl2_image sdl2_mixer yamlcpp box2d Freetype ImGui Threads::Threads OpenGL::GL ${CMAKE_DL_LIBS})

include_directories(.)
include_directories(engine/include)
include_directories(libs)
include_directories(libs/SDL/include/SDL2)
include_directories(libs/freetype/build/include/freetype2)
include_directories(libs/imgui)
include_directories(libs/yamlcpp/include)
include_directories(libs/glm)
include_directories(libs/box2d/include)
include_directories(editor)

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

    add_custom_command(TARGET GDE POST_BUILD COMMAND bash -c "./postLibraryBuildingProcess.sh" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

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