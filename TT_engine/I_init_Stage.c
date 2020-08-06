#include "E_main.h"

static void logic(void);
static void draw(void);
static void initPlayer(void);
static void fireBullet(void);
static void doPlayer(void);
static void doFighters(void);
static void doBullets(void);
static void drawFighters(void);
static void drawBullets(void);
static void spawnEnemies(void);
static int bulletHitFighter(Model *b);
static void doEnemies(void);
static void fireAlienBullet(Model *e);
static void clipPlayer(void);
static void resetE_stage(void);

static Model *player;
static SDL_Texture *bulletTexture;
static SDL_Texture *enemyTexture;
static SDL_Texture *alienBulletTexture;
static SDL_Texture *playerTexture;
static int enemySpawnTimer;
static int E_stageResetTimer;

void I_init_Stage(void)
{
	E_engine.I_delegate.logic = logic;
	E_engine.I_delegate.draw = draw;

	memset(&E_stage, 0, sizeof(E_stage));
	E_stage.fighterTail = &E_stage.fighterHead;
	E_stage.bulletTail = &E_stage.bulletHead;

	bulletTexture = loadTexture("TT_gfx/bul.png");
	enemyTexture = loadTexture("TT_gfx/002.png");
	alienBulletTexture = loadTexture("TT_gfx/abul.png");
	playerTexture = loadTexture("TT_gfx/001.png");

	resetE_stage();


}

static void resetE_stage(void)
{
	Model *e;

	while (E_stage.fighterHead.I_Next)
	{
		e = E_stage.fighterHead.I_Next;
		E_stage.fighterHead.I_Next = e->I_Next;
		free(e);
	}

	while (E_stage.bulletHead.I_Next)
	{
		e = E_stage.bulletHead.I_Next;
		E_stage.bulletHead.I_Next = e->I_Next;
		free(e);
	}

	memset(&E_stage, 0, sizeof(E_stage));
	E_stage.fighterTail = &E_stage.fighterHead;
	E_stage.bulletTail = &E_stage.bulletHead;

	initPlayer();
	enemySpawnTimer = 0;
	E_stageResetTimer = FPS * 2;
}

static void initPlayer()
{
	player = malloc(sizeof(Model));
	memset(player, 0, sizeof(Model));
	E_stage.fighterTail->I_Next = player;
	E_stage.fighterTail = player;

	player->health = 1;
	player->x = 100;
	player->y = 100;
	player->I_Tex = playerTexture;
	SDL_QueryTexture(player->I_Tex, NULL, NULL, &player->w, &player->h);

	player->side = SIDE_PLAYER;
}

static void logic(void)
{
	doPlayer();

	doEnemies();

	doFighters();

	doBullets();

	spawnEnemies();

	clipPlayer();

	if (player == NULL && --E_stageResetTimer <= 0)
	{
		resetE_stage();
	}
}

static void doPlayer(void)
{
	if (player != NULL)
	{
		player->dx = player->dy = 0;

		if (player->reload > 0)
		{
			player->reload--;
		}

		if (E_engine.keyboard[SDL_SCANCODE_UP])
		{
			player->dy = -PLAYER_SPEED;
		}

		if (E_engine.keyboard[SDL_SCANCODE_DOWN])
		{
			player->dy = PLAYER_SPEED;
		}

		if (E_engine.keyboard[SDL_SCANCODE_LEFT])
		{
			player->dx = -PLAYER_SPEED;
		}

		if (E_engine.keyboard[SDL_SCANCODE_RIGHT])
		{
			player->dx = PLAYER_SPEED;
		}

		if (E_engine.keyboard[SDL_SCANCODE_LCTRL] && player->reload <= 0)
		{
			fireBullet();
		}
	}
}

static void fireBullet(void)
{
	Model *bullet;

	bullet = malloc(sizeof(Model));
	memset(bullet, 0, sizeof(Model));
	E_stage.bulletTail->I_Next = bullet;
	E_stage.bulletTail = bullet;

	bullet->x = player->x;
	bullet->y = player->y;
	bullet->dx = PLAYER_BULLET_SPEED;
	bullet->health = 1;
	bullet->I_Tex = bulletTexture;
	bullet->side = player->side;
	SDL_QueryTexture(bullet->I_Tex, NULL, NULL, &bullet->w, &bullet->h);

	bullet->y += (player->h / 2) - (bullet->h / 2);

	bullet->side = SIDE_PLAYER;

	player->reload = 8;
}

