cmake_minimum_required(VERSION 3.15)
project(Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(PROJECT_NAME Engine)

add_definitions("-DGDE_EXPORT")
#add_definitions("-DRUN_TESTS")

SET(TARGET_LINKS Threads::Threads glad freetype sdl2 SDL2_mixer SDL2_image sdl2_net ${CMAKE_DL_LIBS}
        ImGui ImGuiPlot ImGuiTextEditor ImGuiNodeEditor ImGuiAddons
        EnTT::EnTT box2d libdecor yaml-cpp
        )

find_package(Threads REQUIRED)
find_package(Freetype REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(box2d REQUIRED)

add_library(libdecor SHARED IMPORTED )
set_target_properties(libdecor PROPERTIES IMPORTED_LOCATION /usr/local/lib/x86_64-linux-gnu/libdecor-0.so)

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


add_library(
        sdl2_net
        STATIC
        IMPORTED
)
set_target_properties(
        sdl2_net
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL_net/lib/libSDL2_net.a
)

#-------------- STB

set(STB_DIR "engine/src/stb")
include_directories(${STB_DIR})
link_directories(${STB_DIR})

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

file(GLOB_RECURSE GDE_CPP_HEADERS engine/include/*.h* sandbox/*.h*)
file(GLOB_RECURSE GDE_CPP_SOURCES engine/src/*.cpp sandbox/*.cpp)

set(CPP_HEADERS)
set(CPP_SOURCES)

#GDEFirebase

#add_library(Engine STATIC
#        ${SOURCES}
#        ${CPP_SOURCES}
#        ${CPP_HEADERS}
#        )

add_executable(Engine engine/main.cpp
    ${GDE_CPP_HEADERS}
    ${GDE_CPP_SOURCES}
    
    ${CPP_SOURCES}
    ${CPP_HEADERS}
)

target_link_libraries(Engine ${TARGET_LINKS})
target_compile_definitions(Engine PRIVATE ${GL_PROFILES})

