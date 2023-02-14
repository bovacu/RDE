cmake_minimum_required(VERSION 3.18.1)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(VCPKG_TARGET_TRIPLET x64-osx CACHE STRING "VCPKG Target Triplet to use")
set(PROJECT_NAME RDE)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_OSX_DEPLOYMENT_TARGET 12.6)
# set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++")

add_compile_options("-Werror")

set(GL_PROFILES "GL_PROFILE_GL3")

file(GLOB_RECURSE RDE_CPP_HEADERS engine/include/*.h*)
file(GLOB_RECURSE RDE_CPP_SOURCES engine/src/*.cpp )

set(CPP_HEADERS)
set(CPP_SOURCES)

set(SDL2_DIR vcpkg/installed/x64-linux/share/sdl2)
set(EnTT_DIR vcpkg/installed/x64-osx/share/entt)
set(sdl2_image_DIR vcpkg/installed/x64-osx/share/sdl2-image)
set(SDL2_mixer_DIR vcpkg/installed/x64-osx/share/SDL2_mixer)
set(freetype_DIR vcpkg/installed/x64-osx/share/freetype)
set(glad_DIR vcpkg/installed/x64-osx/share/glad)
set(nlohmann_json_DIR vcpkg/installed/x64-osx/share/nlohmann_json)
set(imgui_DIR vcpkg/installed/x64-osx/share/imgui)
set(glm_DIR vcpkg/installed/x64-osx/share/glm)

find_package(SDL2 CONFIG REQUIRED PATHS vcpkg/installed/x64-osx/share/sdl2)
find_package(sdl2_image CONFIG REQUIRED PATHS vcpkg/installed/x64-osx/share/sdl2-image)
find_package(SDL2_mixer CONFIG REQUIRED PATHS vcpkg/installed/x64-osx/share/SDL2_mixer)
find_package(glad CONFIG REQUIRED PATHS vcpkg/installed/x64-osx/share/glad)
find_package(nlohmann_json CONFIG REQUIRED PATHS vcpkg/installed/x64-osx/share/nlohmann_json)
find_package(imgui CONFIG REQUIRED PATHS vcpkg/installed/x64-osx/share/imgui)
find_package(glm CONFIG REQUIRED PATHS vcpkg/installed/x64-osx/share/glm)
find_package(EnTT CONFIG REQUIRED PATHS vcpkg/installed/x64-osx/share/entt)
find_package(freetype REQUIRED PATHS vcpkg/installed/x64-osx/share/freetype)

include_directories(engine/include)
include_directories(nonVcpkgDeps/Chipmunk2D/include)
include_directories(vcpkg/installed/x64-osx/include)
include_directories(vcpkg/installed/x64-osx/include/freetype)
include_directories(editor)

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
    add_executable(${PROJECT_NAME}
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
                        SDL2::SDL2 SDL2::SDL2main SDL2::SDL2-static
                        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
                        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
                        freetype
                        ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-osx/lib/libglad.a
                        ${CMAKE_CURRENT_SOURCE_DIR}/nonVcpkgDeps/Chipmunk2D/libs/Mac/libchipmunk.a
                        EnTT::EnTT
                        nlohmann_json::nlohmann_json
                        imgui::imgui
                        glm::glm
                    )