static void doEnemies(void)
{
	Model *e;

	for (e = E_stage.fighterHead.I_Next ; e != NULL ; e = e->I_Next)
	{
		if (e != player && player != NULL && --e->reload <= 0)
		{
			fireAlienBullet(e);
		}
	}
}

static void fireAlienBullet(Model *e)
{
	Model *bullet;

	bullet = malloc(sizeof(Model));
	memset(bullet, 0, sizeof(Model));
	E_stage.bulletTail->I_Next = bullet;
	E_stage.bulletTail = bullet;

	bullet->x = e->x;
	bullet->y = e->y;
	bullet->health = 1;
	bullet->I_Tex = alienBulletTexture;
	bullet->side = e->side;
	SDL_QueryTexture(bullet->I_Tex, NULL, NULL, &bullet->w, &bullet->h);

	bullet->x += (e->w / 2) - (bullet->w / 2);
	bullet->y += (e->h / 2) - (bullet->h / 2);

	calcSlope(player->x + (player->w / 2), player->y + (player->h / 2), e->x, e->y, &bullet->dx, &bullet->dy);

	bullet->dx *= ALIEN_BULLET_SPEED;
	bullet->dy *= ALIEN_BULLET_SPEED;
	bullet->side = SIDE_ALIEN;

	e->reload = (rand() % FPS * 2);
}

static void doFighters(void)
{
	Model *e, *prev;

	prev = &E_stage.fighterHead;

	for (e = E_stage.fighterHead.I_Next ; e != NULL ; e = e->I_Next)
	{
		e->x += e->dx;
		e->y += e->dy;

		if (e != player && e->x < -e->w)
		{
			e->health = 0;
		}

		if (e->health == 0)
		{
			if (e == player)
			{
				player = NULL;
			}

			if (e == E_stage.fighterTail)
			{
				E_stage.fighterTail = prev;
			}

			prev->I_Next = e->I_Next;
			free(e);
			e = prev;
		}

		prev = e;
	}
}

static void doBullets(void)
{
	Model *b, *prev;

	prev = &E_stage.bulletHead;

	for (b = E_stage.bulletHead.I_Next ; b != NULL ; b = b->I_Next)
	{
		b->x += b->dx;
		b->y += b->dy;

		if (bulletHitFighter(b) || b->x < -b->w || b->y < -b->h || b->x > S_W || b->y > S_H)
		{
			if (b == E_stage.bulletTail)
			{
				E_stage.bulletTail = prev;
			}

			prev->I_Next = b->I_Next;
			free(b);
			b = prev;
		}

		prev = b;
	}
}

static int bulletHitFighter(Model *b)
{
	Model *e;

	for (e = E_stage.fighterHead.I_Next ; e != NULL ; e = e->I_Next)
	{
		if (e->side != b->side && collision(b->x, b->y, b->w, b->h, e->x, e->y, e->w, e->h))
		{
			b->health = 0;
			e->health = 0;

			return 1;
		}
	}

	return 0;
}

static void spawnEnemies(void)
{
	Model *enemy;

	if (--enemySpawnTimer <= 0)
	{
		enemy = malloc(sizeof(Model));
		memset(enemy, 0, sizeof(Model));
		E_stage.fighterTail->I_Next = enemy;
		E_stage.fighterTail = enemy;

		enemy->x = S_W;
		enemy->y = rand() % S_H;
		enemy->I_Tex = enemyTexture;
		SDL_QueryTexture(enemy->I_Tex, NULL, NULL, &enemy->w, &enemy->h);

		enemy->dx = -(2 + (rand() % 4));

		enemy->side = SIDE_ALIEN;
		enemy->health = 1;

		enemy->reload = FPS * (1 + (rand() % 3));

		enemySpawnTimer = 30 + (rand() % FPS);
	}
}

static void clipPlayer(void)
{
	if (player != NULL)
	{
		if (player->x < 0)
		{
			player->x = 0;
		}

		if (player->y < 0)
		{
			player->y = 0;
		}

		if (player->x > S_W - player->w)
		{
			player->x = S_W - player->w;
		}

		if (player->y > S_H - player->h)
		{
			player->y = S_H - player->h;
		}
	}
}

static void draw(void)
{
	drawFighters();

	drawBullets();
}

static void drawFighters(void)
{
	Model *e;

	for (e = E_stage.fighterHead.I_Next ; e != NULL ; e = e->I_Next)
	{
		blit(e->I_Tex, e->x, e->y);
	}
}

