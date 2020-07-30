#include "E_main.h"

static void logic(void);
static void draw(void);
static void Init_Player(void);
static void doPlayer(void);
static void doBullet(void);

static void fireBullet(void);
static void drawPlayer(void);
static void drawBullet(void);

static Model *player;
static SDL_Texture *bulletTexture;

void Init_Stage(void)
{
    void I_init_Stage(void)
{
    E_engine.I_delegate.logic = logic;
    E_engine.I_delegate.render = draw;
    E_stage.fighterTail = &E_stage.fighterHead;
    E_stage.bulletTail = &E_stage.bulletHead;
    Init_Player();
    bulletTexture = loadTexture("TT_gfx/bul.png");
}

static void Init_Player()
{
    player = malloc(sizeof(Model));
    E_stage.fighterTail->I_Next = player;
    E_stage.fighterTail = player;

    player->x = 100;
    player->y = 100;
    player->I_Tex = loadTexture("TT_gfx/001.png");
    SDL_QueryTexture(player->I_Tex, NULL, NULL, &player->w, &player->h);

}

static void logic(void)
{
    doPlayer();
    doBullet();

}

static void doPlayer(void)
{
    player->dx = player->dy = 0;

    if(player->reload > 0)
    {
        player->reload--;
    }

    if(E_engine.keyboard[SDL_SCANCODE_UP])
    {
        player->dy = -PLAYER_SPEED;
    }

    if(E_engine.keyboard[SDL_SCANCODE_DOWN])
    {
        player->dy = PLAYER_SPEED;
    }

    if(E_engine.keyboard[SDL_SCANCODE_LEFT])
    {
        player->dx = -PLAYER_SPEED;
    }

    if(E_engine.keyboard[SDL_SCANCODE_RIGHT])
    {
        player->dx = PLAYER_SPEED;
    }

    if(E_engine.keyboard[SDL_SCANCODE_LCTRL] && player->reload == 0)
    {
        fireBullet();
    }

    player->x += player->dx;
    player->y += player->dy;

}

static void fireBullet(void)
{
    Model *bullet;
    bullet = malloc(sizeof(Model));
    E_stage.bulletTail->I_Next = bullet;
    E_stage.bulletTail = bullet;

    bullet->x = player->x;
    bullet->y = player->y;
    bullet->dx = PLAYER_BULLET_SPEED;
    bullet->health = 1;
    bullet->I_Tex = bulletTexture;
    SDL_QueryTexture(bullet->I_Tex, NULL, NULL, &bullet->w, &bullet->h);

    bullet->y += (player->h / 2) - (bullet->h / 2);
    player->reload = 8;
}

static void doBullet(void)
{
    Model *b, *pre;
    pre = &E_stage.bulletHead;
    for (b = E_stage.bulletHead.I_Next; b != NULL; b = b->I_Next)
    {
        b->x += b->dx;
        b->y += b->dy;

        if(b->x > S_W)
        {
            if(b == E_stage.bulletTail)
            {
                E_stage.bulletTail = pre;
            }

            pre->I_Next = b->I_Next;
            free(b);
            b = pre;
        }

        pre = b;
    }

}


static void draw(void)
{
    drawPlayer();
    drawBullet();
}

/* ура рисовач*/
static void drawPlayer(void)
{
    blit(player->I_Tex, player->x, player->y);
}

static void drawBullet(void)
{
    Model *b;
    for(b = E_stage.bulletHead.I_Next; b!=NULL; b = b->I_Next)
    {
        blit(b->I_Tex, b->x, b->y);
    }
}


}
