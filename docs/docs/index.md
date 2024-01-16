# Welcome to RDE documentation page

RDE is a 2D/3D multi-purpose and cross-platform game engine. It is written in C99
and it consists of one header file and one source file, plus the dependencies.

The engine is meant to be simple and straightforward, with simple but powerfull and
easy to use functions.

## Modules

The engine is built around modules, which can be enabled or disabled during the 
compilation of the engine. The engine follows the idea of "use what you need".
Each module is explained on its section. Available modules are:

* physics
* imgui
* error
* obj
* fbx
* gltf
* audio

## Dependencies

* SDL3
* OpenGL (through GLAD mainly, may change depending on the platform)
* Jolt (for physics, if the module is selected)
* ImGui (if the module is selected)
* Minicoro (for coroutines)
* Miniaudio (if the module is selected)

## Build system

RDE does not use CMake, Make, Premake or other build systems. It includes a single c source file
that needs to be compiled by the user and it will compile a tool that manages the whole engine
compilation. Read section [build system](build_system.md).

The engine also comes with prebuilt libraries if users don't want to build it from source.

The engine currently only supports Clang compiler, no other compilers have been tested so 
be aware that compilation errors or bugs might be found.

## API
Full API documentation of the engine [here](api/api.md).

## Modules
