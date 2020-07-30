#include "E_main.h"

void I_init_SDL(void)
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        exit(1);
    }

}
