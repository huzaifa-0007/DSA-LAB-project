@echo off
setlocal
rem Builds every .cpp in this folder into PathfindingVisualizer.exe.
rem The portable toolchain lives one level above the repository: workspace\_tools

set "HERE=%~dp0"
for %%I in ("%HERE%..\..\_tools") do set "TOOLS=%%~fI"
set "GPP=%TOOLS%\mingw64\bin\g++.exe"
set "RAYLIB=%TOOLS%\raylib-6.2_win64_mingw-w64"

rem Keep compiler scratch files inside the project. Some machines restrict
rem writes to the system temp folder, which makes g++ fail with
rem "Cannot create temporary file".
set "TMPDIR=%HERE%.tmp"
set "TMP=%HERE%.tmp"
set "TEMP=%HERE%.tmp"
if not exist "%TMPDIR%" mkdir "%TMPDIR%"

if not exist "%GPP%" (
    echo ERROR: g++ not found at "%GPP%"
    exit /b 1
)

echo Compiling...
"%GPP%" ^
    "%HERE%core.cpp" ^
    "%HERE%maze.cpp" ^
    "%HERE%bfs.cpp" ^
    "%HERE%dfs.cpp" ^
    "%HERE%dijkstra.cpp" ^
    "%HERE%astar.cpp" ^
    "%HERE%ui.cpp" ^
    "%HERE%main.cpp" ^
    -o "%HERE%PathfindingVisualizer.exe" ^
    -O2 -std=c++17 -Wall ^
    -I"%RAYLIB%\include" -L"%RAYLIB%\lib" ^
    -lraylib -lopengl32 -lgdi32 -lwinmm -ldwmapi ^
    -static -static-libgcc -static-libstdc++

if errorlevel 1 (
    echo.
    echo BUILD FAILED
    exit /b 1
)

echo.
echo Build OK: %HERE%PathfindingVisualizer.exe
endlocal
