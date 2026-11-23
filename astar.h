#ifndef ASTAR_H
#define ASTAR_H

// ===========================================================================
// astar.h - A* (A-Star) pathfinding and its visualisation.
//
// A* orders its priority queue by f(n) = g(n) + h(n):
//   g(n) - cost of the shortest known path from the start to n
//   h(n) - estimated cost from n to the target (the heuristic)
// On this grid movement is four-directional and uniform, so Manhattan distance
// is an admissible heuristic and A* still returns a shortest path while
// exploring considerably fewer cells than Dijkstra.
// ===========================================================================

// Manhattan distance between two grid cells.
int ManhattanDistance(int currentRow, int currentCol, int goalRow, int goalCol);

// Fills the cost and parent tables, then seeds the queue with f(start).
void InitAStar(int beginRow, int beginCol);

// Expands one cell per call. Sets gameState back to StateWaiting once the
// target is reached and the path has been reconstructed.
void StepAStar();

// Paints the explored cells and the reconstructed path.
void DrawAStar();

#endif // ASTAR_H
