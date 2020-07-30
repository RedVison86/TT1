#include "E_main.h"

static void capFrameRate(long *then, float *remainder);

int main(int argc, char *argv[])
{
    long then;
    float rem;
    I_init();
    then = SDL_GetTicks();
    rem = 0;

    while (1)
    {
        R_render_prepare();
        In_input();
        E_engine.I_delegate.logic();
        E_engine.I_delegate.render();
        R_render_present();
        capFrameRate(&then, &rem);
    }

    SDL_DestroyRenderer(E_engine.I_Render);

    return 0;
}

static void capFrameRate(long *then, float *rem)
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


/*
   E - engine files
   I - init files
   In - input files
   R - render files
   g - game files
*/
