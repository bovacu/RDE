cmake_minimum_required(VERSION 3.21)
project(Engine2_0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(PROJECT_NAME ResourcesGame)
SET(TARGET_LINKS flecs glfw3 Threads::Threads ImGui)

include_directories(.)
include_directories(${CMAKE_SOURCE_DIR}/external})

#-------------- GLAD

set(GLAD_DIR "external/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")

#-------------- IMGUI

set(IMGUI_DIR "external/imgui")
file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp)
add_library("ImGui" STATIC ${IMGUI_SOURCES})
target_include_directories("ImGui" PUBLIC ${IMGUI_DIR})

#-------------- STB

set(STB_DIR "external/stb")
include_directories(${STB_DIR})
link_directories(${STB_DIR})

#-------------- GLFW3 AND Threads

find_package(glfw3 REQUIRED)
find_package(Threads REQUIRED)

#-------------- FLECS

find_package(flecs REQUIRED)
if(flecs_FOUND)
    message("-- Found Flecs")
else()
    message("-- To make flecs work download source code, cmake ., make, sudo make install")
endif()

add_executable( Engine2_0 main.cpp

        core/render/elements/Texture.h core/render/elements/Texture.cpp core/render/elements/VertexArray.h
        core/render/elements/VertexArray.cpp core/render/elements/TextureRegion.h core/render/elements/TextureRegion.cpp
        core/render/elements/Shader.h core/render/elements/Shader.cpp core/render/elements/GraphicsContext.h
        core/render/elements/GraphicsContext.cpp core/render/elements/FrameBuffer.h core/render/elements/FrameBuffer.cpp
        core/render/elements/Buffer.h core/render/elements/Buffer.cpp

        core/util/Time.h core/util/Easing.h core/util/Functions.h core/util/Logger.h core/util/PerlinNoise.h
        core/util/Random.h core/util/Util.h core/util/Vec.h core/util/Color.cpp core/util/Color.h core/util/Delta.h
        core/util/Clock.h core/util/Clock.cpp

        core/render/window/event/Event.h core/render/window/event/KeyEvent.h core/render/window/event/MouseEvent.h
        core/render/window/event/WindowEvent.h core/render/window/Window.h core/render/window/Window.cpp

        core/render/layer/Layer.h core/render/layer/LayerStack.h core/render/layer/LayerStack.cpp
        core/render/layer/ImGuiLayer.h core/render/layer/ImGuiLayer.cpp

        core/render/window/input/GamePadKeys.h core/render/window/input/Input.cpp core/render/window/input/Input.h
        core/render/window/input/KeyboardKeys.h core/render/window/input/MouseKeys.h

        core/render/camera/Camera.h core/render/camera/OrthographicCamera.h core/render/camera/OrthographicCamera.cpp

        core/render/shapes/Shape.h core/render/shapes/Shape.cpp

        core/Engine.cpp core/Engine.h

        external/stb/stb_image.h external/stb/stb_image.cpp

        external/imgui/imgui.h external/imgui/imgui.cpp external/imgui/imstb_truetype.h external/imgui/imstb_rectpack.h
        external/imgui/imstb_textedit.h external/imgui/imgui_internal.h external/imgui/imgui_tables.cpp
        external/imgui/imgui_draw.cpp external/imgui/imconfig.h external/imgui/backends/imgui_impl_glfw.h
        external/imgui/backends/imgui_impl_glfw.cpp external/imgui/backends/imgui_impl_opengl3.h
        external/imgui/backends/imgui_impl_opengl3.cpp core/Core.h core/render/Renderer.cpp core/render/Renderer.h core/components/Components.h core/render/camera/SceneCamera.cpp core/render/camera/SceneCamera.h)

target_link_libraries(Engine2_0 ${TARGET_LINKS})

#--------------- GLAD
target_link_libraries (Engine2_0 "glad" "${CMAKE_DL_LIBS}")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(Engine2_0 PRIVATE "${GLAD_DIR}/include")

