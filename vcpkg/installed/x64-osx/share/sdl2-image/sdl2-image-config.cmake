include(CMakeFindDependencyMacro)

find_dependency(SDL2 CONFIG)

if(OFF)
    find_dependency(WebP CONFIG)
endif()

if (ON)
    find_dependency(PNG)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/sdl2-image-targets.cmake")
