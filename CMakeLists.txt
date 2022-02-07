cmake_minimum_required(VERSION 3.15)
project(Engine2_0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(PROJECT_NAME Engine2_0)

#${CMAKE_DL_LIBS} is needed as some part of opengl uses it
SET(TARGET_LINKS glad freetype SDL2::Main SDL2::Mixer SDL2::Net ${CMAKE_DL_LIBS}
        # flecs
        )

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include_directories(.)
include_directories(${CMAKE_SOURCE_DIR}/external})
include_directories (/usr/include/freetype2)

#-------------- IMGUI

#set(IMGUI_DIR "external/imgui")
#file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp)
#add_library("ImGui" STATIC ${IMGUI_SOURCES})
#target_include_directories("ImGui" PUBLIC ${IMGUI_DIR})

#-------------- STB

set(STB_DIR "external/stb")
include_directories(${STB_DIR})
link_directories(${STB_DIR})

#-------------- GLFW3 AND Threads

find_package(Freetype REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_net REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_library(glad external/glad/include/glad/glad.h external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include/)

#-------------- FLECS

#find_package(flecs REQUIRED)
#if(flecs_FOUND)
#    message("-- Found Flecs")
#else()
#    message("-- To make flecs work download source code, cmake ., make, sudo make install")
#endif()

add_executable( Engine2_0 main.cpp

        core/util/Easing.h core/util/Functions.h core/util/Logger.h core/util/PerlinNoise.h
        core/util/Random.h core/util/Util.h core/util/Vec.h core/util/Color.cpp core/util/Color.h core/util/Delta.h
        core/util/Clock.h core/util/Clock.cpp

        core/render/window/event/Event.h core/render/window/event/KeyEvent.h core/render/window/event/MouseEvent.h
        core/render/window/event/WindowEvent.h core/render/window/Window.h core/render/window/Window.cpp

        core/render/layers/Layer.h core/render/layers/LayerStack.h core/render/layers/LayerStack.cpp
        core/render/layers/ImGuiLayer.h core/render/layers/ImGuiLayer.cpp

        core/render/window/keysAndButtons/GamePadKeys.h core/render/window/input/Input.cpp core/render/window/input/Input.h
        core/render/window/keysAndButtons/KeyboardKeys.h core/render/window/keysAndButtons/MouseKeys.h

        core/render/elements/Texture.h core/render/elements/Texture.cpp core/render/elements/SpriteBatch.h
        core/render/elements/SpriteBatch.cpp core/render/elements/Shader.h core/render/elements/Shader.cpp

        core/Engine.cpp core/Engine.h core/Core.h

        external/stb/stb_image.h external/stb/stb_image.cpp external/stb/stb_image_writer.h external/json.h

        core/render/Renderer.cpp core/render/Renderer.h core/systems/eventSystem/EventBus.h

        core/systems/Components.h core/render/elements/Sprite.cpp core/render/elements/Sprite.h core/render/Camera.cpp core/render/Camera.h core/render/shapes/Shape.h

        external/imgui/imgui.h external/imgui/imgui.cpp external/imgui/imstb_truetype.h external/imgui/imstb_rectpack.h
        external/imgui/imstb_textedit.h external/imgui/imgui_internal.h external/imgui/imgui_tables.cpp
        external/imgui/imgui_draw.cpp external/imgui/imconfig.h external/imgui/backends/imgui_impl_sdl.h external/imgui/imgui_widgets.cpp
        external/imgui/backends/imgui_impl_sdl.cpp external/imgui/backends/imgui_impl_opengl3.h
        external/imgui/backends/imgui_impl_opengl3.cpp external/imgui/imgui_demo.cpp

        # https://github.com/epezent/implot
        external/imgui/implot/implot.h external/imgui/implot/implot.cpp external/imgui/implot/implot_internal.h external/imgui/implot/implot_items.cpp

        # https://github.com/thedmd/imgui-node-editor
        external/imgui/nodeEditor/crude_json.h external/imgui/nodeEditor/crude_json.cpp external/imgui/nodeEditor/imgui_bezier_math.h
        external/imgui/nodeEditor/imgui_canvas.h external/imgui/nodeEditor/imgui_canvas.cpp external/imgui/nodeEditor/imgui_extra_math.h
        external/imgui/nodeEditor/imgui_node_editor.h external/imgui/nodeEditor/imgui_node_editor.cpp external/imgui/nodeEditor/imgui_node_editor_api.cpp
        external/imgui/nodeEditor/imgui_node_editor_internal.h external/imgui/textEditor/TextEditor.h external/imgui/textEditor/TextEditor.cpp

        core/systems/profiling/Profiler.h core/systems/profiling/Profiler.cpp core/render/layers/ImGuiCore.h
        core/render/elements/TextureAtlasManager.cpp core/render/elements/TextureAtlasManager.h core/util/Rect.h
        core/systems/animationSystem/Animation.cpp core/systems/animationSystem/Animation.h core/systems/animationSystem/AnimationSystem.cpp
        core/systems/animationSystem/AnimationSystem.h


        core/systems/console/Console.cpp core/systems/console/Console.h core/systems/uiSystem/Text.cpp core/systems/uiSystem/Text.h
        core/systems/uiSystem/FontManager.cpp core/systems/uiSystem/FontManager.h external/stb/stb_image_writer.h core/render/elements/FrameBuffer.cpp
        core/render/elements/FrameBuffer.h core/render/elements/ShaderManager.cpp core/render/elements/ShaderManager.h core/render/elements/IRenderizable.h
        sandbox/Sandbox.cpp sandbox/Sandbox.h core/systems/soundSystem/SoundManager.cpp core/systems/soundSystem/SoundManager.h
        core/render/window/event/JoystickEvent.h core/render/window/input/WindowInput.cpp core/render/window/input/WindowInput.h core/render/window/input/KeyboardInput.cpp core/render/window/input/KeyboardInput.h core/render/window/input/MouseInput.cpp core/render/window/input/MouseInput.h core/render/window/input/ControllerInput.cpp core/render/window/input/ControllerInput.h)

target_link_libraries(Engine2_0 ${TARGET_LINKS})

