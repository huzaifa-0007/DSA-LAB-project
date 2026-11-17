#ifndef UI_H
#define UI_H


// Loads the UI font. Must be called once after InitWindow(), because building
// a font uploads a texture and therefore needs a live OpenGL context.
void InitUI();

// Releases the font. Call before CloseWindow().
void UnloadUI();


void DrawUI();

#endif 
