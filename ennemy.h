

#ifndef ENNEMIE_H_INCLUDED
#define ENNEMIE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <stdbool.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include<time.h>
#include "entity.h"


typedef enum STATE STATE;
enum STATE
{
	WAITING,
	FOLLOWING,
	ATTACKING
};

typedef struct {
	
	int direction;
	int colis ;
	int type;
	int maxi_up,maxi_down;
	int show;
        SDL_Surface *image_EN;  
	SDL_Rect pos_image_EN;
	SDL_Rect single_EN;	
	int EN_num;
	STATE state;
	int vie; 
	Uint32 last_hit_time;
       
}Ennemie;
typedef struct {

          SDL_Surface *image_score; 
         SDL_Rect pos_score; 
	SDL_Rect frame;
}coeur;
void initEnnemi  (Ennemie *e) ;
void afficherEnnemi (Ennemie e, SDL_Surface *screen);
void animerEnnemi (Ennemie *e);
void update_ennemie(Ennemie *e);
void deplacere(Ennemie *e);

void updateEnnemi(Ennemie *e, SDL_Rect pos_Hero);
int detect_collision_ennemie(personnage *p, Ennemie *e);

int gerer(Ennemie *e) ; 
void initcoeur  (coeur *c) ;
void affichercoeur(coeur c, SDL_Surface *screen, int vie);
void deplacerIA(Ennemie *en,personnage p );


#endif 
