#include "enigmetf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <stdbool.h>  

#define MAX_LINE_LENGTH 256
#define TOTAL_QUESTIONS 10

void InitEnigme(enigmetf *e, char nomfichier[]){
   
    TTF_Init();
    e->pos_selected = 0;  // No answer selected yet.
    
    e->background = IMG_Load("img/background.png");
    e->button = IMG_Load("img/button.png");
    e->button_s = IMG_Load("img/button_s.png");

    SDL_Color couleur = {0, 0, 128};  // Text color

    e->police = TTF_OpenFont("arial.ttf", 23);
    e->police1 = TTF_OpenFont("arial.ttf", 20);
    
  
    FILE *fquestion = fopen(nomfichier, "r");
    if (fquestion == NULL) {
        fprintf(stderr, "Error opening file %s\n", nomfichier);
        exit(1);
    }
    
    
    srand(time(NULL));
    int randomIndex = rand() % TOTAL_QUESTIONS; // Initialisation du générateur de nombres aléatoires

    char line[MAX_LINE_LENGTH];
    int currentLine = 0;
    while (fgets(line, MAX_LINE_LENGTH, fquestion)) {
        if (currentLine == randomIndex) {
            break;
        }
        currentLine++;
    }
    fclose(fquestion);
    
  
    line[strcspn(line, "\n")] = 0;
    
    /* Tokenize the line using semicolon as delimiter.
       Expected format: question;answer1;answer2;answer3;correct_index;  */
    char *tokens[5];
    char *token = strtok(line, ";");
    int tokenCount = 0;
    while (token != NULL && tokenCount < 5) {
        tokens[tokenCount++] = token;
        token = strtok(NULL, ";");
    }
    if (tokenCount < 5) {
        fprintf(stderr, "Malformed question line.\n");
        exit(1);
    }
    
    /* Determine which answer is correct based on file */
    int correctFromFile = atoi(tokens[4]);  // This should be 1, 2, or 3
    char *correctAnswer = NULL;
    char *wrong1 = NULL;
    char *wrong2 = NULL;
    
    if (correctFromFile == 1) {
        correctAnswer = tokens[1];
        wrong1 = tokens[2];
        wrong2 = tokens[3];
    } else if (correctFromFile == 2) {
        correctAnswer = tokens[2];
        wrong1 = tokens[1];
        wrong2 = tokens[3];
    } else if (correctFromFile == 3) {
        correctAnswer = tokens[3];
        wrong1 = tokens[1];
        wrong2 = tokens[2];
    } else {
        fprintf(stderr, "Invalid correct answer index in question file.\n");
        exit(1);
    }
    
  
    e->question = TTF_RenderText_Blended(e->police, tokens[0], couleur);
  
    e->reponses[2] = TTF_RenderText_Blended(e->police1, correctAnswer, couleur);
    e->reponses[0] = TTF_RenderText_Blended(e->police1, wrong1, couleur);
    e->reponses[1] = TTF_RenderText_Blended(e->police1, wrong2, couleur);

    
    e->positionVraiReponse = (rand() % 3) + 1;
    
   
    e->pos_question.x = 59;
    e->pos_question.y = 140;
    
    e->pos_reponse1.x = 5;
    e->pos_reponse1.y = 370;
    
    e->pos_reponse2.x = 410;
    e->pos_reponse2.y = 370;
    
    e->pos_reponse3.x = 200;
    e->pos_reponse3.y = 520;
    
    e->pos_reponse1txt.x = e->pos_reponse1.x + 100;
    e->pos_reponse1txt.y = 370 + 30;
    
    e->pos_reponse2txt.x = 510 + 50;
    e->pos_reponse2txt.y = 370 + 30;
    
    e->pos_reponse3txt.x = 260 + 50;
    e->pos_reponse3txt.y = 520 + 30;
    
   
    e->afficherTemps = NULL;
    e->positionTemps.x = 10;
    e->positionTemps.y = 20;
    e->tempsActuel = 0;
    e->tempsPrecedent = 0;
    e->compteur = 20;
    e->fquestion = 0;
    e->police3 = TTF_OpenFont("Sketch.ttf", 65);
}

