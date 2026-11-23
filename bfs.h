#ifndef BFS_H
#define BFS_H

// Pushes the start cell into the queue and marks it visited.
void InitBfs(int beginRow, int beginCol);

// Expands one cell per call. Sets gameState back to StateWaiting once the
// target is reached and the path has been reconstructed.
void StepBfs();

// Paints the explored cells and the reconstructed path.
void DrawBfs();

#endif 
