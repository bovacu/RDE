cmake_minimum_required(VERSION 3.15)
project(Engine2_0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(PROJECT_NAME Engine2_0)

#${CMAKE_DL_LIBS} is needed as some part of opengl uses it
SET(TARGET_LINKS glad freetype SDL2 SDL2_mixer SDL2_net SDL2_image ${CMAKE_DL_LIBS}
        # flecs
        )

list(APPEND CMAKE_MODULE_PATH libs/SDL libs/SDL_image libs/SDL_mixer libs/SDL_net libs/freetype)

#-------------- STB

set(STB_DIR "engine/src/stb")
include_directories(${STB_DIR})
link_directories(${STB_DIR})


include_directories(.)
include_directories(engine/include)
include_directories(sandbox)
include_directories(libs/freetype/include)
include_directories(libs/SDL/include)
include_directories(libs/SDL_net)
include_directories(libs/SDL_mixer/include)
include_directories(libs/SDL_image)
include_directories(libs/glm)

add_library(glad libs/glad/include/glad/glad.h libs/glad/src/glad.c)
target_include_directories(glad PUBLIC libs/glad/include/)

set(GL_PROFILES "GL_PROFILE_GL3")

#-------------- FLECS

#find_package(flecs REQUIRED)
#if(flecs_FOUND)
#    message("-- Found Flecs")
#else()
#    message("-- To make flecs work download source code, cmake ., make, sudo make install")
#endif()

set(SOURCES
        engine/include/core/util/Easing.h engine/include/core/util/Functions.h engine/include/core/util/Logger.h engine/include/core/util/PerlinNoise.h
        engine/include/core/util/Random.h engine/include/core/util/Util.h engine/include/core/util/Vec.h engine/src/core/util/Color.cpp engine/include/core/util/Color.h engine/include/core/util/Delta.h
        engine/include/core/util/Clock.h engine/src/core/util/Clock.cpp

        engine/include/core/render/window/event/Event.h engine/include/core/render/window/event/KeyEvent.h engine/include/core/render/window/event/MouseEvent.h
        engine/include/core/render/window/event/WindowEvent.h engine/include/core/render/window/Window.h engine/src/core/render/window/Window.cpp

        engine/include/core/render/layers/Layer.h engine/include/core/render/layers/LayerStack.h engine/src/core/render/layers/LayerStack.cpp
        engine/include/core/render/layers/ImGuiLayer.h engine/src/core/render/layers/ImGuiLayer.cpp

        engine/include/core/render/window/keysAndButtons/GamePadButtons.h engine/src/core/render/window/input/Input.cpp engine/include/core/render/window/input/Input.h
        engine/include/core/render/window/keysAndButtons/KeyboardKeys.h engine/include/core/render/window/keysAndButtons/MouseKeys.h

        engine/include/core/render/elements/Texture.h engine/src/core/render/elements/Texture.cpp engine/include/core/render/elements/SpriteBatch.h
        engine/src/core/render/elements/SpriteBatch.cpp engine/include/core/render/elements/Shader.h engine/src/core/render/elements/Shader.cpp

        engine/src/core/Engine.cpp engine/include/core/Engine.h engine/include/core/Core.h

        engine/include/stb/stb_image.h engine/src/stb/stb_image.cpp engine/include/stb/stb_image_writer.h

        libs/json.h

        engine/src/core/render/Renderer.cpp engine/include/core/render/Renderer.h engine/include/core/systems/eventSystem/EventBus.h

        engine/include/core/systems/Components.h engine/src/core/render/elements/Sprite.cpp engine/include/core/render/elements/Sprite.h engine/src/core/render/Camera.cpp engine/include/core/render/Camera.h engine/include/core/render/shapes/Shape.h

        libs/imgui/imgui.h libs/imgui/imgui.cpp libs/imgui/imstb_truetype.h libs/imgui/imstb_rectpack.h
        libs/imgui/imstb_textedit.h libs/imgui/imgui_internal.h libs/imgui/imgui_tables.cpp
        libs/imgui/imgui_draw.cpp libs/imgui/imconfig.h libs/imgui/backends/imgui_impl_sdl.h libs/imgui/imgui_widgets.cpp
        libs/imgui/backends/imgui_impl_sdl.cpp libs/imgui/backends/imgui_impl_opengl3.h
        libs/imgui/backends/imgui_impl_opengl3.cpp libs/imgui/imgui_demo.cpp

        # https://github.com/epezent/implot
        libs/imgui/implot/implot.h libs/imgui/implot/implot.cpp libs/imgui/implot/implot_internal.h libs/imgui/implot/implot_items.cpp

        # https://github.com/thedmd/imgui-node-editor
        libs/imgui/nodeEditor/crude_json.h libs/imgui/nodeEditor/crude_json.cpp libs/imgui/nodeEditor/imgui_bezier_math.h
        libs/imgui/nodeEditor/imgui_canvas.h libs/imgui/nodeEditor/imgui_canvas.cpp libs/imgui/nodeEditor/imgui_extra_math.h
        libs/imgui/nodeEditor/imgui_node_editor.h libs/imgui/nodeEditor/imgui_node_editor.cpp libs/imgui/nodeEditor/imgui_node_editor_api.cpp
        libs/imgui/nodeEditor/imgui_node_editor_internal.h libs/imgui/textEditor/TextEditor.h libs/imgui/textEditor/TextEditor.cpp

        engine/include/core/systems/profiling/Profiler.h engine/src/core/systems/profiling/Profiler.cpp engine/include/core/render/layers/ImGuiCore.h
        engine/src/core/render/elements/TextureAtlasManager.cpp engine/include/core/render/elements/TextureAtlasManager.h engine/include/core/util/Rect.h
        engine/src/core/systems/animationSystem/Animation.cpp engine/include/core/systems/animationSystem/Animation.h engine/src/core/systems/animationSystem/AnimationSystem.cpp
        engine/include/core/systems/animationSystem/AnimationSystem.h


        engine/src/core/systems/console/Console.cpp engine/include/core/systems/console/Console.h engine/src/core/systems/uiSystem/Text.cpp engine/include/core/systems/uiSystem/Text.h
        engine/src/core/systems/uiSystem/FontManager.cpp engine/include/core/systems/uiSystem/FontManager.h engine/include/stb/stb_image_writer.h engine/src/core/render/elements/FrameBuffer.cpp
        engine/include/core/render/elements/FrameBuffer.h engine/src/core/render/elements/ShaderManager.cpp engine/include/core/render/elements/ShaderManager.h engine/include/core/render/elements/IRenderizable.h
        sandbox/Sandbox.cpp sandbox/Sandbox.h engine/src/core/systems/soundSystem/SoundManager.cpp engine/include/core/systems/soundSystem/SoundManager.h
        engine/include/core/render/window/event/JoystickEvent.h engine/src/core/render/window/input/WindowInput.cpp engine/include/core/render/window/input/WindowInput.h
        engine/src/core/render/window/input/KeyboardInput.cpp engine/include/core/render/window/input/KeyboardInput.h engine/src/core/render/window/input/MouseInput.cpp
        engine/include/core/render/window/input/MouseInput.h engine/src/core/render/window/input/ControllerInput.cpp engine/include/core/render/window/input/ControllerInput.h
        engine/src/core/platform/Platform.cpp engine/include/core/platform/Platform.h engine/include/core/platform/PlatformHeaderSDL.h
        engine/include/core/platform/PlatformHeaderSDLMixer.h engine/include/core/platform/PlatformHeaderSDLImage.h
        engine/include/core/render/window/input/MobileInput.h engine/src/core/render/window/input/MobileInput.cpp engine/include/core/render/window/event/MobileEvent.h
        )

file(GLOB_RECURSE CPP_HEADERS_FOR_LIBRARY )
file(GLOB_RECURSE CPP_SOURCES_FOR_LIBRARY )

#add_library(Engine2_0 ${SOURCES} ${CPP_HEADERS_FOR_LIBRARY} ${CPP_SOURCES_FOR_LIBRARY})
add_executable(Engine2_0 engine/src/main.cpp ${SOURCES})

target_link_libraries(Engine2_0 ${TARGET_LINKS})
target_compile_definitions(Engine2_0 PRIVATE ${GL_PROFILES})

