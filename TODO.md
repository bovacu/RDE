# This is the list of things that need to be done, fixed, refactored or improved.


## TO DO
- [ ] Editor
  - [ ] Tile Map Editor UI
  - [ ] Localization Manager UI
  - [ ] Physics Editor UI
  - [ ] Animation Editor UI
  - [ ] Configuration of the whole engine tab UI
  - [ ] Building process configuration and deployment UI
  - [ ] Prefab creation UI
  - [ ] Profiler UI

- [ ] Project Manager
  - [ ] Update it as it is totally outdated.
  - [ ] Add correct module importer (For iOS and Android, for the moment)

- [ ] UI
  - [ ] ComboBox
  - [ ] List (With scroll)
  - [ ] Adjust size depending on the DPI (for mobile basically)
  - [ ] Sub-Canvases
  - [ ] UIImage partial radial rendering inverse (not implemented yet and is the last one)
  - [ ] Focus from one focusable UI element to another with 'Tab'
  - [ ] Make SDF font rendering to render also small text (maybe for smaller texts just use non-SDF rendering, will see)

- [ ] Prefabs
  - [ ] Definition and configuration
  - [ ] Load in memory
  - [ ] Remove from memory
  - [ ] Instantiation on scene
  - [ ] Destruction from scene

- [ ] Dialog system
- [ ] Database connection layer
- [ ] Config for all components
- [ ] Standarize how to add not inner engine components
- [ ] Port coordinates system to sub-windows (For example the window of the Editor or others)
- [ ] Multi-window support
- [ ] In-Engine console
- [ ] Coroutines
- [ ] Implement algorithms for terrain generation (Cellular Automata and Wave Function Collapse) 
- [ ] Engine and project manager to not use Python so it does not depend on it to build (maybe, let's see how much effort is this)
- [ ] WASM port
- [ ] Switch port
- [ ] 2D lights
- [ ] Networking layer

- [ ] Scripting language (this is a BIG MAYBE and just because I want to learn how to do it, the idea of the engine is to work with C++ or C)
  - [ ] Python
  - [ ] Rust

- [ ] Add some kind of tests


## TO FIX
- [ ] Two or more Sliders are behaving in a strange way
- [ ] ImGui for mobile
- [ ] On mobile, OnMouseClick event is sometimes not recognized as a touch input, fix this on both platforms


## TO IMPROVE
- [ ] UI
  - [ ] UIInput
    - [ ] Add right and left key movement

- [ ] Particle System
  - [ ] More customization
  - [ ] Better and easier API

- [ ] Renderizable tree for the UI, now it makes too many drawcalls as the algorithm is very simple and it is only grouping contiguous elements of the same type
- [ ] Debug vertices sent CPU -> GPU, as some games may use the line or points drawing (Like a sand falling simulator)


## TO REFACTOR
- [ ] How the render trees for UI and Renderizable elements are done, they are a bit messy and can be improved

- [ ] Now, if a Sprite or a UI element changes, the whole tree is recalculated, so not very efficent.
  - [ ] Idea: Make each Renderizable component have a pointer to its OpenGLVertex. This 
        pointers will be stored in the vector of renderizables, so any change can be applied direclty without needing to recalculate the whole tree. Problem
  - [ ] Problems:
    - [ ] If a layer is changed, then we need to check how to recalculate the tree.
    - [ ] If an element is destroyed, we need to check how to recalculate the tree (Maybe just remove the index on the vector? Need to store that index too in the Renderizable then)

- [ ] Animation System


## TO FURTHER TEST
- [ ] Transitions between scenes


## TO DOCUMENT
- [ ] Explanation of the whole Engine architecture and workflow
- [ ] Examples
- [ ] Whole API
