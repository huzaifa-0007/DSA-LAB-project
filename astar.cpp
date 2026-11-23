

#include "astar.h"

#include "core.h"

int ManhattanDistance(int currentRow, int currentCol, int targetBaris, int targetKolom) {
    return std::abs(currentRow - targetBaris) + std::abs(currentCol - targetKolom);
}

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

void StepAStar() {
    
    int dCol[4] = {0, 0, 1, -1};
    int dRow[4] = {1, -1, 0, 0};

    if (aStarQueue.empty()) {
        return;
    }

    GridPos currentPos = aStarQueue.top().position;
    aStarQueue.pop();

    
    int currentG = bestCostA[currentPos.row][currentPos.col];
    int newCost = currentG + 1;

    
    if (currentPos.row == targetRow && currentPos.col == targetCol) {
        ReconstructAStarPath();
        gameState = StateWaiting;
        return;
    }

    for (int i = 0; i < 4; i++) {
        int nextRow = currentPos.row + dRow[i];
        int nextCol = currentPos.col + dCol[i];

        
        if (nextRow >= 0 && nextRow < RowCount &&
            nextCol >= 0 && nextCol < ColumnCount &&
            newCost < bestCostA[nextRow][nextCol]) {

            
            bool canMove = false;
            if (dCol[i] == 1 && mazeGrid[currentPos.row][currentPos.col].wallRight == false) canMove = true;
            else if (dCol[i] == -1 && mazeGrid[currentPos.row][currentPos.col].wallLeft == false) canMove = true;
            else if (dRow[i] == 1 && mazeGrid[currentPos.row][currentPos.col].wallBottom == false) canMove = true;
            else if (dRow[i] == -1 && mazeGrid[currentPos.row][currentPos.col].wallTop == false) canMove = true;

            if (canMove) {
                bestCostA[nextRow][nextCol] = newCost;
                parentAStar[nextRow][nextCol] = {currentPos.row, currentPos.col};

                
                int H_n = ManhattanDistance(nextRow, nextCol, targetRow, targetCol);
                int F_n = newCost + H_n;
                aStarQueue.push({{nextRow, nextCol}, F_n});
            } else {
                continue;
            }
        }
    }
}

void DrawAStar() {
    
    for (int row = 0; row < RowCount; row++) {
        for (int col = 0; col < ColumnCount; col++) {
            if (bestCostA[row][col] < 99999) {
                FillCell(row, col, 2.0f, 0.28f, Fade(AStarAccent, 0.32f));
            }
        }
    }

    
    for (size_t i = 0; i < pathAStar.size(); i++) {
        int row = pathAStar[i].row;
        int col = pathAStar[i].col;
        FillCell(row, col, 3.0f, 0.35f, Fade(AStarAccent, 0.20f));
        FillCell(row, col, 6.0f, 0.40f, AStarAccent);
    }
}