static void drawBullets(void)
{
	Model *b;

	for (b = E_stage.bulletHead.I_Next ; b != NULL ; b = b->I_Next)
	{
		blit(b->I_Tex, b->x, b->y);
	}
}









/*
#include "E_main.h"

static void logic(void);
static void draw(void);
static void Init_Player(void);
static void doPlayer(void);
static void doEnemies(void);
static void doFighters(void);
static void doBullet(void);
static void spawnEnemies(void);
static void fireBullet(void);
static void drawBullet(void);
static void drawFighters(void);
static void drawPlayer(void);
static void clipPlayer(void);
static void resetE_stage(void);

static  *player;
static SDL_I_Tex *playerI_Tex;
static SDL_I_Tex *bulletI_Tex;
static SDL_I_Tex *enemyI_Tex;
static SDL_I_Tex *enemyBulletI_Tex;

static int bulletHitFighter( *bullet);
static void fireEnemyBullet( *enemy);

static int enemySpawnerTime;
static int resetE_stageTime;

void I_init_E_stage(void)
{
    E_engine.I_I_delegate.logic = logic;
    E_engine.I_I_delegate.draw = draw;

    memset(&E_E_stage, 0, sizeof(E_E_stage));
    E_E_stage.fighterTail = &E_E_stage.fighterHead;
    E_E_stage.bulletTail = &E_E_stage.bulletHead;

    playerI_Tex = loadI_Tex("TT_gfx/001.png");
    bulletI_Tex = loadI_Tex("TT_gfx/bul.png");
    enemyI_Tex = loadI_Tex("TT_gfx/002.png");
    enemyBulletI_Tex = loadI_Tex("TT_gfx/abul.png");

    resetE_stage();

}

static void resetE_stage(void)
{
     *enemy;

    while (E_E_stage.fighterHead.)
    {
        enemy = E_E_stage.fighterHead.;
        E_E_stage.fighterHead.= enemy->;
        free(enemy);
    }

    while (E_E_stage.bulletHead.)
    {
        enemy = E_E_stage.bulletHead.;
        E_E_stage.bulletHead. = enemy->;
        free(enemy);
    }

    memset(&E_E_stage, 0, sizeof(E_E_stage));
    E_E_stage.fighterTail = &E_E_stage.fighterHead;
    E_E_stage.bulletTail = &E_E_stage.bulletHead;


    Init_Player();
    enemySpawnerTime = 0;
    resetE_stageTime = FPS * 2;

}
static void Init_Player()
{
    player = malloc(sizeof());
    memset(player, 0, sizeof());
    E_E_stage.fighterTail-> = player;
    E_E_stage.fighterTail = player;

    player->side = SIDE_PLAYER;
    player->health = 1;
    player->x = 100;
    player->y = 100;
    player->I_Tex = playerI_Tex;
    SDL_QueryI_Tex(player->I_Tex, NULL, NULL, &player->w, &player->h);
}

static void logic(void)
{
    doPlayer();

    doEnemies();

    doFighters();

    doBullet();

    spawnEnemies();

    clipPlayer();

    if(player == NULL && --resetE_stageTime <= 0)
    {
        resetE_stage();
    }
}

static void doPlayer(void)
{
    if(player != NULL)
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

void doFighters(void)
{
     *enemy, *pre;

    pre = &E_E_stage.fighterHead;

    for(enemy = E_E_stage.fighterHead. ; enemy != NULL; enemy = enemy->)
    {
        enemy->x += enemy->dx;
        enemy->y += enemy->dy;


        if(enemy != player && (enemy->x < -enemy->w || enemy->health == 0))
        {
            E_E_stage.fighterTail = pre;
            enemy->health = 0;
        }

        pre-> = enemy->;
        free(enemy);
        enemy = pre;


        if(enemy->h == 0)
        {
            if(enemy == player)
            {
                player = NULL;
            }

            if(enemy == E_E_stage.fighterTail)
            {
                E_E_stage.fighterTail = pre;
            }

            pre-> = enemy->;
            free(enemy);
            enemy = pre;
        }

        pre = enemy;
    }

}


static void fireBullet(void)
{
     *bullet;
    bullet = malloc(sizeof());
    memset(bullet, 0, sizeof());
    E_E_stage.bulletTail-> = bullet;
    E_E_stage.bulletTail = bullet;
    bullet->side = SIDE_PLAYER;

    bullet->x = player->x;
    bullet->y = player->y;
    bullet->dx = PLAYER_BULLET_SPEED;
    bullet->health = 1;
    bullet->I_Tex = bulletI_Tex;
    bullet->side = player->side;
    SDL_QueryI_Tex(bullet->I_Tex, NULL, NULL, &bullet->w, &bullet->h);

    bullet->y += (player->h / 2) - (bullet->h / 2);
    bullet->side = SIDE_PLAYER;
    player->reload = 8;
}

static void doBullet(void)
{
     *bullet, *pre;
    pre = &E_E_stage.bulletHead;
    for (bullet = E_E_stage.bulletHead.; bullet != NULL; bullet = bullet->)
    {
        bullet->x += bullet->dx;
        bullet->y += bullet->dy;


        if(bulletHitFighter(bullet) || bullet->x < -bullet->w || bullet->y < -bullet->h || bullet->x > S_W || bullet->y > S_H)
        {
            if(bullet == E_E_stage.bulletTail)
            {
                E_E_stage.bulletTail = pre;
            }

            pre-> = bullet->;
            free(bullet);
            bullet = pre;
        }

        pre = bullet;
    }

}

static int bulletHitFighter( *bullet)
{
     *enemy;

    for(enemy = E_E_stage.fighterHead. ; enemy != NULL ; enemy = enemy->)
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

static void doEnemies(void)
{
     *enemy;

    for(enemy = E_E_stage.fighterHead. ; enemy != NULL ; enemy = enemy->)
    {
        if(enemy != player && player != NULL && --enemy->reload <= 0)
        {
            fireEnemyBullet(enemy);
        }
    }

}

static void fireEnemyBullet( *enemy)
{
     *bullet;
    bullet = malloc(sizeof());
    memset(bullet, 0, sizeof());
    E_E_stage.bulletTail-> = bullet;
    E_E_stage.bulletTail = bullet;

    bullet->x = enemy->x;
    bullet->y = enemy->y;
    bullet->health = 1;
    bullet->I_Tex = enemyBulletI_Tex;
    bullet->side = enemy->side;

    SDL_QueryI_Tex(bullet->I_Tex, NULL, NULL, &bullet->w, &bullet->h);

    bullet->x += (enemy->w / 2) - (bullet->w / 2);
    bullet->y += (enemy->h / 2) - (bullet->h / 2);

    calcSlope(player->x + (player->w/2), player->y + (player->h/2), enemy->x, enemy->y, &bullet->dx, &bullet->dy);

    bullet->dx *= ALIEN_BULLET_SPEED;
    bullet->dy *= ALIEN_BULLET_SPEED;
    bullet->side = SIDE_ALIEN;

    enemy->reload = (rand() % FPS * 2);

}

static void spawnEnemies(void)
{
     *enemy;

    if(--enemySpawnerTime <= 0)
    {
        enemy = malloc(sizeof());
        memset(enemy, 0, sizeof());
        E_E_stage.fighterTail-> = enemy;
        E_E_stage.fighterTail = enemy;
        enemy->side = SIDE_ALIEN;
        enemy->health = 1;

        enemy->x = S_W;
        enemy->y = rand() % S_H;
        enemy->I_Tex = enemyI_Tex;
        SDL_QueryI_Tex(enemy->I_Tex, NULL, NULL, &enemy->w, &enemy->h);

        enemy->dx = -(2 + (rand() % 4));

        enemy->side = SIDE_ALIEN;
        enemy->health = 1;

        enemy->reload = FPS * (1 + (rand() % 3));
        enemySpawnerTime = 30 + (rand() & FPS);
    }

}



static void clipPlayer(void)
{
    if(player != NULL)
    {
        if(player->x < 0)
        {
            player->x = 0;
        }

        if(player->y < 0)
        {
            player->y = 0;
        }

        if(player->x > S_W - player->w)
        {
            player->x = S_W - player->w;
        }

        if(player->y > S_H - player->h)
        {
            player->y = S_H - player->h;
        }
    }

}


static void draw(void)
{
    drawBullet();
    drawFighters();
    drawPlayer();
}


static void drawBullet(void)
{
     *bullet;
    for(bullet = E_E_stage.bulletHead.; bullet!=NULL; bullet = bullet->)
    {
        blit(bullet->I_Tex, bullet->x, bullet->y);
    }
}

static void drawFighters(void)
{
     *enemy;
    for(enemy = E_E_stage.fighterHead. ; enemy != NULL ; enemy = enemy->)
    {
        blit(enemy->I_Tex, enemy->x, enemy->y);
    }
}

static void drawPlayer(void)
{
    blit(playerI_Tex, player->x, player->y);
}

*/
