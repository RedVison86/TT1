#include "E_main.h"

void I_init_Window(void)
{
    int WinFlag = SDL_WINDOW_SHOWN;
    E_engine.I_Window = SDL_CreateWindow("TT ver 0.0.3",
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         S_W,
                                         S_H,
                                         WinFlag);
}
