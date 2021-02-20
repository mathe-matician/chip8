#ifndef SDLDEFS_H
#define SDLDEFS_H
int initSDL();
void CleanUpSDL();
void GameLoop();
void DrawPixel(SDL_Renderer* a_renderer);
#endif //SDLDEFS_H