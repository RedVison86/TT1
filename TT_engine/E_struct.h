#ifndef E_STRUCT_H_INCLUDED
#define E_STRUCT_H_INCLUDED

typedef struct Model Model;
typedef struct
{
    void (*logic)(void);
    void (*render)(void);

} Delegate;

typedef struct
{
    SDL_Renderer *I_Render;
    SDL_Window *I_Window;
    Delegate I_delegate;
    int keyboard[MAX_KEYBOARD_KEYS];

} Engine;

struct Model
{
    float x;
    float y;
    int w;
    int h;
    float dx;
    float dy;
    int health;
    int reload;
    int side;
    SDL_Texture *I_Tex;
    Model *I_Next;
};
typedef struct
{
      Model fighterHead, *fighterTail;
      Model bulletHead, *bulletTail;
} Stage;

/* ========== */

Engine E_engine;
Stage E_stage;

#endif // E_STRUCT_H_INCLUDED

