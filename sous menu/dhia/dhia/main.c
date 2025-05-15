#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "enigmetf.h"

int main() {
    SDL_Surface *ecran = NULL;
    ecran = SDL_SetVideoMode(750, 650, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    int continuer = 1;
    SDL_Event event;
    char nomfichier[100] = "questions.txt";
    
    enigmetf e;
    InitEnigme(&e, nomfichier);

    while (continuer) {
        afficherEnigme(&e, ecran);
    
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    free_Surface_enigme(e);
                    continuer = 0;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {    
                        case SDLK_a:
                            e.pos_selected = 1;
                            break; 
                        case SDLK_b:
                            e.pos_selected = 2;
                            break; 
                        case SDLK_c:
                            e.pos_selected = 3;
                            break; 
                        case SDLK_RETURN:
                            verify_enigme(&e, ecran); 
                            return 0;
                            break;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    printf("Mouse position: %d %d\n", event.motion.x, event.motion.y);
                    break;
                case SDL_MOUSEBUTTONUP:
                    if ((event.motion.x > 5 && event.motion.x < 245) && (event.motion.y > 368 && event.motion.y < 456)) {
                        e.pos_selected = 1;
                        if (event.button.button == SDL_BUTTON_LEFT) {
                            verify_enigme(&e, ecran); 
                            return 0;
                        } 	 
                    } else if ((event.motion.x > 411 && event.motion.x < 645) && (event.motion.y > 371 && event.motion.y < 439)) {
                        e.pos_selected = 2;
                        if (event.button.button == SDL_BUTTON_LEFT) {	
                            verify_enigme(&e, ecran); 
                            return 0;
                        }
                    } else if ((event.motion.x > 202 && event.motion.x < 607) && (event.motion.y > 525 && event.motion.y < 600)) {
                        e.pos_selected = 3;
                        if (event.button.button == SDL_BUTTON_LEFT) {
                            verify_enigme(&e, ecran); 
                            return 0;
                        } 					
                    }
                    break;
            }
        }
    }
    return 0;
}

