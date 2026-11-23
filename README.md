# Pathfinding Visualizer - modular source

C++ / Raylib visualiser for procedural maze generation and four pathfinding
algorithms (BFS, DFS, Dijkstra, A*).

## Build

```bat
build.cmd
```

Produces `PathfindingVisualizer.exe` next to the sources. The compiler and
Raylib are the portable copies in `..\..\_tools` (MinGW-w64 GCC 16.2.0 and
Raylib 6.2), so nothing needs to be installed system-wide. The script keeps the
compiler's scratch files in `.tmp\` so it also works on machines that restrict
writes to the system temp folder.

To build by hand:

```bat
g++ core.cpp maze.cpp bfs.cpp dfs.cpp dijkstra.cpp astar.cpp ui.cpp main.cpp ^
    -o PathfindingVisualizer.exe -O2 -std=c++17 -Wall ^
    -I..\..\_tools\raylib-6.2_win64_mingw-w64\include ^
    -L..\..\_tools\raylib-6.2_win64_mingw-w64\lib ^
    -lraylib -lopengl32 -lgdi32 -lwinmm -ldwmapi ^
    -static -static-libgcc -static-libstdc++
```

## Look and feel

The UI is a dark board with one saturated accent per algorithm, so the four
searches are distinguishable at a glance:

| Element | Colour |
| --- | --- |
| Background | `#080A0E` |
| Toolbar | `#0D1016` with a gradient and a hairline border |
| Board panel | `#0B0E13`, rounded, with a subtle outline |
| Cell floor | `#11151D` |
| Walls | `#2F3B4E` |
| Start / target | `#FF5C5C` / `#42E08B` |
| BFS | blue `#4D9EFF` |
| DFS | amber `#FFB454` |
| Dijkstra | cyan `#2EE6D6` |
| A* | violet `#B388FF` |

Each algorithm's accent is reused for its button, its exploration wash and its
final path, so the screen reads as one system. Text uses Segoe UI, rasterised at
two sizes (16px and 22px) so glyphs are drawn near the size they were generated
at rather than scaled down from a single large atlas.

The window is 824x684, which fits a 1366x768 screen without the board running
under the taskbar. The window is centred, the title bar is set to dark mode, and
buttons animate on hover.

## Modules

| File | Responsibility |
| --- | --- |
| `core.h` / `core.cpp` | Shared constants, geometry helpers, colour palette, `GameState`, `GridPos`, `cell`, all global state, `ResetMaze()` |
| `maze.h` / `maze.cpp` | Randomised DFS maze generation, board rendering (`DrawPath`) and markers (`DrawMarkers`) |
| `bfs.h` / `bfs.cpp` | Breadth-First Search + visualisation |
| `dfs.h` / `dfs.cpp` | Depth-First Search + visualisation |
| `dijkstra.h` / `dijkstra.cpp` | Dijkstra's algorithm + visualisation |
| `astar.h` / `astar.cpp` | A* with Manhattan heuristic + visualisation |
| `ui.h` / `ui.cpp` | Toolbar, buttons, status pill, font loading |
| `main.cpp` | Window setup and the update/draw loop |

Every module only depends on `core.h`; the UI and `main.cpp` are the two places
that know about all of them.

## How the pieces fit together

`core.h` declares the shared state (`extern`) and `core.cpp` defines it exactly
once. The four search algorithms each own:

* an "is visited" table (`bfsVisited`, `dfsVisited`) or a cost table
  (`bestCost`, `bestCostA`);
* a parent table used to rebuild the final path;
* one container (`bfsQueue`, `dfsStack`, `dijkstraQueue`, `aStarQueue`).

A search advances one cell per frame, which is what makes the traversal
visible. When the target cell is popped, the algorithm reconstructs the path
and sets `gameState` back to `StateWaiting`, re-enabling the buttons.

Drawing order in `main.cpp` matters: the board is drawn first, then the search
overlay, then the start/target markers, so the markers are never painted over by
the exploration wash.
