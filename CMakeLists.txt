cmake_minimum_required(VERSION 3.18.1)

set(PROJECT_NAME RDE)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(GL_PROFILES "GL_PROFILE_GL3")

set(CPP_HEADERS)
set(CPP_SOURCES)

if(WIN32)
    set(OS "windows")
	set(LIB_TYPE SHARED)
	set(LIB_EXT "lib")
endif()

if(APPLE)
    set(OS "osx")
	set(LIB_TYPE)
	set(LIB_EXT "a")
endif()

if(UNIX AND NOT APPLE)
    set(OS "linux")
	set(LIB_TYPE)
	set(LIB_EXT "a")
endif()

set(LIBS)

if(WIN32)
    list(APPEND LIBS ${CMAKE_CURRENT_SOURCE_DIR}/external/${OS}/lib/manual-link/SDL2main.${LIB_EXT})
endif()

list(APPEND LIBS ${CMAKE_CURRENT_SOURCE_DIR}/external/${OS}/lib/SDL2.${LIB_EXT})
list(APPEND LIBS ${CMAKE_CURRENT_SOURCE_DIR}/external/${OS}/lib/SDL2_image.${LIB_EXT})
list(APPEND LIBS ${CMAKE_CURRENT_SOURCE_DIR}/external/${OS}/lib/glad.${LIB_EXT})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/${OS}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)

include_directories(engine/include)

option(RDE_IMGUI_MODULE "Enables ImGui for the End-User" ON)
if(RDE_IMGUI_MODULE)
    add_definitions(-DRDE_IMGUI_MODULE)
endif(RDE_IMGUI_MODULE)

option(RDE_AUDIO_MODULE "Enables Audio for the End-User" ON)
if(RDE_AUDIO_MODULE)
	list(APPEND LIBS ${CMAKE_CURRENT_SOURCE_DIR}/external/${OS}/lib/SDL2_mixer.${LIB_EXT})
	add_definitions(-DRDE_AUDIO_MODULE)
endif(RDE_AUDIO_MODULE)

option(RDE_PHYSICS_MODULE "Enables Physics for the End-User" ON)
if(RDE_PHYSICS_MODULE)
	add_definitions(-DRDE_PHYSICS_MODULE)
endif(RDE_PHYSICS_MODULE)

#RDEFirebase

get_directory_property( DirDefs DIRECTORY ${CMAKE_SOURCE_DIR} COMPILE_DEFINITIONS )
foreach( d ${DirDefs} )
    message( STATUS "Found Define: " ${d} )
endforeach()

add_library(${PROJECT_NAME} ${LIB_TYPE}
${CMAKE_CURRENT_SOURCE_DIR}/engine/include/rde.h
		${CMAKE_CURRENT_SOURCE_DIR}/engine/src/rde.cpp

		${CPP_SOURCES}
		${CPP_HEADERS}
		)
target_compile_definitions(${PROJECT_NAME} PRIVATE ${GL_PROFILES})
target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra)

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})