
#include "ennemy.h"

void initEnnemi(Ennemie *e)
{
    e->image_EN = IMG_Load("ennemi_spr.png");
   

    e->pos_image_EN.x = 900;
    e->pos_image_EN.y = 400;

    e->single_EN.w = 100;
    e->single_EN.h = 100;
    e->single_EN.x = 0;
    e->single_EN.y = 0;
    e->EN_num = 0;
    e->direction = 1;
    e->colis = 0;
    e->show = 1;
    e->state = WAITING;
    e->vie = 3;
    e->last_hit_time = 0;
}
void initcoeur(coeur *c)
{
    c->image_score = IMG_Load("v1.png"); 
    c->pos_score.x = 1000;
    c->pos_score.y = 50;

    c->frame.w = 126;
    c->frame.h = 35;
    c->frame.x = 0;
    c->frame.y = 0;
}

void afficherEnnemi(Ennemie e, SDL_Surface *screen)
{
    SDL_BlitSurface(e.image_EN, &e.single_EN, screen, &e.pos_image_EN);
   
}
void affichercoeur(coeur c, SDL_Surface *screen, int vie)
{
    if (vie > 0)
    {
        c.frame.w = 42 * vie; 
        SDL_BlitSurface(c.image_score, &c.frame, screen, &c.pos_score);
    }
}


void animerEnnemi(Ennemie *e)
{
	switch (e->direction)
	{
		case 0:
			e->single_EN.y = 0;
			break;
		case 1:
			e->single_EN.y = 100;
			break;
		case 3:
			e->single_EN.y = 200;
			break;
		case 4:
			e->single_EN.y = 300;
			break;
	}


	if (e->EN_num >= 0 && e->EN_num < 4)
	{
		e->single_EN.x = e->EN_num * e->single_EN.w;
		e->EN_num++;
	}
	if (e->EN_num == 4)
	{
		e->EN_num = 0;
		e->single_EN.x = e->EN_num * e->single_EN.w;
	}
}

void deplacere(Ennemie *e)
{

	srand(time(0));

	int maxi_down = 400 + rand() %600;
	int maxi_up = 10 + rand() % 600;
	

	if (e->pos_image_EN.x >= maxi_down) 
	{
		e->direction = 1;
		e->single_EN.y = 60;
	}
	if (e->pos_image_EN.x <= maxi_up) 
	{
		e->direction = 0;
		e->single_EN.y = 0;
	}

	if (e->direction == 1) 
	{
		e->pos_image_EN.x -= 5; 
	}
	if (e->direction == 0) 
	{

		e->pos_image_EN.x += 5; 
	}
}

void update_ennemie(Ennemie *e)
{
	
	animerEnnemi(e);
}

int detect_collision_ennemie(personnage *p, Ennemie *e)
{

	if ((p->perso_pos.x + p->perso_pos.w < e->pos_image_EN.x) || (p->perso_pos.x > e->pos_image_EN.x + e->single_EN.w) ||
		(p->perso_pos.y + p->perso_pos.h < e->pos_image_EN.y) || (p->perso_pos.y > e->pos_image_EN.y + e->single_EN.h))
		return 0;
	else
		return 1;
}

int gerer(Ennemie *e)
{

	e->show = 0;

	return 0;
}

void updateEnnemi(Ennemie *e, SDL_Rect pos_Hero)
{
	int d = pos_Hero.x - e->pos_image_EN.x;
	int s1 = 300;
	int s2 = 1000;

	if (pos_Hero.x < s1 || pos_Hero.x > s2 )
	{
		e->state = WAITING;
	}
	else if (pos_Hero.x > s1 && pos_Hero.x < s2 && abs(d) > 300)
	{
		e->state = FOLLOWING;
	}
	else e->state = ATTACKING;

	switch (e->state)
	{
		case WAITING:
			if (e->direction == 3) e->direction = 0;
			else if (e->direction == 4) e->direction = 1;
			break;
		case FOLLOWING:
			if (d > 0)
			{
				e->direction = 0;
				e->pos_image_EN.x += 5;
			}
			else if (d < 0)
			{
				e->direction = 1;
				e->pos_image_EN.x -= 5;
			}
			break;
		case ATTACKING:
			if (d > 0)
			{
				e->direction = 3;
				e->pos_image_EN.x += 5;
			}
			else if (d < 0)
			{
				e->direction = 4;
				e->pos_image_EN.x -= 5;
			}
			break;
	}
}
void deplacerIA(Ennemie *e, personnage p)
{
    if (((e->pos_image_EN.x - p.perso_pos.x) < 200) && ((e->pos_image_EN.x - p.perso_pos.x) > 80))
    {
        e->direction = 1;
        e->pos_image_EN.x -= 10;
    }
    else if (((e->pos_image_EN.x - p.perso_pos.x) < -80) && ((e->pos_image_EN.x - p.perso_pos.x) > -400))
    {
        e->direction = 0;
        e->pos_image_EN.x += 10;
    }
    else if (((e->pos_image_EN.x - p.perso_pos.x) <= 80) && ((e->pos_image_EN.x - p.perso_pos.x) >= 0))
    {
        e->direction = 1;
    }
    else if (((e->pos_image_EN.x - p.perso_pos.x) <= 0) && ((e->pos_image_EN.x - p.perso_pos.x) >= -80))
    {
        e->direction = 0;
    }
    else
    {
        deplacere(e); // appel du déplacement par défaut
    }
}

