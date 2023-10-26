# RDE (Rubber Duck Engine)

Rubber Duck Engine is a general purpose game engine for 2D and basic 3D development. It is built in C99 and compiles for Windows, Linux, Mac, Android, iOS and WASM. It uses as core libs SDL2, Glad, CGLM, miniaudio and some 3d model format parsin libraries.

This repo provides 2 main elements, the core library and the external tools:
  - Tools:
    - Sprite Atlas Generator: given a directory, it creates a full texture atlas with all the textures inside
    - Font Atlas Generator: given a font file, it creates a full texture atlas for all the fonts
    - Project Generator: creates a base project template to start working with the library

The library is composed of the core module (which is always added) and optional modules, those are:
  - audio
  - physics_2d
  - physics_3d
  - rendering
  - ui
  - error
  - 3D format parser libs
Those modules are not required to use the library and are added when compiling the library.

To build the library it is as easy as compiling the engine compiler program:
```
clang rde_build.c -o builder
```
And this will create the engine compiler. This is a C program that will compile the engine with specific settings. If you make
```
builder --help
```
This will print out the options for building the engine, but the basic options are:
  - -b: this specifies what you want to build. Options are:
    - engine: only builds the engine
    - examples: builds the engie and the examples hub
    - tools: only builds the external tools
    - tests: builds the engine and the unit-tests
    - all: builds all the previous elements
    And example is 'builder -b=all'
  - -bt: specifies if the build is debug or release. Also those are the options. An example is 'builder -bt=release'
  - -lt: specifies if the output library is static or shared. Also those are the options. An example is 'builder -lt=shared'
  - -m: specifies the list of optional modules to add. Options are [audio,physics_2d,physics_3d,rendering,fbx,obj,gltf,ui,error]. . An example is 'builder -m="[renedring,audio,errpr]"'

After building with those options, you will get the engine lib, the example hub executable, the tools executables or the unit-test executables (depending on the options).

Features:
  - 2D sprite rendering with batching system
  - 2D text rendering
  - 3D model and mesh rendering (OBJ done, FBX WIP, GLTF WIP)
  - 3D lighting (WIP)
  - 3D PBR Materials (TODO)
  - 3D text rendering (WIP)
  - 2D Physics (WIP)
  - 3D Physics (TODO)
  - Window event handling
  - Multiple window management (rendering WIP)
  - Audio (WIP)
  - File System functions (WIP)
  - More features to come, but not yet on the roadmap. This document will be updated with more features.

Screenshots:
Model Viewer
![Captura de tela 2023-10-26 112727](https://github.com/bovacu/RDE/assets/36163709/b6a98d0e-117d-48a6-a90a-013601d18ce3)


