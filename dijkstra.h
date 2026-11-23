#ifndef DIJKSTRA_H
#define DIJKSTRA_H

// ===========================================================================
// dijkstra.h - Dijkstra's shortest path algorithm and its visualisation.
//
// Every move currently costs 1, so on this grid Dijkstra behaves exactly like
// BFS. It is implemented with a priority queue on purpose: that is the
// structure needed once weighted terrain is added.
// ===========================================================================

// Fills the cost table, then seeds the priority queue with the start cell.
void InitDijkstra(int beginRow, int beginCol);

// Expands one cell per call. Sets gameState back to StateWaiting once the
// target is reached and the path has been reconstructed.
void StepDijkstra();

// Paints the explored cells and the reconstructed path.
void DrawDijkstra();

#endif // DIJKSTRA_H
