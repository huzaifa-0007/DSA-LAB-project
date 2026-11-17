// ===========================================================================
// ui.cpp - Toolbar, buttons, status pill and the shared UI fonts.
// The buttons are the only place that changes gameState, which keeps the state
// machine honest: a search can only start from StateWaiting, i.e. once the
// maze has finished generating.
// ===========================================================================

#include "ui.h"

#include <cstdio>

#include "astar.h"
#include "bfs.h"
#include "core.h"
#include "dfs.h"
#include "dijkstra.h"
#include "maze.h"

namespace {

using PQueue = std::priority_queue<PriorityNode, std::vector<PriorityNode>, std::greater<PriorityNode>>;

// Fonts
//
// Two rasterisation sizes are loaded so that every piece of text is drawn at
// roughly the size it was rasterised at. Drawing a 64px atlas down to 13px
// samples far too few texels and the glyph edges come out jagged.

Font g_fontSmall;  // 13px text: buttons and the status pill
Font g_fontLarge;  // 19px text: the brand
bool g_fontsLoaded = false;

const float TRACKING = 0.2f;

Vector2 Measure(Font font, const char *text, float size) {
    return MeasureTextEx(font, text, size, TRACKING);
}

void DrawTextAt(Font font, const char *text, float x, float y, float size, Color color) {
    DrawTextEx(font, text, {x, y}, size, TRACKING, color);
}

void DrawTextCentered(Font font, const char *text, Rectangle box, float size, Color color) {
    Vector2 m = Measure(font, text, size);
    DrawTextEx(font, text,
               {box.x + (box.width - m.x) * 0.5f, box.y + (box.height - m.y) * 0.5f},
               size, TRACKING, color);
}
// Local shades that only the toolbar needs
const Color ButtonBase     = { 19, 24, 33, 255 };
const Color ButtonDisabled = { 16, 20, 27, 255 };
const Color BorderDisabled = { 33, 40, 52, 255 };
const Color InkOnAccent    = { 10, 13, 18, 255 };

// One toolbar button.
// Three visual states matter: disabled (greyed out), idle (accent outline that
// brightens on hover) and running (filled with the accent colour).
struct ToolButton {
    Rectangle rec;
    const char *label;
    Color accent;
    bool enabled;
    bool running;
    bool finished;
    float hover = 0.0f;
    float press = 0.0f;

    bool Hovered() const {
        return enabled && CheckCollisionPointRec(GetMousePosition(), rec);
    }

