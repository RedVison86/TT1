#include "E_main.h"

void R_render_present(void)
{
    SDL_RenderPresent(E_engine.I_Render);
}


SDL_Texture *loadTexture(char *filename)
{
    SDL_Texture *tex;
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);
    tex = IMG_LoadTexture(E_engine.I_Render, filename);
    return tex;
}

void blit(SDL_Texture *tex, int x, int y)
{
    SDL_Rect des;
    des.x = x;
    des.y = y;
    SDL_QueryTexture(tex, NULL, NULL, &des.w, &des.h);
    SDL_RenderCopy(E_engine.I_Render, tex, NULL, &des);

}
