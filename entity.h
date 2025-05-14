
#ifndef entite_H_INCLUDED
#define entite_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <time.h>
#include <math.h>
typedef struct struct_text
{
	TTF_Font *font;
	SDL_Color color;
	SDL_Surface *text;
	SDL_Rect postext;
} Text;



typedef struct
{

	int direction;						  
	int col;							 
	int maxi_up, maxi_down;				  
	int show;							 
	SDL_Rect pos_entity; 
	SDL_Surface *entity;
	SDL_Rect single_EN;
	int EN_num;

} entity;

typedef struct
{
	SDL_Rect perso_pos;
	SDL_Surface *perso;
	int Vscore;
	Text score;
} personnage;

void initentity(entity *e);							
void afficherentity(entity e, SDL_Surface *screen);
void animerentity(entity *e);						
void deplacer(entity *e);							
void update_entity(entity *e);						
int collisionTri(entity e, SDL_Rect pose);
int detect_collision(personnage *p, entity *e);
int gestion(entity *e);						
void initialiser_perso(personnage *p);
void afficher_perso(personnage p, SDL_Surface *screen);

#endif
