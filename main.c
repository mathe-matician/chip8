#include "chip8.h"
#include <stdlib.h>
#include "SDL2/SDL.h"
#include "SDLdefs.h"

CHIP8_t Chip8;
SDL_Window* g_Window = NULL;
SDL_Renderer *g_renderer = NULL;
SDL_Surface *g_surface = NULL;
SDL_Texture *g_texture = NULL;
SDL_Event g_event;
 
int main(int argc, char **argv) 
{
  if (initSDL()) {
    ERROR_MSG;
    return -1;
  }
  //initInput();

  Chip8.EmulateCycle = emulateCycleImp;
  Chip8.SetKeys = setKeysImp;
  Chip8.LoadProgram = loadProgramImp;
  Chip8.init = initSystemImp;

  Chip8.init(&Chip8);
  Chip8.LoadProgram(&Chip8, "IBM_logo");

  GameLoop();

  CleanUpSDL(g_Window);

  return 0;
}

void GameLoop() {
  int running = 1;
  while(running)
  {
    //Chip8.EmulateCycle(&Chip8);
    //consume events
	  while(SDL_PollEvent(&g_event)){
		  if(g_event.type == SDL_QUIT){
			  running=0;
		  }
	  }

    SDL_Delay(16);
    if(!(Chip8.V[15] & 0)) {
      DrawPixel(g_renderer);
      SDL_UpdateWindowSurface(g_Window);
    }
 
    Chip8.SetKeys(&Chip8);	
  }
}

int initSDL() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
    return 3;
  }

  g_Window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 256, 128, SDL_WINDOW_RESIZABLE);
  g_surface = SDL_GetWindowSurface(g_Window);
  //g_surface = SDL_CreateRGBSurfaceWithFormat(0, 2, 3, 8, SDL_PIXELFORMAT_INDEX8);
    if (g_surface == NULL) {
        SDL_Log("SDL_CreateRGBSurfaceWithFormat() failed: %s", SDL_GetError());
        exit(1);
    }
  g_renderer = SDL_CreateSoftwareRenderer(g_surface);
  if (!g_renderer) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Render creation for surface fail : %s\n",SDL_GetError());
    return 1;
  }

  SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0x00, 0x00);
  SDL_RenderClear(g_renderer);
  SDL_ShowWindow(g_Window);

  return 0;
}

void DrawPixel(SDL_Renderer* a_renderer) {
    SDL_Rect l_rect, l_area;

    SDL_RenderGetViewport(g_renderer, &l_area);
    SDL_SetRenderDrawColor(a_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    l_rect.w = 8;
    l_rect.h = 8;
    l_rect.x = l_area.w/2; //x position on screen
    l_rect.y = l_area.h/2; //y position on screen
    SDL_RenderFillRect(a_renderer, &l_rect);
}

void CleanUpSDL(SDL_Window* a_Window) {
  SDL_DestroyWindow(g_Window);
  SDL_DestroyRenderer(g_renderer);
  SDL_Quit();
}