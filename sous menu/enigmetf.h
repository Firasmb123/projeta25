#ifndef ENIGMETF_H_INCLUDED
#define ENIGMETF_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

typedef struct {
    SDL_Surface *background;
    SDL_Surface *question;       // Surface for question text
    SDL_Surface *reponses[3];      // Surface for the three answers
    SDL_Surface *button;
    SDL_Surface *button_s;

    int pos_selected;            // Selected answer index (1, 2 or 3)
    SDL_Rect pos_question;       // Position for question text
    SDL_Rect pos_reponse1;       // Position for answer 1 button
    SDL_Rect pos_reponse2;       // Position for answer 2 button
    SDL_Rect pos_reponse3;       // Position for answer 3 button
    int num_question;           

    SDL_Rect pos_reponse1txt;    // Position for answer 1 text
    SDL_Rect pos_reponse2txt;    // Position for answer 2 text
    SDL_Rect pos_reponse3txt;    // Position for answer 3 text

    int positionVraiReponse;     
    TTF_Font *police; 
    TTF_Font *police1;
    SDL_Surface *afficherTemps;
    SDL_Rect positionTemps;
    int gameover;
    int tempsActuel, tempsPrecedent, compteur, fquestion;
    char temps[20];
    TTF_Font *police3;
} enigmetf;

void InitEnigme(enigmetf *e, char nomfichier[]);
void afficherEnigme(enigmetf *e, SDL_Surface *ecran);
void verify_enigme(enigmetf *e, SDL_Surface *ecran);
void animer(enigmetf *e);
void free_Surface_enigme(enigmetf e);

#endif

