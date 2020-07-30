#include "E_main.h"

void R_render_prepare(void)
{
    SDL_SetRenderDrawColor(E_engine.I_Render, 72, 60, 72, 0);
    SDL_RenderClear(E_engine.I_Render);
}
