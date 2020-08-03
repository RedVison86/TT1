#include "E_main.h"

/* real game logic */

static void logic(void);
static void draw(void);
static void Init_Player(void);
static void doPlayer(void);
static void doFighters(void);
static void doBullet(void);
static void spawnEnemies(void);
static void fireBullet(void);
static void drawPlayer(void);
static void drawBullet(void);
static void drawFighters(void);

static Model *player;
static SDL_Texture *bulletTexture;
static SDL_Texture *enemyTexture;

static int bulletHitFighter(Model *bullet);

int enemySpawnerTime;

void I_init_Stage(void)
{
    E_engine.I_delegate.logic = logic;
    E_engine.I_delegate.render = draw;

    E_stage.fighterTail = &E_stage.fighterHead;
    E_stage.bulletTail = &E_stage.bulletHead;
    Init_Player();

    bulletTexture = loadTexture("TT_gfx/bul.png");
    enemyTexture = loadTexture("TT_gfx/002.png");

    enemySpawnerTime = 0;
}


static void Init_Player()
{
    player = malloc(sizeof(Model));
    memset(player, 0, sizeof(Model));
    E_stage.fighterTail->I_Next = player;
    E_stage.fighterTail = player;
    player->side = SIDE_PLAYER;

    player->x = 100;
    player->y = 100;
    player->I_Tex = loadTexture("TT_gfx/001.png");
    SDL_QueryTexture(player->I_Tex, NULL, NULL, &player->w, &player->h);

}


static void logic(void)
{
    doPlayer();
    doFighters();
    doBullet();
    spawnEnemies();


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

static void doFighters(void)
{
    Model *enemy, *pre;
    for(enemy = E_stage.fighterHead.I_Next ; enemy != NULL; enemy = enemy->I_Next)
    {
        enemy->x += enemy->dx;
        enemy->y += enemy->dy;

        pre = &E_stage.fighterHead;

        if(enemy != player && (enemy->x < -enemy->w || enemy->health == 0))
        {
            if(enemy == E_stage.fighterTail)
            {
                E_stage.fighterTail = pre;
            }

            pre->I_Next = enemy->I_Next;
            free(enemy);
            enemy = pre;
        }

        pre = enemy;
    }



}

static void fireBullet(void)
{
    Model *bullet;
    bullet = malloc(sizeof(Model));
    memset(bullet, 0, sizeof(Model));
    E_stage.bulletTail->I_Next = bullet;
    E_stage.bulletTail = bullet;
    bullet->side = SIDE_PLAYER;

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

        if(bulletHitFighter(b) || b->x > S_W)
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

static int bulletHitFighter(Model *bullet)
{
    Model *enemy;

    for(enemy = E_stage.fighterHead.I_Next ; enemy != NULL ; enemy = enemy->I_Next)
    {
        if(enemy->side != bullet->side && collision(bullet->x,
                                           bullet->y,
                                           bullet->w,
                                           bullet->h,
                                           enemy->x,
                                           enemy->y,
                                           enemy->w,
                                           enemy->h))
        {
            bullet->health = 0;
            enemy->health = 0;

            return 1;
        }
    }

    return 0;
}

static void spawnEnemies(void)
{
    Model *enemy;
    if(--enemySpawnerTime <= 0)
    {
        enemy = malloc(sizeof(Model));
        memset(enemy, 0, sizeof(Model));
        E_stage.fighterTail->I_Next = enemy;
        E_stage.fighterTail = enemy;
        enemy->side = SIDE_ALIEN;
        enemy->health = 1;

        enemy->x = S_W;
        enemy->y = rand() % S_H;
        enemy->I_Tex = enemyTexture;
        SDL_QueryTexture(enemy->I_Tex, NULL, NULL, &enemy->w, &enemy->h);

        enemy->dx = -(2 + (rand() % 4));
        enemySpawnerTime = 30 + (rand() % 60);
    }

}

static void draw(void)
{
    drawPlayer();
    drawBullet();
    drawFighters();
}

/* óðà ðèñîâà÷*/
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

static void drawFighters(void)
{
    Model *enemy;
    for(enemy = E_stage.fighterHead.I_Next ; enemy != NULL ; enemy = enemy->I_Next)
    {
        blit(enemy->I_Tex, enemy->x, enemy->y);
    }
}
