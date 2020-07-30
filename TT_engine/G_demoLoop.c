#include "E_main.h"

void G_demoLoop(void)
{
    while (1)
    {
        R_render_prepare();
        In_input();
        E_engine.I_delegate.logic();
        E_engine.I_delegate.render();
        R_render_present();
        SDL_Delay(16);
    }

}

/*

static void FrameRate(long *then, float *rem)
{
    long wait, FrameTime;
    wait = 16 + *rem;
    *rem -= (int)*rem;
    FrameTime = SDL_GetTicks() - *then;
    wait -= FrameTime;
    if(wait < 1)
    {
        wait = 1;
    }

    SDL_Delay(wait);
    *rem += 0.665;
    *then = SDL_GetTicks();
}

*/

