cmake_minimum_required(VERSION 3.23)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(PROJECT_NAME RDE)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_compile_options("-Werror")

set(GL_PROFILES "GL_PROFILE_GL3")

file(GLOB_RECURSE RDE_CPP_HEADERS engine/include/*.h*)
file(GLOB_RECURSE RDE_CPP_SOURCES engine/src/*.cpp )

set(CPP_HEADERS)
set(CPP_SOURCES)

find_package(EnTT CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED PATHS vcpkg/packages/nlohmann-json_x64-linux/share/nlohmann_json)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

include_directories(
        ${SDL2_INCLUDE_DIR}
        ${SDL2_INCLUDE_DIR}/SDL2
        ${SDL2_IMAGE_INCLUDE_DIR}
        ${SDL2_MIXER_INCLUDE_DIR}
        ${FREETYPE_INCLUDE_DIR}
        ${GLAD_INCLUDE_DIR}
        ${BOX2D_INCLUDE_DIR}
        ${ENTT_INCLUDE_DIR}
        ${JSON_INCLUDE_DIR}
        ${IMGUI_INCLUDE_DIR}
        ${GLM_INCLUDE_DIR}
)

include_directories(engine/include)
include_directories(editor)
include_directories(vcpkg/installed/x64-windows/include)

#RDEFirebase

option(RDE_LIBRARY "Build library instead of executable" OFF)
if(RDE_LIBRARY)
    message("Building lib")
    add_library(${PROJECT_NAME} STATIC
            ${RDE_CPP_HEADERS}
            ${RDE_CPP_SOURCES}

            ${CPP_SOURCES}
            ${CPP_HEADERS}
            )
    target_compile_definitions(${PROJECT_NAME} PRIVATE ${GL_PROFILES})

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND bash -c "./postLibraryBuildingProcess.sh" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

else()
    message("Building executable")
    add_executable(${PROJECT_NAME} WIN32
            editor/Editor.cpp editor/EditorEntry.cpp

            ${RDE_CPP_HEADERS}
            ${RDE_CPP_SOURCES}

            ${CPP_SOURCES}
            ${CPP_HEADERS}
            )
    target_compile_definitions(${PROJECT_NAME} PRIVATE ${GL_PROFILES})
endif()
unset(RDE_LIBRARY CACHE)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        SDL2::SDL2main SDL2::SDL2
        SDL2::SDL2_image
        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
        freetype
        glad::glad
        EnTT::EnTT
        nlohmann_json::nlohmann_json
        imgui::imgui
        glm::glm
        ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows/lib/chipmunk.lib
        )