void afficherEnigme(enigmetf *e, SDL_Surface *ecran) {
    SDL_Surface *animations[4];
    animations[0] = IMG_Load("img/animation1.png");
    animations[1] = IMG_Load("img/animation3.png");
    animations[2] = IMG_Load("img/animation2.png");
    animations[3] = IMG_Load("img/animation4.png");

    if (animations[0] == NULL || animations[1] == NULL || animations[2] == NULL || animations[3] == NULL) {
        fprintf(stderr, "Error loading animation images: %s\n", IMG_GetError());
        return;
    }
  
    SDL_Rect posAnimation = {630,555}; // Animation position
    int maxAnimations = 4; // Number of animation cycles

    SDL_Color couleurNoire = {255, 255, 255};
    char temps[20];
    sprintf(temps, "Temps : %d", e->compteur);
    e->afficherTemps = TTF_RenderText_Solid(e->police3, temps, couleurNoire);
    e->tempsActuel = SDL_GetTicks();
    if (e->tempsActuel - e->tempsPrecedent >= 1000) {
        e->compteur -= 1;
        if (e->compteur == 0) {
            SDL_Delay(1000);
            SDL_Quit();
        }
        e->tempsPrecedent = e->tempsActuel;
    }

    SDL_BlitSurface(e->background, NULL, ecran, NULL);
    SDL_BlitSurface(e->question, NULL, ecran, &e->pos_question);
    SDL_BlitSurface(e->afficherTemps, NULL, ecran, &e->positionTemps);

   
    switch (e->positionVraiReponse) {
        case 1: {
            switch (e->pos_selected) {
                case 1: {
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse1);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse2);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse3);
                    SDL_BlitSurface(e->reponses[2], NULL, ecran, &e->pos_reponse1txt);  // correct answer
                    SDL_BlitSurface(e->reponses[0], NULL, ecran, &e->pos_reponse2txt);
                    SDL_BlitSurface(e->reponses[1], NULL, ecran, &e->pos_reponse3txt);
                }
                break;
                case 2: {
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse1);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse2);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse3);
                    SDL_BlitSurface(e->reponses[2], NULL, ecran, &e->pos_reponse1txt);
                    SDL_BlitSurface(e->reponses[0], NULL, ecran, &e->pos_reponse2txt);
                    SDL_BlitSurface(e->reponses[1], NULL, ecran, &e->pos_reponse3txt);
                }
                break;
                case 3: {
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse1);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse2);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse3);
                    SDL_BlitSurface(e->reponses[2], NULL, ecran, &e->pos_reponse1txt);
                    SDL_BlitSurface(e->reponses[0], NULL, ecran, &e->pos_reponse2txt);
                    SDL_BlitSurface(e->reponses[1], NULL, ecran, &e->pos_reponse3txt);
                }
                break;
                default: {
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse1);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse2);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse3);
                    SDL_BlitSurface(e->reponses[2], NULL, ecran, &e->pos_reponse1txt);
                    SDL_BlitSurface(e->reponses[0], NULL, ecran, &e->pos_reponse2txt);
                    SDL_BlitSurface(e->reponses[1], NULL, ecran, &e->pos_reponse3txt);
                }
            }
            break;
        }
        case 2: {
            switch (e->pos_selected) {
                case 1: {
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse1);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse2);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse3);
                    SDL_BlitSurface(e->reponses[0], NULL, ecran, &e->pos_reponse1txt);
                    SDL_BlitSurface(e->reponses[2], NULL, ecran, &e->pos_reponse2txt);
                    SDL_BlitSurface(e->reponses[1], NULL, ecran, &e->pos_reponse3txt);
                }
                break;
                case 2: {
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse1);
                    SDL_BlitSurface(e->button_s, NULL, ecran, &e->pos_reponse2);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse3);
                    SDL_BlitSurface(e->reponses[0], NULL, ecran, &e->pos_reponse1txt);
                    SDL_BlitSurface(e->reponses[2], NULL, ecran, &e->pos_reponse2txt);
                    SDL_BlitSurface(e->reponses[1], NULL, ecran, &e->pos_reponse3txt);
                }
                break;
                case 3: {
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse1);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse2);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse3);
                    SDL_BlitSurface(e->reponses[0], NULL, ecran, &e->pos_reponse1txt);
                    SDL_BlitSurface(e->reponses[2], NULL, ecran, &e->pos_reponse2txt);
                    SDL_BlitSurface(e->reponses[1], NULL, ecran, &e->pos_reponse3txt);
                }
                break;
                default: {
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse1);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse2);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse3);
                    SDL_BlitSurface(e->reponses[0], NULL, ecran, &e->pos_reponse1txt);
                    SDL_BlitSurface(e->reponses[2], NULL, ecran, &e->pos_reponse2txt);
                    SDL_BlitSurface(e->reponses[1], NULL, ecran, &e->pos_reponse3txt);
                }
            }
            break;
        }
        case 3: {
            switch (e->pos_selected) {
                case 1: {
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse1);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse2);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse3);
                    SDL_BlitSurface(e->reponses[0], NULL, ecran, &e->pos_reponse1txt);
                    SDL_BlitSurface(e->reponses[1], NULL, ecran, &e->pos_reponse2txt);
                    SDL_BlitSurface(e->reponses[2], NULL, ecran, &e->pos_reponse3txt);
                }
                break;
                case 2: {
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse1);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse2);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse3);
                    SDL_BlitSurface(e->reponses[0], NULL, ecran, &e->pos_reponse1txt);
                    SDL_BlitSurface(e->reponses[1], NULL, ecran, &e->pos_reponse2txt);
                    SDL_BlitSurface(e->reponses[2], NULL, ecran, &e->pos_reponse3txt);
                }
                break;
                case 3: {
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse1);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse2);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse3);
                    SDL_BlitSurface(e->reponses[0], NULL, ecran, &e->pos_reponse1txt);
                    SDL_BlitSurface(e->reponses[1], NULL, ecran, &e->pos_reponse2txt);
                    SDL_BlitSurface(e->reponses[2], NULL, ecran, &e->pos_reponse3txt);
                }
                break;
                default: {
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse1);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse2);
                    SDL_BlitSurface(e->button, NULL, ecran, &e->pos_reponse3);
                    SDL_BlitSurface(e->reponses[0], NULL, ecran, &e->pos_reponse1txt);
                    SDL_BlitSurface(e->reponses[1], NULL, ecran, &e->pos_reponse2txt);
                    SDL_BlitSurface(e->reponses[2], NULL, ecran, &e->pos_reponse3txt);
                }
            }
            break;
        }
    }

    SDL_Flip(ecran);
    for (int i = 0; i < maxAnimations; i++) { 
        SDL_BlitSurface(animations[i], NULL, ecran, &posAnimation);
        SDL_Flip(ecran);
        SDL_Delay(400);
    
    }
}

void verify_enigme(enigmetf *e, SDL_Surface *ecran){
    SDL_Rect pos = {150, 200}; 
    if (e->positionVraiReponse == e->pos_selected) {
       // printf("You win\n");
        SDL_Surface *win = IMG_Load("img/youwin.png");
        SDL_BlitSurface(win, NULL, ecran, &pos);
        SDL_Flip(ecran);
    } else {
        SDL_Surface *lost = IMG_Load("img/youlost.png");
        SDL_BlitSurface(lost, NULL, ecran, &pos);
        SDL_Flip(ecran);
       // printf("You lost\n");
    }
    SDL_Delay(1700);
}

void free_Surface_enigme(enigmetf e){
    SDL_FreeSurface(e.background);
    SDL_FreeSurface(e.question);
    SDL_FreeSurface(e.reponses[0]);
    SDL_FreeSurface(e.reponses[1]);
    SDL_FreeSurface(e.reponses[2]);
    SDL_FreeSurface(e.button);
    SDL_FreeSurface(e.button_s);
    SDL_FreeSurface(e.afficherTemps);
}

