

#include "dijkstra.h"

#include "core.h"


void InitDijkstra(int beginRow, int beginCol) {
    for (int row = 0; row < RowCount; row++) {
        for (int col = 0; col < ColumnCount; col++) {
            bestCost[row][col] = 99999;
        }
    }

    bestCost[beginRow][beginCol] = 0;
    dijkstraQueue.push({{beginRow, beginCol}, 0});
}


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

void StepDijkstra() {
    
    int dCol[4] = {0, 0, 1, -1};
    int dRow[4] = {1, -1, 0, 0};

    if (dijkstraQueue.empty()) {
        return;
    }

    GridPos currentPos = dijkstraQueue.top().position;
    int currentCost = dijkstraQueue.top().cost;
    dijkstraQueue.pop();

    
    int newCost = currentCost + 1;

    
    if (currentPos.row == targetRow && currentPos.col == targetCol) {
        ReconstructDijkstraPath();
        gameState = StateWaiting;
        return;
    }

    for (int i = 0; i < 4; i++) {
        int nextRow = currentPos.row + dRow[i];
        int nextCol = currentPos.col + dCol[i];

        
        if (nextRow >= 0 && nextRow < RowCount &&
            nextCol >= 0 && nextCol < ColumnCount &&
            newCost < bestCost[nextRow][nextCol]) {

            
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


void DrawDijkstra() {
    
    for (int row = 0; row < RowCount; row++) {
        for (int col = 0; col < ColumnCount; col++) {
            if (bestCost[row][col] < 99999) {
                FillCell(row, col, 2.0f, 0.28f, Fade(DijkstraAccent, 0.32f));
            }
        }
    }

    
    for (size_t i = 0; i < pathDijkstra.size(); i++) {
        int row = pathDijkstra[i].row;
        int col = pathDijkstra[i].col;
        FillCell(row, col, 3.0f, 0.35f, Fade(DijkstraAccent, 0.20f));
        FillCell(row, col, 6.0f, 0.40f, DijkstraAccent);
    }
}
