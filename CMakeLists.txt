cmake_minimum_required(VERSION 3.21)
project(Engine2_0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(PROJECT_NAME Engine2_0)
SET(TARGET_LINKS OpenGL GLEW glfw Threads::Threads flecs IrrKlang
#        sndfile openal
        )

include_directories(.)
include_directories(${CMAKE_SOURCE_DIR}/external})

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

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenAL REQUIRED)
#find_package(IrrKlang REQUIRED)


#---------------- SNDFILE
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBSNDFILE_PKGCONF sndfile)
endif(PKG_CONFIG_FOUND)

# Include dir
find_path(LIBSNDFILE_INCLUDE_DIR
        NAMES sndfile.h
        PATHS ${LIBSNDFILE_PKGCONF_INCLUDE_DIRS}
        )

# Library
find_library(LIBSNDFILE_LIBRARY
        NAMES sndfile libsndfile-1
        PATHS ${LIBSNDFILE_PKGCONF_LIBRARY_DIRS}
        )

#--------------- IRRKLANG

set(IRRKLANG_DIR "external/irrKlang")
include_directories(${IRRKLANG_DIR}/include)
link_directories(${IRRKLANG_DIR}/bin/linux-gcc-64)

#-------------- FLECS

find_package(flecs REQUIRED)
if(flecs_FOUND)
    message("-- Found Flecs")
else()
    message("-- To make flecs work download source code, cmake ., make, sudo make install")
endif()

add_executable( Engine2_0 main.cpp

        core/util/Easing.h core/util/Functions.h core/util/Logger.h core/util/PerlinNoise.h
        core/util/Random.h core/util/Util.h core/util/Vec.h core/util/Color.cpp core/util/Color.h core/util/Delta.h
        core/util/Clock.h core/util/Clock.cpp

        core/render/window/event/Event.h core/render/window/event/KeyEvent.h core/render/window/event/MouseEvent.h
        core/render/window/event/WindowEvent.h core/render/window/Window.h core/render/window/Window.cpp

        core/render/layers/Layer.h core/render/layers/LayerStack.h core/render/layers/LayerStack.cpp
        core/render/layers/ImGuiLayer.h core/render/layers/ImGuiLayer.cpp

        core/render/window/input/GamePadKeys.h core/render/window/input/Input.cpp core/render/window/input/Input.h
        core/render/window/input/KeyboardKeys.h core/render/window/input/MouseKeys.h

        core/render/elements/Texture.h core/render/elements/Texture.cpp core/render/elements/SpriteBatch.h
        core/render/elements/SpriteBatch.cpp core/render/elements/Shader.h core/render/elements/Shader.cpp

        core/Engine.cpp core/Engine.h core/Core.h

        external/stb/stb_image.h external/stb/stb_image.cpp external/json.h

        core/render/Renderer.cpp core/render/Renderer.h core/systems/eventSystem/EventBus.h

        core/systems/Components.h core/render/elements/Sprite.cpp core/render/elements/Sprite.h core/render/Camera.cpp core/render/Camera.h core/render/shapes/Shape.h

        external/imgui/imgui.h external/imgui/imgui.cpp external/imgui/imstb_truetype.h external/imgui/imstb_rectpack.h
        external/imgui/imstb_textedit.h external/imgui/imgui_internal.h external/imgui/imgui_tables.cpp
        external/imgui/imgui_draw.cpp external/imgui/imconfig.h external/imgui/backends/imgui_impl_glfw.h external/imgui/imgui_widgets.cpp
        external/imgui/backends/imgui_impl_glfw.cpp external/imgui/backends/imgui_impl_opengl3.h
        external/imgui/backends/imgui_impl_opengl3.cpp external/imgui/imgui_demo.cpp

        # https://github.com/bwrsandman/imgui-flame-graph
        # external/imgui/flame/imgui_widget_flamegraph.h external/imgui/flame/imgui_widget_flamegraph.cpp

        # https://github.com/epezent/implot
        external/imgui/implot/implot.h external/imgui/implot/implot.cpp external/imgui/implot/implot_internal.h external/imgui/implot/implot_items.cpp

        # https://github.com/thedmd/imgui-node-editor
        external/imgui/nodeEditor/crude_json.h external/imgui/nodeEditor/crude_json.cpp external/imgui/nodeEditor/imgui_bezier_math.h
        external/imgui/nodeEditor/imgui_canvas.h external/imgui/nodeEditor/imgui_canvas.cpp external/imgui/nodeEditor/imgui_extra_math.h
        external/imgui/nodeEditor/imgui_node_editor.h external/imgui/nodeEditor/imgui_node_editor.cpp external/imgui/nodeEditor/imgui_node_editor_api.cpp
        external/imgui/nodeEditor/imgui_node_editor_internal.h

        core/systems/profiling/Profiler.h core/systems/profiling/Profiler.cpp core/render/layers/ImGuiCore.h
        core/render/elements/TextureAtlasManager.cpp core/render/elements/TextureAtlasManager.h core/util/Rect.h
        core/systems/animationSystem/Animation.cpp core/systems/animationSystem/Animation.h core/systems/animationSystem/AnimationSystem.cpp
        core/systems/animationSystem/AnimationSystem.h core/systems/soundSystem/SoundSystem.cpp core/systems/soundSystem/SoundSystem.h

        external/irrKlang/include/irrKlang.h external/irrKlang/include/ik_ESoundEngineOptions.h external/irrKlang/include/ik_ESoundOutputDrivers.h
        external/irrKlang/include/ik_EStreamModes.h external/irrKlang/include/ik_IAudioRecorder.h external/irrKlang/include/ik_IAudioStream.h
        external/irrKlang/include/ik_IAudioStreamLoader.h external/irrKlang/include/ik_IFileFactory.h external/irrKlang/include/ik_IFileReader.h
        external/irrKlang/include/ik_IRefCounted.h external/irrKlang/include/ik_irrKlangTypes.h external/irrKlang/include/ik_ISound.h
        external/irrKlang/include/ik_ISoundDeviceList.h external/irrKlang/include/ik_ISoundEffectControl.h external/irrKlang/include/ik_ISoundEngine.h
        external/irrKlang/include/ik_ISoundMixedOutputReceiver.h external/irrKlang/include/ik_ISoundSource.h external/irrKlang/include/ik_ISoundStopEventReceiver.h
        external/irrKlang/include/ik_IVirtualRefCounted.h external/irrKlang/include/ik_SAudioStreamFormat.h external/irrKlang/include/ik_vec3d.h

#        core/systems/soundSystem/SoundBuffer.cpp core/systems/soundSystem/SoundBuffer.h core/systems/soundSystem/SoundDevice.cpp
#        core/systems/soundSystem/SoundDevice.h core/systems/soundSystem/SoundSource.cpp core/systems/soundSystem/SoundSource.h

        core/systems/console/Console.cpp core/systems/console/Console.h)

target_link_libraries(Engine2_0 ${TARGET_LINKS})

