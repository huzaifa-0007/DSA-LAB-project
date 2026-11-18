// ===========================================================================
// dfs.cpp - Depth-First Search.
//
// The stack always holds the current cell on top. Each call looks at the open
// neighbours of that cell, picks one at random and pushes it, so the search
// walks a single corridor to its end before backing up.
// ===========================================================================

#include "dfs.h"

#include "core.h"

// ---------------------------------------------------------------------------
// Walks the parent table from the target back to the start, collecting the
// cells in reverse order (target first, start last).
// ---------------------------------------------------------------------------
static void ReconstructDfsPath() {
    int currRow = targetRow;
    int currCol = targetCol;

    while (currRow != startRow || currCol != startCol) {
        pathDfs.push_back({currRow, currCol});
        int prevRow = parentDfs[currRow][currCol].row;
        int prevCol = parentDfs[currRow][currCol].col;

        currRow = prevRow;
        currCol = prevCol;
    }
    pathDfs.push_back({startRow, startCol});
}

// ---------------------------------------------------------------------------
// Seeds the search at the start cell.
// ---------------------------------------------------------------------------
void InitDfs(int beginRow, int beginCol) {
    dfsVisited[beginRow][beginCol] = 1;
    dfsStack.push({beginRow, beginCol});
}

// ---------------------------------------------------------------------------
// Expands a single cell per call so the traversal can be animated.
// ---------------------------------------------------------------------------
void StepDfs() {
    if (dfsStack.empty()) {
        return;
    }

    std::vector<int> neighbors;  // Direction indices of reachable neighbours

    int currentRow = dfsStack.top().row;
    int currentCol = dfsStack.top().col;

    // Target reached: rebuild the path and hand control back to the UI.
    if (currentRow == targetRow && currentCol == targetCol) {
        ReconstructDfsPath();
        gameState = StateWaiting;
        return;
    }

    // Direction vectors: index 0 = right, 1 = left, 2 = down, 3 = up.
    int dCol[4] = {0, 0, 1, -1};
    int dRow[4] = {1, -1, 0, 0};

    // Collect every unvisited neighbour that can be reached through an open
    // wall.
    for (int i = 0; i < 4; i++) {
        int nextRow = currentRow + dRow[i];
        int nextCol = currentCol + dCol[i];

        if (nextRow >= 0 && nextRow < RowCount &&
            nextCol >= 0 && nextCol < ColumnCount &&
            dfsVisited[nextRow][nextCol] != 1) {

            if (dCol[i] == 1 && mazeGrid[currentRow][currentCol].wallRight == false) {
                neighbors.push_back(i);
            }
            if (dCol[i] == -1 && mazeGrid[currentRow][currentCol].wallLeft == false) {
                neighbors.push_back(i);
            }
            if (dRow[i] == 1 && mazeGrid[currentRow][currentCol].wallBottom == false) {
                neighbors.push_back(i);
            }
            if (dRow[i] == -1 && mazeGrid[currentRow][currentCol].wallTop == false) {
                neighbors.push_back(i);
            }
        }
    }

    // Dead end: backtrack one cell and wait for the next frame.
    if (neighbors.empty()) {
        dfsStack.pop();
        return;
    }

    // Push a randomly chosen neighbour, keeping the branch going deeper.
    int randomIndex = GetRandomValue(0, neighbors.size() - 1);
    int chosenDir = neighbors[randomIndex];
    int nextRow = currentRow + dRow[chosenDir];
    int nextCol = currentCol + dCol[chosenDir];

    if (dCol[chosenDir] == 1 && mazeGrid[currentRow][currentCol].wallRight == false) {
        dfsVisited[nextRow][nextCol] = 1;
        dfsStack.push({nextRow, nextCol});
        parentDfs[nextRow][nextCol].row = currentRow;
        parentDfs[nextRow][nextCol].col = currentCol;
    }
    if (dCol[chosenDir] == -1 && mazeGrid[currentRow][currentCol].wallLeft == false) {
        dfsVisited[nextRow][nextCol] = 1;
        dfsStack.push({nextRow, nextCol});
        parentDfs[nextRow][nextCol].row = currentRow;
        parentDfs[nextRow][nextCol].col = currentCol;
    }
    if (dRow[chosenDir] == 1 && mazeGrid[currentRow][currentCol].wallBottom == false) {
        dfsVisited[nextRow][nextCol] = 1;
        dfsStack.push({nextRow, nextCol});
        parentDfs[nextRow][nextCol].row = currentRow;
        parentDfs[nextRow][nextCol].col = currentCol;
    }
    if (dRow[chosenDir] == -1 && mazeGrid[currentRow][currentCol].wallTop == false) {
        dfsVisited[nextRow][nextCol] = 1;
        dfsStack.push({nextRow, nextCol});
        parentDfs[nextRow][nextCol].row = currentRow;
        parentDfs[nextRow][nextCol].col = currentCol;
    }
}

// ---------------------------------------------------------------------------
// Draws the explored region, then the final path on top of it.
// ---------------------------------------------------------------------------
void DrawDfs() {
    // Exploration wash: a soft tint on every cell the stack has touched.
    for (int row = 0; row < RowCount; row++) {
        for (int col = 0; col < ColumnCount; col++) {
            if (dfsVisited[row][col] == 1) {
                FillCell(row, col, 2.0f, 0.28f, Fade(DfsAccent, 0.32f));
            }
        }
    }

    // Final path: a faint halo under a solid tile.
    for (size_t i = 0; i < pathDfs.size(); i++) {
        int row = pathDfs[i].row;
        int col = pathDfs[i].col;
        FillCell(row, col, 3.0f, 0.35f, Fade(DfsAccent, 0.20f));
        FillCell(row, col, 6.0f, 0.40f, DfsAccent);
    }
}
