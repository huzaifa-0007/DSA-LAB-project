// ===========================================================================
// dijkstra.cpp - Dijkstra's shortest path algorithm.
//
// The priority queue always hands back the cell with the lowest known cost.
// Because every edge weighs 1, "lowest cost first" degenerates to "closest
// first", which is why the result matches BFS. The cost table (bestCost)
// is also the relaxation table: a neighbour is only updated when the new cost
// is strictly better than the cost already recorded.
// ===========================================================================

#include "dijkstra.h"

#include "core.h"

// ---------------------------------------------------------------------------
// Initialises the cost table with "infinity" (99999) and seeds the queue.
// ---------------------------------------------------------------------------
void InitDijkstra(int beginRow, int beginCol) {
    for (int row = 0; row < RowCount; row++) {
        for (int col = 0; col < ColumnCount; col++) {
            bestCost[row][col] = 99999;
        }
    }

    bestCost[beginRow][beginCol] = 0;
    dijkstraQueue.push({{beginRow, beginCol}, 0});
}

// ---------------------------------------------------------------------------
// Walks the parent table from the target back to the start, collecting the
// cells in reverse order (target first, start last).
// ---------------------------------------------------------------------------
static void ReconstructDijkstraPath() {
    int currRow = targetRow;
    int currCol = targetCol;

    while (currCol != startCol || currRow != startRow) {
        pathDijkstra.push_back({currRow, currCol});
        int prevRow = parentDijkstra[currRow][currCol].row;
        int prevCol = parentDijkstra[currRow][currCol].col;

        currRow = prevRow;
        currCol = prevCol;
    }

    pathDijkstra.push_back({startRow, startCol});
}

// ---------------------------------------------------------------------------
// Expands a single cell per call so the traversal can be animated.
// ---------------------------------------------------------------------------
void StepDijkstra() {
    // Direction vectors: index 0 = right, 1 = left, 2 = down, 3 = up.
    int dCol[4] = {0, 0, 1, -1};
    int dRow[4] = {1, -1, 0, 0};

    if (dijkstraQueue.empty()) {
        return;
    }

    GridPos currentPos = dijkstraQueue.top().position;
    int currentCost = dijkstraQueue.top().cost;
    dijkstraQueue.pop();

    // Every move costs 1, so the cost of any neighbour is the current cost + 1.
    int newCost = currentCost + 1;

    // Target reached: rebuild the path and hand control back to the UI.
    if (currentPos.row == targetRow && currentPos.col == targetCol) {
        ReconstructDijkstraPath();
        gameState = StateWaiting;
        return;
    }

    for (int i = 0; i < 4; i++) {
        int nextRow = currentPos.row + dRow[i];
        int nextCol = currentPos.col + dCol[i];

        // Relax the neighbour only if the new cost improves on the old one.
        if (nextRow >= 0 && nextRow < RowCount &&
            nextCol >= 0 && nextCol < ColumnCount &&
            newCost < bestCost[nextRow][nextCol]) {

            // A neighbour is only reachable when the shared wall is open.
            bool canMove = false;
            if (dCol[i] == 1 && mazeGrid[currentPos.row][currentPos.col].wallRight == false) canMove = true;
            else if (dCol[i] == -1 && mazeGrid[currentPos.row][currentPos.col].wallLeft == false) canMove = true;
            else if (dRow[i] == 1 && mazeGrid[currentPos.row][currentPos.col].wallBottom == false) canMove = true;
            else if (dRow[i] == -1 && mazeGrid[currentPos.row][currentPos.col].wallTop == false) canMove = true;

            if (canMove) {
                bestCost[nextRow][nextCol] = newCost;
                parentDijkstra[nextRow][nextCol] = {currentPos.row, currentPos.col};
                dijkstraQueue.push({{nextRow, nextCol}, newCost});
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Draws every cell that has a finite cost, then the final path on top.
// ---------------------------------------------------------------------------
void DrawDijkstra() {
    // Exploration wash: every cell that has a finite cost has been reached.
    for (int row = 0; row < RowCount; row++) {
        for (int col = 0; col < ColumnCount; col++) {
            if (bestCost[row][col] < 99999) {
                FillCell(row, col, 2.0f, 0.28f, Fade(DijkstraAccent, 0.32f));
            }
        }
    }

    // Final path: a faint halo under a solid tile.
    for (size_t i = 0; i < pathDijkstra.size(); i++) {
        int row = pathDijkstra[i].row;
        int col = pathDijkstra[i].col;
        FillCell(row, col, 3.0f, 0.35f, Fade(DijkstraAccent, 0.20f));
        FillCell(row, col, 6.0f, 0.40f, DijkstraAccent);
    }
}
