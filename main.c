#include "chip8.h"
#include <stdlib.h>
#include "SDL2/SDL.h"

int initSDL();
void CleanUpSDL();
void GameLoop();

CHIP8_t Chip8;
SDL_Window* Window;
 
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

  CleanUpSDL(Window);

  return 0;
}

void GameLoop() {
  SDL_Event l_event;
  int running = 1;
  while(running)
  {
    //Chip8.EmulateCycle(&Chip8);
    //consume events
	  while(SDL_PollEvent(&l_event)){
		  if(l_event.type == SDL_QUIT){
			  running=0;
		  }
	  }

    SDL_Delay(16);
    if(!(Chip8.V[15] & 0))
      printf("drawing graphics\n");
 
    Chip8.SetKeys(&Chip8);	
  }
}

int initSDL() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    return -1;
  }

  Window = SDL_CreateWindow("", 0, 0, 0, 0, SDL_WINDOW_HIDDEN);
  SDL_SetWindowSize(Window,200,200);
  SDL_SetWindowTitle(Window, "Chip8 Emulator");
  SDL_ShowWindow(Window);
  SDL_SetWindowPosition(Window,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED);
}

void CleanUpSDL(SDL_Window* a_Window) {
  SDL_DestroyWindow(Window);
  SDL_Quit();
}