    bool Clicked() const {
        return Hovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

    // Eases the hover and press values toward their targets so the buttons
    // feel responsive instead of snapping.
    void Update() {
        float hoverTarget = Hovered() ? 1.0f : 0.0f;
        hover += (hoverTarget - hover) * 0.30f;

        float pressTarget = (Hovered() && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) ? 1.0f : 0.0f;
        press += (pressTarget - press) * 0.35f;
    }

    void Draw() const {
        Rectangle r = {rec.x, rec.y - hover * 1.5f + press * 1.5f, rec.width, rec.height};

        Color fill;
        Color border;
        Color text;

        if (!enabled) {
            fill = ButtonDisabled;
            border = BorderDisabled;
            text = TextDim;
        } else if (running) {
            fill = accent;
            border = accent;
            text = InkOnAccent;
        } else if (finished) {
            fill = ColorLerp(ButtonBase, accent, 0.10f);
            border = Fade(accent, 0.70f);
            text = accent;
        } else {
            fill = ColorLerp(ButtonBase, accent, 0.14f * hover);
            border = Fade(accent, 0.32f + 0.45f * hover);
            text = ColorLerp(accent, TextPrimary, 0.10f + 0.10f * hover);
        }

        DrawRectangleRounded(r, 0.42f, 10, fill);
        DrawRectangleRoundedLinesEx(r, 0.42f, 10, 1.0f, border);
        DrawTextCentered(g_fontSmall, label, r, 13.0f, text);
    }
};


// Status pill: a coloured dot plus a short message, anchored top-right.

void DrawStatusPill(const char *text, Color accent, bool pulsing) {
    const float size = 12.0f;
    const float height = 24.0f;

    Vector2 m = Measure(g_fontSmall, text, size);
    float width = m.x + 34.0f;
    Rectangle r = {(float)(ScreenWidth - 16) - width, 12.0f, width, height};

    DrawRectangleRounded(r, 0.5f, 12, Fade(accent, 0.13f));
    DrawRectangleRoundedLinesEx(r, 0.5f, 12, 1.0f, Fade(accent, 0.42f));

    // The dot pulses while something is running so the UI looks live.
    float pulse = 1.0f;
    if (pulsing) {
        pulse = 0.55f + 0.45f * (0.5f + 0.5f * sinf((float)GetTime() * 5.0f));
    }
    DrawCircleV({r.x + 13.0f, r.y + height * 0.5f}, 3.2f, Fade(accent, pulse));

    DrawTextEx(g_fontSmall, text, {r.x + 22.0f, r.y + (height - m.y) * 0.5f}, size, TRACKING, accent);
}
// Resets the per-algorithm UI state before a search starts.
void ClearAllPaths() {
    pathBfs.clear();
    pathDfs.clear();
    pathDijkstra.clear();
    pathAStar.clear();
}

}  // namespace

void InitUI() {
    // Segoe UI ships with every supported Windows version; the bitmap fallback
    // keeps the application usable if the font file is missing.
    const char *candidates[] = {
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
    };

    const char *chosen = nullptr;
    for (const char *path : candidates) {
        if (FileExists(path)) {
            chosen = path;
            break;
        }
    }

    if (chosen != nullptr) {
        Font small = LoadFontEx(chosen, 16, nullptr, 0);
        Font large = LoadFontEx(chosen, 22, nullptr, 0);

        if (small.texture.id != 0 && large.texture.id != 0) {
            SetTextureFilter(small.texture, TEXTURE_FILTER_BILINEAR);
            SetTextureFilter(large.texture, TEXTURE_FILTER_BILINEAR);
            g_fontSmall = small;
            g_fontLarge = large;
            g_fontsLoaded = true;
        }
    }

    if (!g_fontsLoaded) {
        g_fontSmall = GetFontDefault();
        g_fontLarge = GetFontDefault();
    }
}

void UnloadUI() {
    if (g_fontsLoaded) {
        UnloadFont(g_fontSmall);
        UnloadFont(g_fontLarge);
        g_fontsLoaded = false;
    }
}

void DrawUI() {
    //Toolbar background
    DrawRectangleGradientV(0, 0, ScreenWidth, HeaderHeight, HeaderBgTop, HeaderBg);
    DrawRectangle(0, HeaderHeight - 1, ScreenWidth, 1, HeaderBorder);

    //Brand 
    const float brandSize = 19.0f;
    const float gapAfterBrand = 7.0f;
    float brandX = 18.0f;

    DrawTextAt(g_fontLarge, "MAZE", brandX, 13.0f, brandSize, BfsAccent);
    brandX += Measure(g_fontLarge, "MAZE", brandSize).x + gapAfterBrand;
    DrawTextAt(g_fontLarge, "VISUALIZER", brandX, 13.0f, brandSize, TextPrimary);

    // Buttons
    const int buttonW = 150;
    const int buttonH = 34;
    const int gap = 10;
    const int totalW = 5 * buttonW + 4 * gap;
    const int startX = (ScreenWidth - totalW) / 2;
    const int buttonY = 44;

    bool ready = (gameState == StateIdle || gameState == StateWaiting);

    bool bfsRunning = (gameState == StateBfsRunning);
    bool dfsRunning = (gameState == StateDfsRunning);
    bool dijkstraRunning = (gameState == StateDijkstraRunning);
    bool aStarRunning = (gameState == StateAStarRunning);

    // Which search, if any, is currently displayed as finished.
    bool bfsFinished = (gameState == StateWaiting && !pathBfs.empty());
    bool dfsFinished = (gameState == StateWaiting && !pathDfs.empty());
    bool dijkstraFinished = (gameState == StateWaiting && !pathDijkstra.empty());
    bool aStarFinished = (gameState == StateWaiting && !pathAStar.empty());

    ToolButton btnGenerate = {{(float)startX, (float)buttonY, (float)buttonW, (float)buttonH},
                              "Generate Maze", GenericAccent, ready, false, false};
    ToolButton btnBfs = {{(float)(startX + (buttonW + gap)), (float)buttonY, (float)buttonW, (float)buttonH},
                         "Start BFS", BfsAccent, gameState == StateWaiting, bfsRunning, bfsFinished};
    ToolButton btnDfs = {{(float)(startX + 2 * (buttonW + gap)), (float)buttonY, (float)buttonW, (float)buttonH},
                         "Start DFS", DfsAccent, gameState == StateWaiting, dfsRunning, dfsFinished};
    ToolButton btnDijkstra = {{(float)(startX + 3 * (buttonW + gap)), (float)buttonY, (float)buttonW, (float)buttonH},
                              "Start Dijkstra", DijkstraAccent, gameState == StateWaiting, dijkstraRunning, dijkstraFinished};
    ToolButton btnAStar = {{(float)(startX + 4 * (buttonW + gap)), (float)buttonY, (float)buttonW, (float)buttonH},
                           "Start A*", AStarAccent, gameState == StateWaiting, aStarRunning, aStarFinished};

    btnGenerate.Update();
    btnBfs.Update();
    btnDfs.Update();
    btnDijkstra.Update();
    btnAStar.Update();

    btnGenerate.Draw();
    btnBfs.Draw();
    btnDfs.Draw();
    btnDijkstra.Draw();
    btnAStar.Draw();

    // A pointing hand over any clickable button makes the toolbar feel native.
    bool anyHovered = btnGenerate.Hovered() || btnBfs.Hovered() || btnDfs.Hovered() ||
                      btnDijkstra.Hovered() || btnAStar.Hovered();
    SetMouseCursor(anyHovered ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);

    // Button actions
    if (btnGenerate.Clicked()) {
        ResetMaze();
        startRow = GetRandomValue(0, RowCount - 1);
        targetRow = GetRandomValue(0, RowCount - 1);
        InitMazeGeneration(startRow, startCol);
        gameState = StateGeneratingMaze;
    }

    if (btnBfs.Clicked()) {
        ClearAllPaths();
        for (int row = 0; row < RowCount; row++)
            for (int col = 0; col < ColumnCount; col++) bfsVisited[row][col] = false;
        while (!bfsQueue.empty()) bfsQueue.pop();

        InitBfs(startRow, startCol);
        gameState = StateBfsRunning;
    }

    if (btnDfs.Clicked()) {
        ClearAllPaths();
        for (int row = 0; row < RowCount; row++)
            for (int col = 0; col < ColumnCount; col++) dfsVisited[row][col] = false;
        while (!dfsStack.empty()) dfsStack.pop();

        InitDfs(startRow, startCol);
        gameState = StateDfsRunning;
    }

    if (btnDijkstra.Clicked()) {
        ClearAllPaths();
        // std::priority_queue has no clear(), so assign a fresh empty one.
        dijkstraQueue = PQueue();

        InitDijkstra(startRow, startCol);
        gameState = StateDijkstraRunning;
    }

    if (btnAStar.Clicked()) {
        ClearAllPaths();
        aStarQueue = PQueue();

        InitAStar(startRow, startCol);
        gameState = StateAStarRunning;
    }

    //Status pill
    const char *statusText = "Ready";
    Color statusColor = TextMuted;
    bool pulsing = false;
    char buffer[128];

    switch (gameState) {
        case StateIdle:
            statusText = "Ready";
            statusColor = TextMuted;
            break;
        case StateGeneratingMaze:
            statusText = "Building maze";
            statusColor = GenericAccent;
            pulsing = true;
            break;
        case StateBfsRunning:
            statusText = "BFS searching";
            statusColor = BfsAccent;
            pulsing = true;
            break;
        case StateDfsRunning:
            statusText = "DFS searching";
            statusColor = DfsAccent;
            pulsing = true;
            break;
        case StateDijkstraRunning:
            statusText = "Dijkstra searching";
            statusColor = DijkstraAccent;
            pulsing = true;
            break;
        case StateAStarRunning:
            statusText = "A* searching";
            statusColor = AStarAccent;
            pulsing = true;
            break;
        case StateWaiting: {
            const char *name = nullptr;
            size_t cells = 0;
            Color accent = TextMuted;

            if (!pathBfs.empty()) { name = "BFS"; cells = pathBfs.size(); accent = BfsAccent; }
            else if (!pathDfs.empty()) { name = "DFS"; cells = pathDfs.size(); accent = DfsAccent; }
            else if (!pathDijkstra.empty()) { name = "Dijkstra"; cells = pathDijkstra.size(); accent = DijkstraAccent; }
            else if (!pathAStar.empty()) { name = "A*"; cells = pathAStar.size(); accent = AStarAccent; }

            if (name != nullptr) {
                snprintf(buffer, sizeof(buffer), "%s done - %d cells", name, (int)cells);
                statusText = buffer;
                statusColor = accent;
            } else {
                statusText = "Maze ready";
                statusColor = TargetColor;
            }
            break;
        }
    }

    DrawStatusPill(statusText, statusColor, pulsing);

    // --- Empty-board hint --------------------------------------------------
    // With nothing carved the board is just a dark panel, so tell the user
    // what to do next.
    if (gameState == StateIdle) {
        const char *line1 = "Press Generate Maze to build a maze";
        const char *line2 = "then pick an algorithm to watch it search";

        Vector2 m1 = Measure(g_fontSmall, line1, 15.0f);
        Vector2 m2 = Measure(g_fontSmall, line2, 12.5f);

        float centreY = BoardY + BoardHeight * 0.5f;
        DrawTextEx(g_fontSmall, line1,
                   {(ScreenWidth - m1.x) * 0.5f, centreY - m1.y - 4.0f},
                   15.0f, TRACKING, TextMuted);
        DrawTextEx(g_fontSmall, line2,
                   {(ScreenWidth - m2.x) * 0.5f, centreY + 4.0f},
                   12.5f, TRACKING, Fade(TextMuted, 0.65f));
    }
}
