#include "E_main.h"

void I_init_Render(void)
{
    int RenderFlag = SDL_RENDERER_ACCELERATED;
    E_engine.I_Render = SDL_CreateRenderer(E_engine.I_Window,
                                           -1,
                                           RenderFlag);
}
