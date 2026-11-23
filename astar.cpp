// ===========================================================================
// astar.cpp - A* (A-Star) search.
//
// The priority queue is ordered by f(n) = g(n) + h(n). g(n) is read back from
// bestCostA (the best cost found so far) and h(n) is the Manhattan
// distance to the target. Because the heuristic never overestimates the true
// remaining distance, the first time the target is popped the path is optimal.
// ===========================================================================

#include "astar.h"

#include "core.h"

// ---------------------------------------------------------------------------
// Manhattan distance: |dRow| + |dCol|.
//
// Movement is limited to the four cardinal directions, so this is the exact
// number of steps needed on an empty grid.
// ---------------------------------------------------------------------------
int ManhattanDistance(int currentRow, int currentCol, int targetBaris, int targetKolom) {
    return std::abs(currentRow - targetBaris) + std::abs(currentCol - targetKolom);
}

// ---------------------------------------------------------------------------
// Initialises the cost table with "infinity" (99999), clears the parent table
// and seeds the queue with f(start) = h(start).
// ---------------------------------------------------------------------------
void InitAStar(int beginRow, int beginCol) {
    for (int row = 0; row < RowCount; row++) {
        for (int col = 0; col < ColumnCount; col++) {
            bestCostA[row][col] = 99999;
            parentAStar[row][col] = {-1, -1};
        }
    }

    bestCostA[beginRow][beginCol] = 0;
    int H_n = ManhattanDistance(beginRow, beginCol, targetRow, targetCol);
    int F_n = H_n;
    aStarQueue.push({{beginRow, beginCol}, F_n});
}

// ---------------------------------------------------------------------------
// Walks the parent table from the target back to the start, collecting the
// cells in reverse order (target first, start last).
// ---------------------------------------------------------------------------
static void ReconstructAStarPath() {
    int currRow = targetRow;
    int currCol = targetCol;

    while (currCol != startCol || currRow != startRow) {
        pathAStar.push_back({currRow, currCol});
        int prevRow = parentAStar[currRow][currCol].row;
        int prevCol = parentAStar[currRow][currCol].col;

        currRow = prevRow;
        currCol = prevCol;
    }

    pathAStar.push_back({startRow, startCol});
}

// ---------------------------------------------------------------------------
// Expands a single cell per call so the traversal can be animated.
// ---------------------------------------------------------------------------
void StepAStar() {
    // Direction vectors: index 0 = right, 1 = left, 2 = down, 3 = up.
    int dCol[4] = {0, 0, 1, -1};
    int dRow[4] = {1, -1, 0, 0};

    if (aStarQueue.empty()) {
        return;
    }

    GridPos currentPos = aStarQueue.top().position;
    aStarQueue.pop();

    // g(current) is the best cost recorded for the popped cell.
    int currentG = bestCostA[currentPos.row][currentPos.col];
    int newCost = currentG + 1;

    // Target reached: rebuild the path and hand control back to the UI.
    if (currentPos.row == targetRow && currentPos.col == targetCol) {
        ReconstructAStarPath();
        gameState = StateWaiting;
        return;
    }

    for (int i = 0; i < 4; i++) {
        int nextRow = currentPos.row + dRow[i];
        int nextCol = currentPos.col + dCol[i];

        // Relax the neighbour only if the new g value improves on the old one.
        if (nextRow >= 0 && nextRow < RowCount &&
            nextCol >= 0 && nextCol < ColumnCount &&
            newCost < bestCostA[nextRow][nextCol]) {

            // A neighbour is only reachable when the shared wall is open.
            bool canMove = false;
            if (dCol[i] == 1 && mazeGrid[currentPos.row][currentPos.col].wallRight == false) canMove = true;
            else if (dCol[i] == -1 && mazeGrid[currentPos.row][currentPos.col].wallLeft == false) canMove = true;
            else if (dRow[i] == 1 && mazeGrid[currentPos.row][currentPos.col].wallBottom == false) canMove = true;
            else if (dRow[i] == -1 && mazeGrid[currentPos.row][currentPos.col].wallTop == false) canMove = true;

            if (canMove) {
                bestCostA[nextRow][nextCol] = newCost;
                parentAStar[nextRow][nextCol] = {currentPos.row, currentPos.col};

                // Push with the estimated total cost, not the raw g value.
                int H_n = ManhattanDistance(nextRow, nextCol, targetRow, targetCol);
                int F_n = newCost + H_n;
                aStarQueue.push({{nextRow, nextCol}, F_n});
            } else {
                continue;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Draws every cell that has a finite cost, then the final path on top.
// ---------------------------------------------------------------------------
void DrawAStar() {
    // Exploration wash: every cell that has a finite cost has been reached.
    for (int row = 0; row < RowCount; row++) {
        for (int col = 0; col < ColumnCount; col++) {
            if (bestCostA[row][col] < 99999) {
                FillCell(row, col, 2.0f, 0.28f, Fade(AStarAccent, 0.32f));
            }
        }
    }

    // Final path: a faint halo under a solid tile.
    for (size_t i = 0; i < pathAStar.size(); i++) {
        int row = pathAStar[i].row;
        int col = pathAStar[i].col;
        FillCell(row, col, 3.0f, 0.35f, Fade(AStarAccent, 0.20f));
        FillCell(row, col, 6.0f, 0.40f, AStarAccent);
    }
}
