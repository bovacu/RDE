cmake_minimum_required(VERSION 3.21)
project(Engine2_0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(PROJECT_NAME Engine2_0)
SET(TARGET_LINKS OpenGL GLEW glfw Threads::Threads flecs)

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

#-------------- FLECS

find_package(flecs REQUIRED)
if(flecs_FOUND)
    message("-- Found Flecs")
else()
    message("-- To make flecs work download source code, cmake ., make, sudo make install")
endif()

add_executable( Engine2_0 main.cpp

        core/util/Time.h core/util/Easing.h core/util/Functions.h core/util/Logger.h core/util/PerlinNoise.h
        core/util/Random.h core/util/Util.h core/util/Vec.h core/util/Color.cpp core/util/Color.h core/util/Delta.h
        core/util/Clock.h core/util/Clock.cpp

        core/render/window/event/Event.h core/render/window/event/KeyEvent.h core/render/window/event/MouseEvent.h
        core/render/window/event/WindowEvent.h core/render/window/Window.h core/render/window/Window.cpp

#        core/render/layer/Layer.h core/render/layer/LayerStack.h core/render/layer/LayerStack.cpp
#        core/render/layer/ImGuiLayer.h core/render/layer/ImGuiLayer.cpp

        core/render/window/input/GamePadKeys.h core/render/window/input/Input.cpp core/render/window/input/Input.h
        core/render/window/input/KeyboardKeys.h core/render/window/input/MouseKeys.h

        core/render/elements/Texture.h core/render/elements/Texture.cpp core/render/elements/SpriteBatch.h
        core/render/elements/SpriteBatch.cpp core/render/elements/Shader.h core/render/elements/Shader.cpp

        core/Engine.cpp core/Engine.h core/Core.h

        external/stb/stb_image.h external/stb/stb_image.cpp external/json.h

        core/render/Renderer.cpp core/render/Renderer.h core/systems/eventSystem/EventBus.h

#        external/imgui/imgui.h external/imgui/imgui.cpp external/imgui/imstb_truetype.h external/imgui/imstb_rectpack.h
#        external/imgui/imstb_textedit.h external/imgui/imgui_internal.h external/imgui/imgui_tables.cpp
#        external/imgui/imgui_draw.cpp external/imgui/imconfig.h external/imgui/backends/imgui_impl_glfw.h
#        external/imgui/backends/imgui_impl_glfw.cpp external/imgui/backends/imgui_impl_opengl3.h
#        external/imgui/backends/imgui_impl_opengl3.cpp

        core/systems/Components.h core/render/elements/Sprite.cpp core/render/elements/Sprite.h core/render/Camera.cpp core/render/Camera.h core/render/shapes/Shape.h)

target_link_libraries(Engine2_0 ${TARGET_LINKS})

