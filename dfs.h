#ifndef DFS_H
#define DFS_H

// ===========================================================================
// dfs.h - Depth-First Search pathfinding and its visualisation.
//
// DFS dives as deep as possible down a single branch and only backtracks when
// it hits a dead end. It finds *a* path, but not necessarily the shortest one.
// ===========================================================================

// Pushes the start cell onto the DFS stack and marks it visited.
void InitDfs(int beginRow, int beginCol);

// Expands one cell per call. Sets gameState back to StateWaiting once the
// target is reached and the path has been reconstructed.
void StepDfs();

// Paints the explored cells and the reconstructed path.
void DrawDfs();

#endif // DFS_H
