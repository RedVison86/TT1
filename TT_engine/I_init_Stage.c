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
