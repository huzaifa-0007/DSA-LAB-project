#ifndef CORE_H
#define CORE_H

// ===========================================================================
// core.h - Shared types, constants and global state.
//
// Every translation unit in the project includes this header. It owns:
//   * window and grid geometry
//   * the colour palette
//   * the GameState values used by the finite state machine
//   * the shared grid, containers and per-algorithm bookkeeping tables
// ===========================================================================

#include <cmath>
#include <functional>
#include <queue>
#include <stack>
#include <vector>

#include "raylib.h"

// ---------------------------------------------------------------------------
// Window and grid geometry
//
// The window is sized so the whole board fits on a 1366x768 screen: the client
// area stays under 700px tall, leaving room for the title bar and the taskbar.
// ---------------------------------------------------------------------------
const int CellSize     = 32;   // Cell edge in pixels
const int ColumnCount  = 25;   // Grid columns
const int RowCount     = 18;   // Grid rows
const int HeaderHeight = 84;   // Height of the toolbar at the top
const int BoardMargin  = 12;   // Gap between the grid and the window edge

const int BoardX      = BoardMargin;                 // Left edge of the grid
const int BoardY      = HeaderHeight + BoardMargin;  // Top edge of the grid
const int BoardWidth  = ColumnCount * CellSize;      // 800
const int BoardHeight = RowCount * CellSize;         // 576

const int ScreenWidth  = BoardWidth + BoardMargin * 2;        // 824
const int ScreenHeight = BoardY + BoardHeight + BoardMargin;  // 684

// Top-left pixel of a grid cell. Every module draws through these two helpers,
// so the board can be repositioned without touching the algorithms.
inline int CellX(int col) { return BoardX + col * CellSize; }
inline int CellY(int row) { return BoardY + row * CellSize; }

// Fills one cell with a colour, inset by `inset` pixels and with rounded
// corners. Used for the exploration wash and the final path.
inline void FillCell(int row, int col, float inset, float roundness, Color color) {
    Rectangle r = {(float)(CellX(col) + inset), (float)(CellY(row) + inset),
                   (float)(CellSize - inset * 2.0f), (float)(CellSize - inset * 2.0f)};
    DrawRectangleRounded(r, roundness, 6, color);
}

// ---------------------------------------------------------------------------
// Colour palette
//
// A dark "board game" palette: near-black background, a slightly lighter
// toolbar, and one saturated accent per algorithm so the four searches are
// instantly distinguishable.
// ---------------------------------------------------------------------------
extern Color AppBg;          // Window background
extern Color HeaderBg;       // Toolbar background
extern Color HeaderBgTop;    // Toolbar background, top of the gradient
extern Color HeaderBorder;   // Thin line under the toolbar

extern Color PanelBg;        // Board panel background
extern Color PanelBorder;    // Board panel outline
extern Color CellFloor;      // Floor of a carved cell
extern Color WallColor;      // Maze walls

extern Color TextPrimary;    // Headings and button labels
extern Color TextMuted;      // Secondary text
extern Color TextDim;        // Disabled text

extern Color StartColor;     // Start marker
extern Color TargetColor;    // Target marker

extern Color BfsAccent;      // Breadth-First Search
extern Color DfsAccent;      // Depth-First Search
extern Color DijkstraAccent; // Dijkstra
extern Color AStarAccent;    // A*
extern Color GenericAccent;  // "Generate Maze" button

// ---------------------------------------------------------------------------
// Application states (finite state machine)
//
// The state machine guarantees that a search can only be started once the
// maze has finished generating, which prevents overlapping updates.
// ---------------------------------------------------------------------------
enum GameState {
    StateIdle,            // Application just launched, nothing to draw yet
    StateGeneratingMaze,  // Maze generation is running
    StateWaiting,         // Maze finished, any search may be started
    StateBfsRunning,      // BFS traversal running
    StateDfsRunning,      // DFS traversal running
    StateDijkstraRunning, // Dijkstra traversal running
    StateAStarRunning     // A* traversal running
};

// ---------------------------------------------------------------------------
// Basic data structures
// ---------------------------------------------------------------------------

// A single cell coordinate on the grid.
struct GridPos {
    int row;  // Row index
    int col;  // Column index
};

// An entry of the priority queue. "cost" is the value the queue sorts on; the
// comparison operator makes std::greater<PriorityNode> behave as a min-heap.
struct PriorityNode {
    GridPos position;
    int cost;

    bool operator>(const PriorityNode &other) const {
        return cost > other.cost;
    }
};

// One cell of the maze. Each cell keeps its own four walls so that carving a
// passage is a local operation on two neighbouring cells.
struct cell {
    bool isVisited  = false;  // Whether the generator has already entered it
    bool wallRight  = true;   // Right wall
    bool wallLeft   = true;   // Left wall
    bool wallTop    = true;   // Top wall
    bool wallBottom = true;   // Bottom wall
};

// ---------------------------------------------------------------------------
// Shared global state
//
// Declared here and defined exactly once, in core.cpp.
// ---------------------------------------------------------------------------
extern GameState gameState;

extern int targetRow;  // Target row
extern int targetCol;  // Target column
extern int startRow;   // Start row
extern int startCol;   // Start column

extern cell mazeGrid[RowCount][ColumnCount];  // The maze grid itself

// Containers owned by the individual algorithms. They live here because the
// UI has to clear them, and main.cpp has to inspect them.
extern std::stack<GridPos> generatorStack;  // Backtracking stack of the maze generator
extern std::stack<GridPos> dfsStack;        // DFS search stack
extern std::queue<GridPos> bfsQueue;        // BFS search queue

// Priority queues for Dijkstra and A*. std::priority_queue has no clear(),
// so the UI reassigns a fresh empty queue instead.
extern std::priority_queue<PriorityNode, std::vector<PriorityNode>, std::greater<PriorityNode>> dijkstraQueue;
extern std::priority_queue<PriorityNode, std::vector<PriorityNode>, std::greater<PriorityNode>> aStarQueue;

extern int bestCost[RowCount][ColumnCount];   // Dijkstra: best known cost per cell
extern int bestCostA[RowCount][ColumnCount];  // A*:       best known cost per cell

extern GridPos parentBfs[RowCount][ColumnCount];       // BFS parent table (for backtracking)
extern GridPos parentDfs[RowCount][ColumnCount];       // DFS parent table
extern GridPos parentDijkstra[RowCount][ColumnCount];  // Dijkstra parent table
extern GridPos parentAStar[RowCount][ColumnCount];     // A* parent table

extern bool bfsVisited[RowCount][ColumnCount];  // BFS visited flags
extern bool dfsVisited[RowCount][ColumnCount];  // DFS visited flags

extern std::vector<GridPos> pathBfs;       // Reconstructed BFS path
extern std::vector<GridPos> pathDfs;       // Reconstructed DFS path
extern std::vector<GridPos> pathDijkstra;  // Reconstructed Dijkstra path
extern std::vector<GridPos> pathAStar;     // Reconstructed A* path

// Resets every piece of board and search state back to its initial value.
// Called when the user presses "Generate Maze".
void ResetMaze();

#endif // CORE_H
