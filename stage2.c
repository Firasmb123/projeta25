#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <time.h>
#include "entity.h"
#include "ennemy.h"

int rungame2() {
    int done = 1;
    int a = 1, an = 1;
    int largeur_fenetre = 1500, hauteur_fenetre = 600;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        printf("Erreur initialisation SDL : %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *screen = SDL_SetVideoMode(largeur_fenetre, hauteur_fenetre, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!screen) {
        printf("Erreur création fenêtre : %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_WM_SetCaption("Stage 2 - 2024", NULL);

    SDL_Surface *background = IMG_Load("432.jpg");
    SDL_Surface *sorry = IMG_Load("youlost.png");
    SDL_Surface *win = IMG_Load("win.jpg");

    if (!background) {
        printf("Erreur chargement de l'image de fond : %s\n", IMG_GetError());
        return 1;
    }
    if (!sorry) {
        printf("Erreur chargement de l'image 'youlost' : %s\n", IMG_GetError());
        return 1;
    }
    if (!win) {
        printf("Erreur chargement de l'image 'win' : %s\n", IMG_GetError());
        return 1;
    }

    SDL_Rect background_pos = {0, 0, 0, 0};
    SDL_Rect pos_sorry = {300, 300, 0, 0};
    SDL_Rect pos_win = {300, 300, 0, 0};

    personnage p;
    entity e;
    Ennemie en;
    coeur c;

    initialiser_perso(&p);
    initentity(&e);
    initEnnemi(&en);
    initcoeur(&c);

    SDL_Event event;

    while (done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = 0;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_RIGHT:
                        p.perso_pos.x += 10;
                        p.Vscore += 1;
                        break;
                    case SDLK_LEFT:
                        p.perso_pos.x -= 10;
                        p.Vscore += 1;
                        break;
                    case SDLK_UP:
                        p.perso_pos.y -= 10;
                        p.Vscore += 1;
                        break;
                    case SDLK_DOWN:
                        p.perso_pos.y += 10;
                        p.Vscore += 1;
                        break;
                    case SDLK_f: {
                        Uint32 now = SDL_GetTicks();
                        if (abs(p.perso_pos.x - en.pos_image_EN.x) <= 30 && en.vie > 0) {
                            if (now - en.last_hit_time > 500) {
                                en.vie--;
                                en.last_hit_time = now;
                                printf("Ennemi touché ! Vie restante : %d\n", en.vie);

                                if (en.vie == 0) {
                                    en.show = 0;
                                }
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        update_entity(&e);
        update_ennemie(&en);
        updateEnnemi(&en, p.perso_pos);

        SDL_BlitSurface(background, NULL, screen, &background_pos);

        if (collisionTri(e, p.perso_pos)) {
            a = gestion(&e);
            p.Vscore += 500;
        }

        if (a) {
            afficherentity(e, screen);
        }

        if (detect_collision_ennemie(&p, &en))
            an = gerer(&en);

        if (en.vie > 0 && an) {
            afficher_perso(p, screen);
            afficherEnnemi(en, screen);
            affichercoeur(c, screen, en.vie);
        } else if (en.vie <= 0) {
            SDL_BlitSurface(win, NULL, screen, &pos_win);
        } else {
            SDL_BlitSurface(sorry, NULL, screen, &pos_sorry);
        }

        SDL_Flip(screen);
        SDL_Delay(20);
    }

    SDL_FreeSurface(background);
    SDL_FreeSurface(sorry);
    SDL_FreeSurface(win);
    SDL_Quit();

    return 0;
}
