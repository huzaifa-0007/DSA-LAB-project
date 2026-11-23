#include "maze.h"

#include "core.h"

// Seeds the generator at the given cell.

void InitMazeGeneration(int beginRow, int beginCol) {
    mazeGrid[beginRow][beginCol].isVisited = true;
    generatorStack.push({beginRow, beginCol});
}

void UpdateMazeStep() {
    std::vector<int> neighbors;  // Neighbour direction indices that are still valid

    if (generatorStack.empty()) {
        return;
    }

    // Current cell on top of the stack.
    int currentRow = generatorStack.top().row;
    int currentCol = generatorStack.top().col;

    // Direction vectors: index 0 = right, 1 = left, 2 = down, 3 = up.
    int dCol[4] = {0, 0, 1, -1};
    int dRow[4] = {1, -1, 0, 0};

    // Collect every in-bounds neighbour that has not been visited yet.
    for (int i = 0; i < 4; i++) {
        int nextRow = currentRow + dRow[i];
        int nextCol = currentCol + dCol[i];
        if (nextRow >= 0 && nextRow < RowCount &&
            nextCol >= 0 && nextCol < ColumnCount && mazeGrid[nextRow][nextCol].isVisited != true) {
            neighbors.push_back(i);
        }
    }

    // Dead end: backtrack and wait for the next frame.
    if (neighbors.empty()) {
        generatorStack.pop();
        return;
    }

    // Pick one neighbour at random and open the wall between the two cells.
    int randomIndex = GetRandomValue(0, neighbors.size() - 1);
    int chosenDir = neighbors[randomIndex];
    int nextRow = currentRow + dRow[chosenDir];
    int nextCol = currentCol + dCol[chosenDir];

    if (dCol[chosenDir] == 1) {
        mazeGrid[currentRow][currentCol].wallRight = false;
        mazeGrid[nextRow][nextCol].wallLeft = false;
    }
    if (dCol[chosenDir] == -1) {
        mazeGrid[currentRow][currentCol].wallLeft = false;
        mazeGrid[nextRow][nextCol].wallRight = false;
    }
    if (dRow[chosenDir] == 1) {
        mazeGrid[currentRow][currentCol].wallBottom = false;
        mazeGrid[nextRow][nextCol].wallTop = false;
    }
    if (dRow[chosenDir] == -1) {
        mazeGrid[currentRow][currentCol].wallTop = false;
        mazeGrid[nextRow][nextCol].wallBottom = false;
    }

    // The neighbour becomes the new current cell.
    mazeGrid[nextRow][nextCol].isVisited = true;
    generatorStack.push({nextRow, nextCol});
}

void DrawPath() {
    // Panel behind the grid.
    Rectangle panel = {(float)(BoardX - 6), (float)(BoardY - 6),
                       (float)(BoardWidth + 12), (float)(BoardHeight + 12)};
    DrawRectangleRounded(panel, 0.02f, 8, PanelBg);
    DrawRectangleRoundedLinesEx(panel, 0.02f, 8, 1.0f, PanelBorder);

    for (int row = 0; row < RowCount; row++) {
        for (int col = 0; col < ColumnCount; col++) {
            const cell &cellRef = mazeGrid[row][col];

            // Cells the generator has not reached yet stay empty, so the board
            // fills in as the maze is carved instead of showing a bare grid.
            if (cellRef.isVisited == false) continue;

            int x = CellX(col);
            int y = CellY(row);

            DrawRectangle(x, y, CellSize, CellSize, CellFloor);

            if (cellRef.wallTop == true) {
                DrawRectangle(x, y, CellSize + 1, 1, WallColor);
            }
            if (cellRef.wallLeft == true) {
                DrawRectangle(x, y, 1, CellSize + 1, WallColor);
            }
            if (col == ColumnCount - 1 && cellRef.wallRight == true) {
                DrawRectangle(x + CellSize, y, 1, CellSize + 1, WallColor);
            }
            if (row == RowCount - 1 && cellRef.wallBottom == true) {
                DrawRectangle(x, y + CellSize, CellSize + 1, 1, WallColor);
            }
        }
    }

}

void DrawMarkers() {
    if (mazeGrid[startRow][startCol].isVisited == true) {
        FillCell(startRow, startCol, 0.0f, 0.35f, Fade(StartColor, 0.18f));
        FillCell(startRow, startCol, 6.0f, 0.40f, StartColor);
    }

    if (mazeGrid[targetRow][targetCol].isVisited == true) {
        FillCell(targetRow, targetCol, 0.0f, 0.35f, Fade(TargetColor, 0.18f));
        FillCell(targetRow, targetCol, 6.0f, 0.40f, TargetColor);
    }
}
