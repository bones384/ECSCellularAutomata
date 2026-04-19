# Entity Component System in cellular automata

A demonstration of how an ECS can be used for efficient cellular automata based simulations. Includes a forest fire sim (with procedural forest generation) and an implementation of Conway's Game of Life. Powered by Raylib.

## Key features 

* Real-time fire spread simulation in procedurally generated forest
  * Multi-octave Perlin noise-based forest generation

* Interactive Conway’s Game of Life
  *  save/load states
    * rewind to saved state
  *  simulation speed control
    * pause and step-forward functionality

## Architecture

* Entity Component System (lightweight implementation)
* CPU-based simulation and rendering
* Focus on simplicity and iteration speed
* Raygui and TinyFileDialogs libraries used to ensure multiplatform support
  
## Related Work

This project uses a minimal ECS focused on simplicity.

A [newer project](https://github.com/bones384/TitleRoguelike) explores a more advanced architecture with:
- cache-friendly ECS design
  - components stored in sparse vectors
- robust level generation
- pathfinding
- line-of-sight calculation

## Building

The provided CMakeLists.txt automatically downloads dependencies and configures the project. Tested on Windows and Linux. 
