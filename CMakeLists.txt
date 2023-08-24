cmake_minimum_required(VERSION 3.18.1)

set(PROJECT_NAME RDE)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(GL_PROFILES "GL_PROFILE_GL3")

set(CPP_HEADERS)
set(CPP_SOURCES)

set(OS)

if(WIN32)
    set(OS "windows")
endif()

if(APPLE)
    set(OS "osx")
endif()

if(UNIX AND NOT APPLE)
    set(OS "linux")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/${OS}/include)
include_directories(engine/include)

#RDEFirebase

option(RDE_LIBRARY "Build library instead of executable" OFF)
if(RDE_LIBRARY)
	add_definitions(-DRDE_LIBRARY)
    message("Building lib")
    add_library(${PROJECT_NAME} SHARED
            ${CMAKE_CURRENT_SOURCE_DIR}/engine/include/rde.h
            ${CMAKE_CURRENT_SOURCE_DIR}/engine/src/rde.cpp

            ${CPP_SOURCES}
            ${CPP_HEADERS}
            )
    target_compile_definitions(${PROJECT_NAME} PRIVATE ${GL_PROFILES} RDE_LIBRARY=1)
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror)

else()
    message("Building executable")
    add_executable(${PROJECT_NAME} WIN32
			${CMAKE_CURRENT_SOURCE_DIR}/engine/include/rde.h
            ${CMAKE_CURRENT_SOURCE_DIR}/engine/src/rde.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/engine/main.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/test/test.cpp

            ${CPP_SOURCES}
            ${CPP_HEADERS}
            )
    target_compile_definitions(${PROJECT_NAME} PRIVATE ${GL_PROFILES})
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror)
endif()
unset(RDE_LIBRARY CACHE)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/${OS}/lib/manual-link/SDL2main.lib
		${CMAKE_CURRENT_SOURCE_DIR}/external/${OS}/lib/SDL2.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/external/${OS}/lib/SDL2_image.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/external/${OS}/lib/SDL2_mixer.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/external/${OS}/lib/freetype.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/external/${OS}/lib/glad.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/external/${OS}/lib/imgui.lib
        )