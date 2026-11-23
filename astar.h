#ifndef ASTAR_H
#define ASTAR_H




int ManhattanDistance(int currentRow, int currentCol, int goalRow, int goalCol);


void InitAStar(int beginRow, int beginCol);


void StepAStar();


void DrawAStar();

#endif 
