#ifndef E_MAIN_H_INCLUDED
#define E_MAIN_H_INCLUDED

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

/* ============ */

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

/* ============ */
#include "E_def.h"
#include "E_struct.h"



/* 1level arch */
extern void I_init(void);


/* 2level arch */
extern void I_init_SDL(void);
extern void I_init_Window(void);
extern void I_init_Render(void);
extern void I_init_SDL_IMG(void);

extern void I_init_Stage(void);

extern void In_input(void);
extern void In_input_total(void);
extern void R_render_prepare(void);
extern void R_render_present(void);

/* 3Level arch */
extern void blit(SDL_Texture *tex, int x, int y);
extern SDL_Texture *loadTexture(char *filename);
extern int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
extern void calcSlope(int x1, int y1, int x2, int y2, float *dx, float *dy);

#endif // E_MAIN_H_INCLUDED
