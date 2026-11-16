// ===========================================================================
// core.cpp - Definitions of the shared global state declared in core.h.
// ===========================================================================

#include "core.h"

// ---------------------------------------------------------------------------
// Colour palette
//
// Neutrals carry the layout, accents carry the meaning. Each algorithm gets
// one accent, reused for its button, its exploration wash and its final path
// so the whole screen reads as a single system.
// ---------------------------------------------------------------------------
Color AppBg        = { 8, 10, 14, 255 };     // Window background
Color HeaderBg     = { 13, 16, 22, 255 };    // Toolbar
Color HeaderBgTop  = { 18, 22, 30, 255 };    // Toolbar gradient (top)
Color HeaderBorder = { 30, 38, 52, 255 };    // Hairline under the toolbar

Color PanelBg      = { 11, 14, 19, 255 };    // Board panel
Color PanelBorder  = { 28, 35, 47, 255 };    // Board outline
Color CellFloor    = { 17, 21, 29, 255 };    // Carved cell floor
Color WallColor    = { 47, 59, 78, 255 };    // Maze walls

Color TextPrimary  = { 226, 233, 245, 255 }; // Headings
Color TextMuted    = { 122, 136, 158, 255 }; // Secondary text
Color TextDim      = { 74, 86, 104, 255 };   // Disabled text

Color StartColor   = { 255, 92, 92, 255 };   // Start marker
Color TargetColor  = { 66, 224, 139, 255 };  // Target marker

Color BfsAccent      = { 77, 158, 255, 255 };  // Blue
Color DfsAccent      = { 255, 180, 84, 255 };  // Amber
Color DijkstraAccent = { 46, 230, 214, 255 };  // Cyan
Color AStarAccent    = { 179, 136, 255, 255 }; // Violet
Color GenericAccent  = { 120, 160, 255, 255 }; // Neutral accent for Generate

// ---------------------------------------------------------------------------
// Random start / target positions.
//
// They are picked once at start-up; pressing "Generate Maze" re-rolls them.
// ---------------------------------------------------------------------------
int targetRow = GetRandomValue(0, RowCount - 1), targetCol = ColumnCount - 1;
int startRow  = GetRandomValue(0, RowCount - 1), startCol  = 0;

GameState gameState = StateIdle;

// ---------------------------------------------------------------------------
// The maze grid
// ---------------------------------------------------------------------------
cell mazeGrid[RowCount][ColumnCount];

// ---------------------------------------------------------------------------
// Algorithm containers and bookkeeping tables
// ---------------------------------------------------------------------------
std::stack<GridPos> generatorStack; // Maze generator backtracking stack
std::stack<GridPos> dfsStack;       // DFS search stack
std::queue<GridPos> bfsQueue;       // BFS search queue

std::priority_queue<PriorityNode, std::vector<PriorityNode>, std::greater<PriorityNode>> dijkstraQueue; // Dijkstra
std::priority_queue<PriorityNode, std::vector<PriorityNode>, std::greater<PriorityNode>> aStarQueue;    // A*

int bestCost[RowCount][ColumnCount];
int bestCostA[RowCount][ColumnCount];

GridPos parentBfs[RowCount][ColumnCount];
GridPos parentDfs[RowCount][ColumnCount];
GridPos parentDijkstra[RowCount][ColumnCount];
GridPos parentAStar[RowCount][ColumnCount];

bool bfsVisited[RowCount][ColumnCount] = {0};
bool dfsVisited[RowCount][ColumnCount] = {0};

std::vector<GridPos> pathBfs;
std::vector<GridPos> pathDfs;
std::vector<GridPos> pathDijkstra;
std::vector<GridPos> pathAStar;

// ---------------------------------------------------------------------------
// ResetMaze
//
// Clears every queue, stack, path and bookkeeping table so a new maze can be
// generated from a clean slate.
// ---------------------------------------------------------------------------
void ResetMaze() {
    while (!generatorStack.empty()) generatorStack.pop();
    while (!bfsQueue.empty()) bfsQueue.pop();
    while (!dfsStack.empty()) dfsStack.pop();

    // std::priority_queue has no clear(), so the safest reset is to assign a
    // freshly constructed empty queue.
    dijkstraQueue = std::priority_queue<PriorityNode, std::vector<PriorityNode>, std::greater<PriorityNode>>();
    aStarQueue    = std::priority_queue<PriorityNode, std::vector<PriorityNode>, std::greater<PriorityNode>>();

    pathBfs.clear();
    pathDfs.clear();
    pathDijkstra.clear();
    pathAStar.clear();

    for (int row = 0; row < RowCount; row++) {
        for (int col = 0; col < ColumnCount; col++) {
            // Restore the four walls and mark the cell as unvisited.
            mazeGrid[row][col].isVisited  = false;
            mazeGrid[row][col].wallRight  = true;
            mazeGrid[row][col].wallLeft   = true;
            mazeGrid[row][col].wallTop    = true;
            mazeGrid[row][col].wallBottom = true;

            bfsVisited[row][col] = false;
            dfsVisited[row][col] = false;
            bestCost[row][col]   = 99999;
            bestCostA[row][col]  = 99999;
        }
    }
}
