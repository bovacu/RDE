cmake_minimum_required(VERSION 3.15)
project(Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(PROJECT_NAME Engine)

add_definitions("-DGDE_EXPORT")
#add_definitions("-DRUN_TESTS")


#${CMAKE_DL_LIBS} is needed as some part of opengl in ImGui uses it
SET(TARGET_LINKS Threads::Threads glad freetype sdl2 SDL2_mixer SDL2_image sdl2_net ${CMAKE_DL_LIBS}
        ImGui ImGuiPlot ImGuiTextEditor ImGuiNodeEditor ImGuiAddons
        EnTT::EnTT box2d
        )

find_package(Threads REQUIRED)
find_package(Freetype REQUIRED)

add_library(
        sdl2
        STATIC
        IMPORTED
)
set_target_properties(
        sdl2
        PROPERTIES
        IMPORTED_LOCATION
        /usr/local/Cellar/sdl2/2.0.20/lib/libSDL2-2.0.0.dylib
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
        /usr/local/Cellar/sdl2_image/2.0.5/lib/libSDL2_image-2.0.0.dylib
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
        /usr/local/Cellar/sdl2_mixer/2.0.4_3/lib/libSDL2_mixer-2.0.0.dylib
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
        /usr/local/Cellar/sdl2_net/2.0.1/lib/libSDL2_net-2.0.0.dylib
)


add_library(
        box2d
        STATIC
        IMPORTED
)
set_target_properties(
        box2d
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/submodules/box2d/build/bin/libbox2d.a
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

set(SOURCES
        engine/include/GDE.h

        engine/include/core/util/Easing.h engine/include/core/util/Functions.h engine/include/core/util/Logger.h engine/include/core/util/PerlinNoise.h
        engine/include/core/util/Random.h engine/include/core/util/Util.h engine/include/core/util/Vec.h engine/src/core/util/Color.cpp engine/include/core/util/Color.h engine/include/core/util/Delta.h
        engine/include/core/util/Clock.h engine/src/core/util/Clock.cpp

        engine/include/core/render/window/event/Event.h engine/include/core/render/window/event/KeyEvent.h engine/include/core/render/window/event/MouseEvent.h
        engine/include/core/render/window/event/WindowEvent.h engine/include/core/render/window/Window.h engine/src/core/render/window/Window.cpp

        engine/include/core/graph/Scene.h engine/src/core/graph/Scene.cpp
        engine/include/core/graph/ImGuiScene.h engine/src/core/graph/ImGuiScene.cpp

        engine/include/core/render/window/keysAndButtons/GamePadButtons.h engine/src/core/render/window/input/Input.cpp engine/include/core/render/window/input/Input.h
        engine/include/core/render/window/keysAndButtons/KeyboardKeys.h engine/include/core/render/window/keysAndButtons/MouseKeys.h

        engine/include/core/render/elements/Texture.h engine/src/core/render/elements/Texture.cpp engine/include/core/render/elements/SpriteBatch.h
        engine/src/core/render/elements/SpriteBatch.cpp engine/include/core/render/elements/Shader.h engine/src/core/render/elements/Shader.cpp

        engine/src/core/Engine.cpp engine/include/core/Engine.h engine/include/core/Core.h

        engine/include/stb/stb_image.h engine/src/stb/stb_image.cpp engine/include/stb/stb_image_writer.h

        libs/json.h

        engine/src/core/render/Renderer.cpp engine/include/core/render/Renderer.h engine/include/core/systems/eventSystem/EventBus.h

        engine/include/core/graph/Components.h engine/src/core/render/Camera.cpp engine/include/core/render/Camera.h engine/include/core/render/shapes/Shape.h

        engine/include/core/systems/profiling/Profiler.h engine/src/core/systems/profiling/Profiler.cpp engine/include/core/graph/ImGuiCore.h
        engine/src/core/render/elements/TextureAtlasManager.cpp engine/include/core/render/elements/TextureAtlasManager.h engine/include/core/util/Rect.h
        engine/src/core/systems/animationSystem/Animation.cpp engine/include/core/systems/animationSystem/Animation.h engine/src/core/systems/animationSystem/AnimationSystem.cpp
        engine/include/core/systems/animationSystem/AnimationSystem.h


        engine/src/core/systems/console/Console.cpp engine/include/core/systems/console/Console.h
        engine/src/core/systems/uiSystem/FontManager.cpp engine/include/core/systems/uiSystem/FontManager.h engine/include/stb/stb_image_writer.h engine/src/core/render/elements/FrameBuffer.cpp
        engine/include/core/render/elements/FrameBuffer.h engine/src/core/render/elements/ShaderManager.cpp engine/include/core/render/elements/ShaderManager.h
        sandbox/Sandbox.cpp sandbox/Sandbox.h engine/src/core/systems/soundSystem/SoundManager.cpp engine/include/core/systems/soundSystem/SoundManager.h
        engine/include/core/render/window/event/JoystickEvent.h engine/src/core/render/window/input/WindowInput.cpp engine/include/core/render/window/input/WindowInput.h
        engine/src/core/render/window/input/KeyboardInput.cpp engine/include/core/render/window/input/KeyboardInput.h engine/src/core/render/window/input/MouseInput.cpp
        engine/include/core/render/window/input/MouseInput.h engine/src/core/render/window/input/ControllerInput.cpp engine/include/core/render/window/input/ControllerInput.h
        engine/src/core/platform/Platform.cpp engine/include/core/platform/Platform.h engine/include/core/platform/PlatformHeaderSDL.h
        engine/include/core/platform/PlatformHeaderSDLMixer.h engine/include/core/platform/PlatformHeaderSDLImage.h
        engine/include/core/render/window/input/MobileInput.h engine/src/core/render/window/input/MobileInput.cpp engine/include/core/render/window/event/MobileEvent.h
        engine/include/core/procedural/CellularAutomataMapGenerator.h engine/src/core/procedural/CellularAutomataMapGenerator.cpp
        engine/include/core/systems/fileSystem/FilesSystem.h engine/src/core/systems/fileSystem/FilesSystem.cpp engine/include/core/util/StringUtils.h
        engine/src/core/render/ViewPort.cpp engine/include/core/render/elements/ViewPort.h
        engine/include/core/util/Delegate.h engine/include/core/graph/Graph.h engine/src/core/graph/Graph.cpp
        engine/src/core/graph/Components.cpp engine/include/core/graph/Components.h
        engine/include/core/graph/Node.h engine/src/core/graph/Node.cpp

        unitTesting/HierarchyTest.cpp unitTesting/doctest.h
        engine/include/core/systems/particleSystem/ParticleSystem.h engine/src/core/systems/particleSystem/ParticleSystem.cpp)

set(CPP_HEADERS)
set(CPP_SOURCES)

if(APPLE)
   include_directories("/opt/X11/include")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" )

   find_package(Freetype REQUIRED)

   SET(TARGET_LINKS Threads::Threads glad ${FREETYPE_LIBRARIES} sdl2 sdl2_mixer sdl2_image sdl2_net ${CMAKE_DL_LIBS}
        ImGui ImGuiPlot ImGuiTextEditor ImGuiNodeEditor ImGuiAddons
        EnTT::EnTT box2d
        )
else()
        SET(TARGET_LINKS Threads::Threads glad freetype sdl2 sdl2_mixer sdl2_image sdl2_net ${CMAKE_DL_LIBS}
        ImGui ImGuiPlot ImGuiTextEditor ImGuiNodeEditor ImGuiAddons
        EnTT::EnTT box2d
        )
endif()



#GDEFirebase

#add_library(Engine STATIC
#        ${SOURCES}
#        ${CPP_SOURCES}
#        ${CPP_HEADERS}
#        )

add_executable(Engine engine/main.cpp
        ${SOURCES}
        ${CPP_SOURCES}
        ${CPP_HEADERS}
        )

target_link_libraries(Engine ${TARGET_LINKS})
target_compile_definitions(Engine PRIVATE ${GL_PROFILES})

