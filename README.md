# Pathfinding Visualizer

Interactive A* pathfinding visualizer built with modern C++ and SFML. Paint obstacles, choose start and goal tiles, then watch the algorithm explore the grid and reveal the final path.

<img width="962" height="670" alt="Screenshot 2025-10-29 at 3 46 07 AM" src="https://github.com/user-attachments/assets/1133aea7-6ad1-413e-842c-8c54330b2d29" />



## Prerequisties
- CMake
- A C++ compiler
- Git (CMake will fetch SFML automatically)

## Build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The executable is placed at `./build/pathfinding_visualizer`.

## Run
```bash
./build/pathfinding_visualizer
```

## Controls
- Left mouse drag: paint obstacles
- Right mouse drag: erase obstacles
- `S`: set start at the hovered cell
- `G`: set goal at the hovered cell
- `Enter`: start or restart the search
- `R`: reset the current search overlays
- `C`: clear all obstacles

The open list, closed list, and final path are highlighted in distinct colors as the search progresses.
