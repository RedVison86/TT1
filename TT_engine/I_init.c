#include "E_main.h"

void I_init(void)
{
    I_init_SDL();
    I_init_Window();
    I_init_Render();
    I_init_SDL_IMG();

    I_init_Stage();
}
