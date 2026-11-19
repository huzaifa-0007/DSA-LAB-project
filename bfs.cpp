// ===========================================================================
// bfs.cpp - Breadth-First Search.
//
// BFS uses a FIFO queue, so cells are expanded in order of their distance from
// the start. Every newly discovered cell stores its parent in parentBfs,
// which is what allows the path to be rebuilt backwards from the target.
// ===========================================================================

#include "bfs.h"

#include "core.h"

// ---------------------------------------------------------------------------
// Walks the parent table from the target back to the start, collecting the
// cells in reverse order (target first, start last).
// ---------------------------------------------------------------------------
static void ReconstructBfsPath() {
    int currRow = targetRow;
    int currCol = targetCol;

    while (currCol != startCol || currRow != startRow) {
        pathBfs.push_back({currRow, currCol});
        int prevRow = parentBfs[currRow][currCol].row;
        int prevCol = parentBfs[currRow][currCol].col;

        currRow = prevRow;
        currCol = prevCol;
    }

    pathBfs.push_back({startRow, startCol});
}

// ---------------------------------------------------------------------------
// Seeds the search at the start cell.
// ---------------------------------------------------------------------------
void InitBfs(int beginRow, int beginCol) {
    bfsVisited[beginRow][beginCol] = 1;
    bfsQueue.push({beginRow, beginCol});
}

// ---------------------------------------------------------------------------
// Expands a single cell per call so the traversal can be animated.
// ---------------------------------------------------------------------------
void StepBfs() {
    // Direction vectors: index 0 = right, 1 = left, 2 = down, 3 = up.
    int dCol[4] = {0, 0, 1, -1};
    int dRow[4] = {1, -1, 0, 0};

    if (bfsQueue.empty()) {
        return;
    }

    int currentRow = bfsQueue.front().row;
    int currentCol = bfsQueue.front().col;
    bfsQueue.pop();

    // Target reached: rebuild the path and hand control back to the UI.
    if (currentRow == targetRow && currentCol == targetCol) {
        ReconstructBfsPath();
        gameState = StateWaiting;
        return;
    }

    // Visit every neighbour that is reachable through an open wall.
    for (int i = 0; i < 4; i++) {
        int nextRow = currentRow + dRow[i];
        int nextCol = currentCol + dCol[i];

        if (nextRow >= 0 && nextRow < RowCount &&
            nextCol >= 0 && nextCol < ColumnCount &&
            bfsVisited[nextRow][nextCol] != 1) {

            // A neighbour is only reachable when the shared wall is open.
            if (dCol[i] == 1 && mazeGrid[currentRow][currentCol].wallRight == false) {
                bfsVisited[nextRow][nextCol] = 1;
                bfsQueue.push({nextRow, nextCol});
                parentBfs[nextRow][nextCol].row = currentRow;
                parentBfs[nextRow][nextCol].col = currentCol;
            } else if (dCol[i] == -1 && mazeGrid[currentRow][currentCol].wallLeft == false) {
                bfsVisited[nextRow][nextCol] = 1;
                bfsQueue.push({nextRow, nextCol});
                parentBfs[nextRow][nextCol].row = currentRow;
                parentBfs[nextRow][nextCol].col = currentCol;
            } else if (dRow[i] == -1 && mazeGrid[currentRow][currentCol].wallTop == false) {
                bfsVisited[nextRow][nextCol] = 1;
                bfsQueue.push({nextRow, nextCol});
                parentBfs[nextRow][nextCol] = {currentRow, currentCol};
            } else if (dRow[i] == 1 && mazeGrid[currentRow][currentCol].wallBottom == false) {
                bfsVisited[nextRow][nextCol] = 1;
                bfsQueue.push({nextRow, nextCol});
                parentBfs[nextRow][nextCol] = {currentRow, currentCol};
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Draws the explored region, then the final path on top of it.
// ---------------------------------------------------------------------------
void DrawBfs() {
    // Exploration wash: a soft tint on every cell the queue has touched.
    for (int row = 0; row < RowCount; row++) {
        for (int col = 0; col < ColumnCount; col++) {
            if (bfsVisited[row][col] == 1) {
                FillCell(row, col, 2.0f, 0.28f, Fade(BfsAccent, 0.32f));
            }
        }
    }

    // Final path: a faint halo under a solid tile, drawn last so it stays on
    // top of the exploration wash.
    for (size_t i = 0; i < pathBfs.size(); i++) {
        int row = pathBfs[i].row;
        int col = pathBfs[i].col;
        FillCell(row, col, 3.0f, 0.35f, Fade(BfsAccent, 0.20f));
        FillCell(row, col, 6.0f, 0.40f, BfsAccent);
    }
}
