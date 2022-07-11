cmake_minimum_required(VERSION 3.18.1)
project(Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(PROJECT_NAME Engine)

add_definitions("-DGDE_EXPORT")
#add_definitions("-DRUN_TESTS")

SET(TARGET_LINKS Threads::Threads glad freetype sdl2 SDL2_mixer SDL2_image ${CMAKE_DL_LIBS}
        ImGui ImGuiPlot ImGuiTextEditor ImGuiNodeEditor ImGuiAddons
        EnTT::EnTT box2d yaml-cpp
        )

find_package(Threads REQUIRED)
find_package(Freetype REQUIRED)
find_package(box2d REQUIRED)

execute_process(COMMAND "echo $XDG_SESSION_TYPE" OUTPUT_VARIABLE result_display_server)
if (NOT ${resulting_display_server} STREQUAL "x11")
    message("Wayland detected, linking to LibDecor")
    add_library(libdecor SHARED IMPORTED )
    set_target_properties(libdecor PROPERTIES IMPORTED_LOCATION /usr/local/lib/x86_64-linux-gnu/libdecor-0.so)
    LIST(APPEND TARGET_LINKS libdecor)
else()
    message("x11 detected, skipping link to LibDecor")
endif()

add_library(
        sdl2
        STATIC
        IMPORTED
)
set_target_properties(
        sdl2
        PROPERTIES
        IMPORTED_LOCATION
        /usr/local/lib/libSDL2.a
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

add_compile_options("-Werror")

include_directories(.)
include_directories(engine/include)
include_directories(sandbox)

include_directories(submodules/freetype/include)
include_directories(submodules/SDL/include)
include_directories(submodules/SDL_net)
include_directories(submodules/SDL_mixer/include)
include_directories(submodules/SDL_image)
include_directories(submodules/glm)
include_directories(submodules/imgui)
include_directories(submodules/entt/src)
include_directories(submodules/box2d/include)
include_directories(submodules/yaml-cpp/include)
add_subdirectory(submodules/entt)

add_library(glad libs/glad/include/glad/glad.h libs/glad/src/glad.c)
target_include_directories(glad PUBLIC libs/glad/include/)

set(IMGUI_PATH  "submodules/imgui")
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp ${IMGUI_PATH}/backends/imgui_impl_sdl.cpp ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp ${IMGUI_PATH}/imgui_demo.cpp)
add_library("ImGui" STATIC ${IMGUI_SOURCES})
target_include_directories("ImGui" PUBLIC ${IMGUI_PATH})

set(IMGUI_PLOT_PATH  "submodules/implot")
file(GLOB IMGUI_PLOT_SOURCES ${IMGUI_PLOT_PATH}/*.cpp)
add_library("ImGuiPlot" STATIC ${IMGUI_PLOT_SOURCES})
target_include_directories("ImGuiPlot" PUBLIC ${IMGUI_PLOT_PATH})

set(IMGUI_ADDONS_PATH  "submodules/imguiAddons")
file(GLOB IMGUI_ADDONS_SOURCES ${IMGUI_ADDONS_PATH}/FileBrowser/*.cpp)
add_library("ImGuiAddons" STATIC ${IMGUI_ADDONS_SOURCES})
target_include_directories("ImGuiAddons" PUBLIC ${IMGUI_ADDONS_PATH})

set(IMGUI_TEXT_EDITOR_PATH  "submodules/imguiTextEditor")
file(GLOB IMGUI_TEXT_EDITOR_SOURCES ${IMGUI_TEXT_EDITOR_PATH}/*.cpp)
add_library("ImGuiTextEditor" STATIC ${IMGUI_TEXT_EDITOR_SOURCES})
target_include_directories("ImGuiTextEditor" PUBLIC ${IMGUI_TEXT_EDITOR_PATH})

set(IMGUI_NODE_EDITOR_PATH  "submodules/imguiNodeEditor")
file(GLOB IMGUI_NODE_EDITOR_SOURCES ${IMGUI_NODE_EDITOR_PATH}/*.cpp)
add_library("ImGuiNodeEditor" STATIC ${IMGUI_NODE_EDITOR_SOURCES})
target_include_directories("ImGuiNodeEditor" PUBLIC ${IMGUI_NODE_EDITOR_PATH})

set(GL_PROFILES "GL_PROFILE_GL3")

file(GLOB_RECURSE GDE_CPP_HEADERS engine/include/*.h*)
file(GLOB_RECURSE GDE_CPP_SOURCES engine/src/*.cpp)

set(CPP_HEADERS)
set(CPP_SOURCES)

#GDEFirebase

option(GDE_LIBRARY "Build library instead of executable" OFF)
if(GDE_LIBRARY)
    message("Building lib")
    add_library(Engine STATIC
            ${GDE_CPP_HEADERS}
            ${GDE_CPP_SOURCES}

            ${CPP_SOURCES}
            ${CPP_HEADERS}
            )
else()
    message("Building executable")
    add_executable(Engine engine/main.cpp sandbox/Sandbox.h sandbox/Sandbox.cpp

            ${GDE_CPP_HEADERS}
            ${GDE_CPP_SOURCES}

            ${CPP_SOURCES}
            ${CPP_HEADERS}
            )
endif()
unset(GDE_LIBRARY CACHE)

target_link_libraries(Engine ${TARGET_LINKS})
target_compile_definitions(Engine PRIVATE ${GL_PROFILES})

