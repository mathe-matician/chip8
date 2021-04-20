#include "chip8.h"
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "SDLRect.h"

CHIP8_t Chip8;
const uint8_t *state;
void GameLoop();
int initSDL();
void CleanUpSDL();
void DrawPixel(SDL_Renderer* a_renderer);
void GoRight(SDL_Renderer* a_renderer);
void GoLeft(SDL_Renderer* a_renderer);
void GoUp(SDL_Renderer* a_renderer);
void GoDown(SDL_Renderer* a_renderer);
void HandleInput();

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

    HandleInput();

    SDL_Delay(16);
    if(!(Chip8.V[15] & 0)) {
      //DrawPixel(g_renderer);
    }

    SDL_UpdateWindowSurface(g_Window);
 
    Chip8.SetKeys(&Chip8);	
  }
}

int initSDL() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
    return 3;
  }

  g_Window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
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

  g_rect.w = 8;
  g_rect.h = 8;
  g_rect.x = 50;
  g_rect.y = 50;

  return 0;
}

void DrawPixel(SDL_Renderer* a_renderer) {
    SDL_RenderGetViewport(g_renderer, &g_area);
    SDL_SetRenderDrawColor(a_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    //g_rect.x += 1;
    //g_rect.y += 1;
    SDL_RenderFillRect(a_renderer, &g_rect);
}

void GoRight(SDL_Renderer* a_renderer) {
    SDL_RenderGetViewport(g_renderer, &g_area);
    SDL_SetRenderDrawColor(a_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    if (g_rect.x + 1 > 255) {
      g_rect.x = 0;
    } else {
      g_rect.x += 1;
    }
    SDL_RenderFillRect(a_renderer, &g_rect);
}

void GoLeft(SDL_Renderer* a_renderer) {
    SDL_RenderGetViewport(g_renderer, &g_area);
    SDL_SetRenderDrawColor(a_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    if (g_rect.x - 1 < 0) {
      g_rect.x = 255;
    } else {
      g_rect.x -= 1;
    }
    SDL_RenderFillRect(a_renderer, &g_rect);
}

void GoUp(SDL_Renderer* a_renderer) {
    DRW(&Chip8);
}

void GoDown(SDL_Renderer* a_renderer) {
    SDL_RenderGetViewport(g_renderer, &g_area);
    SDL_SetRenderDrawColor(a_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    if (g_rect.y + 1 > 127) {
      g_rect.y = 0;
    } else {
      g_rect.y += 1;
    }
    SDL_RenderFillRect(a_renderer, &g_rect);
}

void CleanUpSDL(SDL_Window* a_Window) {
  SDL_DestroyWindow(g_Window);
  SDL_DestroyRenderer(g_renderer);
  SDL_Quit();
}

void HandleInput() {
    state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_RIGHT]) {
      GoRight(g_renderer);
    }

    if (state[SDL_SCANCODE_LEFT]) {
      GoLeft(g_renderer);
    }

    if (state[SDL_SCANCODE_UP]) {
      GoUp(g_renderer);
    }

    if (state[SDL_SCANCODE_DOWN]) {
      GoDown(g_renderer);
    }
}