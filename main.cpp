// ===========================================================================
// main.cpp - Window setup and the application loop.
//
// Each frame is split into two halves:
//   1. UPDATE - advance whatever the state machine says is running. The
//      algorithms do a single step per frame, which is what makes the
//      traversal visible instead of instant.
//   2. DRAW   - render the maze, then the overlay of whichever search is
//      running or has just finished, then the UI.
// ===========================================================================

#include "astar.h"
#include "bfs.h"
#include "core.h"
#include "dfs.h"
#include "dijkstra.h"
#include "maze.h"
#include "ui.h"

#if defined(_WIN32)
// Declared here rather than pulling in <windows.h>: the Win32 headers define a
// GDI function called Rectangle, which collides with raylib's Rectangle type.
extern "C" __declspec(dllimport) long __stdcall DwmSetWindowAttribute(
    void *hwnd, unsigned int attribute, const void *value, unsigned int size);

// Paints the title bar dark so it matches the rest of the window. Silently does
// nothing on Windows versions that do not support it.
static void EnableDarkTitleBar() {
    void *window = GetWindowHandle();
    if (window == nullptr) return;

    const unsigned int DWMWA_USE_IMMERSIVE_DARK_MODE = 20;
    const unsigned int DWMWA_USE_IMMERSIVE_DARK_MODE_OLD = 19;
    int enabled = 1;

    long result = DwmSetWindowAttribute(window, DWMWA_USE_IMMERSIVE_DARK_MODE, &enabled, sizeof(enabled));
    if (result != 0) {
        DwmSetWindowAttribute(window, DWMWA_USE_IMMERSIVE_DARK_MODE_OLD, &enabled, sizeof(enabled));
    }
}
#endif

int main() {
    InitWindow(ScreenWidth, ScreenHeight, "Maze Visualizer");

    // Centre the window, but bias it up slightly: on a 768px-tall screen a
    // perfectly centred 684px client would push the bottom edge under the
    // taskbar.
    int posX = (GetMonitorWidth(0) - ScreenWidth) / 2;
    int posY = (GetMonitorHeight(0) - ScreenHeight) / 2 - 16;
    if (posY < 4) posY = 4;
    SetWindowPosition(posX, posY);

#if defined(_WIN32)
    EnableDarkTitleBar();
#endif

    SetTargetFPS(60);  // 60 frames per second, i.e. 60 algorithm steps per second

    InitUI();  // must come after InitWindow: it uploads a font texture

    while (!WindowShouldClose()) {
        // ---------------------------------------------------------------
        // UPDATE LOGIC
        // ---------------------------------------------------------------
        switch (gameState) {
            case StateGeneratingMaze:
                // Several generation steps per frame so the carving animation
                // does not crawl.
                UpdateMazeStep();
                UpdateMazeStep();
                UpdateMazeStep();
                UpdateMazeStep();
                if (generatorStack.empty()) gameState = StateWaiting;
                break;

            case StateBfsRunning:
                StepBfs();
                break;

            case StateDfsRunning:
                StepDfs();
                break;

            case StateDijkstraRunning:
                StepDijkstra();
                break;

            case StateAStarRunning:
                StepAStar();
                break;

            default:
                break;
        }

        // ---------------------------------------------------------------
        // DRAWING LOGIC
        // ---------------------------------------------------------------
        BeginDrawing();
        ClearBackground(AppBg);

        DrawPath();  // Base maze

        // The overlay is drawn while the search is running *and* afterwards,
        // so the final path stays visible once the target is found.
        if (gameState == StateBfsRunning || !pathBfs.empty()) DrawBfs();
        if (gameState == StateDfsRunning || !pathDfs.empty()) DrawDfs();
        if (gameState == StateDijkstraRunning || !pathDijkstra.empty()) DrawDijkstra();
        if (gameState == StateAStarRunning || !pathAStar.empty()) DrawAStar();

        DrawMarkers();  // Start/target stay readable above the wash
        DrawUI();
        EndDrawing();
    }

    UnloadUI();
    CloseWindow();
    return 0;
}
