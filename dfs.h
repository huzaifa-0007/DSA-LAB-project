#ifndef DFS_H
#define DFS_H

// Pushes the start cell onto the DFS stack and marks it visited.
void InitDfs(int beginRow, int beginCol);

// Expands one cell per call. Sets gameState back to StateWaiting once the
// target is reached and the path has been reconstructed.
void StepDfs();

// Paints the explored cells and the reconstructed path.
void DrawDfs();

#endif
