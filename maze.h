#ifndef MAZE_H
#define MAZE_H


// Marks the start cell as visited and seeds the generator stack.
void InitMazeGeneration(int beginRow, int beginCol);

// Advances the generator by one cell. Called several times per frame so the
// carve-out animation stays smooth.
void UpdateMazeStep();

// Draws the maze surface (panel, cell floors and walls).
void DrawPath();

// Draws the start and target markers. Called after the search overlay so the
// markers are never painted over.
void DrawMarkers();

#endif
