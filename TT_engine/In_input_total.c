#include "E_main.h"

void In_input_down(SDL_KeyboardEvent *eve)
{
    if(eve->repeat == 0 &&
       eve->keysym.scancode < MAX_KEYBOARD_KEYS)
    {
        E_engine.keyboard[eve->keysym.scancode] = 1;
    }

}

void In_input_up(SDL_KeyboardEvent *eve)
{
    if(eve->repeat == 0 &&
       eve->keysym.scancode < MAX_KEYBOARD_KEYS)
    {
        E_engine.keyboard[eve->keysym.scancode] = 0;
    }
}

void In_input_total(void)
{
    SDL_Event eve;
    while (SDL_PollEvent(&eve))
    {
        switch (eve.type)
        {
            case SDL_QUIT:
                exit(0);
                break;

            case SDL_KEYDOWN:
                In_input_down(&eve.key);
                break;

            case SDL_KEYUP:
                In_input_up(&eve.key);
                break;

            default:
                break;
        }
    }

}
