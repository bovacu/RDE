cmake_minimum_required(VERSION 3.18.1)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(PROJECT_NAME GDE)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++")

add_compile_options("-Werror")

set(GL_PROFILES "GL_PROFILE_GL3")

file(GLOB_RECURSE GDE_CPP_HEADERS engine/include/*.h*)
file(GLOB_RECURSE GDE_CPP_SOURCES engine/src/*.cpp )

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

include_directories(engine/include)
include_directories(editor)

#GDEFirebase

option(GDE_LIBRARY "Build library instead of executable" OFF)
if(GDE_LIBRARY)
    message("Building lib")
    add_library(${PROJECT_NAME} STATIC
            ${GDE_CPP_HEADERS}
            ${GDE_CPP_SOURCES}

            ${CPP_SOURCES}
            ${CPP_HEADERS}
            )
    target_compile_definitions(${PROJECT_NAME} PRIVATE ${GL_PROFILES})

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND bash -c "./postLibraryBuildingProcess.sh" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

else()
    message("Building executable")
    add_executable(${PROJECT_NAME} engine/main.cpp 
            editor/Editor.cpp

            ${GDE_CPP_HEADERS}
            ${GDE_CPP_SOURCES}

            ${CPP_SOURCES}
            ${CPP_HEADERS}
            )
    target_compile_definitions(${PROJECT_NAME} PRIVATE ${GL_PROFILES})
endif()
unset(GDE_LIBRARY CACHE)

target_link_libraries(${PROJECT_NAME}
                        PRIVATE
                        SDL2::SDL2 SDL2::SDL2main SDL2::SDL2-static
                        SDL2::SDL2_image
                        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
                        ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/packages/freetype_x64-linux/lib/libfreetype.a
                        box2d::box2d
                        glad::glad
                        EnTT::EnTT
                        nlohmann_json::nlohmann_json
                        imgui::imgui
                        glm::glm
                    )