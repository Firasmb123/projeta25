#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <time.h>
#include "entity.h"
#include "ennemy.h"
#include "stage2.h"

void scroll_background(SDL_Rect *background_pos, SDL_Rect *player_pos, int screen_width, int screen_height, int scroll_speed, int background_width, int background_height) {
    // Défilement horizontal
    if (player_pos->x >= screen_width - player_pos->w) {
        if (background_pos->x > -(background_width - screen_width)) {
            background_pos->x -= scroll_speed;
            
	printf("Scrolling background left: %d\n", background_pos->x);
        }
    } else if (player_pos->x <= 0) {
        if (background_pos->x < 0) {
            background_pos->x += scroll_speed;
            printf("Scrolling background right: %d\n", background_pos->x);
        }
    }

    // Défilement vertical
    if (player_pos->y >= screen_height - player_pos->h) {
        if (background_pos->y > -(background_height - screen_height)) {
            background_pos->y -= scroll_speed;
            printf("Scrolling background up: %d\n", background_pos->y);
        }
    } else if (player_pos->y <= 0) {
        if (background_pos->y < 0) {
            background_pos->y += scroll_speed;
            printf("Scrolling background down: %d\n", background_pos->y);
        }
    }
}

int main() {
    int hauteur_fenetre = 902, done = 1, a = 1, an = 1,
        largeur_fenetre = 1500;

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0) {
        printf("Unable to initialize SDL or SDL_ttf: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *screen = SDL_SetVideoMode(largeur_fenetre, hauteur_fenetre, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (screen == NULL) {
        printf("Unable to set video mode: %s\n", SDL_GetError());
        return 1;
    }

    SDL_WM_SetCaption("Firasennemie2025", NULL);

    SDL_Surface *background = IMG_Load("background.jpg");
    if (!background) {
        printf("Failed to load background image: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Surface *sorry = IMG_Load("youlost.png");
    if (!sorry) {
        printf("Failed to load sorry image: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Surface *win = IMG_Load("win.jpg");
    if (!win) {
        printf("Failed to load win image: %s\n", IMG_GetError());
        return 1;
    }

    // Spécifiez le chemin absolu vers la police
    TTF_Font *font = TTF_OpenFont("angelina.TTF", 24);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Color textColor = {255, 255, 255, 0}; // White color with unused initialized
    SDL_Rect background_pos = {0, 0, largeur_fenetre, hauteur_fenetre};
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

    int scroll_speed = 10; // Speed of scrolling
    Uint32 startTime = SDL_GetTicks(); // Start time
    SDL_Event event;
    while (done) {
        SDL_PollEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                done = 0;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_RIGHT:
                        if (p.perso_pos.x < largeur_fenetre - p.perso_pos.w) {
                            p.perso_pos.x += 10;
                            printf("Moving player right: %d\n", p.perso_pos.x);
                        }
                        p.Vscore += 1;
                        break;
                    case SDLK_LEFT:
                        if (p.perso_pos.x > 0) {
                            p.perso_pos.x -= 10;
                            printf("Moving player left: %d\n", p.perso_pos.x);
                        }
                        p.Vscore += 1;
                        break;
                    case SDLK_UP:
                        if (p.perso_pos.y > 0) {
                            p.perso_pos.y -= 10;
                            printf("Moving player up: %d\n", p.perso_pos.y);
                        }
                        p.Vscore += 1;
                        break;
                    case SDLK_DOWN:
                        if (p.perso_pos.y < hauteur_fenetre - p.perso_pos.h) {
                            p.perso_pos.y += 10;
                            printf("Moving player down: %d\n", p.perso_pos.y);
                        }
                        p.Vscore += 1;
                        break;
                    case SDLK_g:
                        rungame2();
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
                break;
        }

        // Appeler la fonction de défilement
        scroll_background(&background_pos, &p.perso_pos, largeur_fenetre, hauteur_fenetre, scroll_speed, background->w, background->h);

        printf("Background position: %d, %d\n", background_pos.x, background_pos.y);
        printf("Player position: %d, %d\n", p.perso_pos.x, p.perso_pos.y);

        update_entity(&e);
        update_ennemie(&en);
        updateEnnemi(&en, p.perso_pos);

        // Render the background
        SDL_BlitSurface(background, NULL, screen, &background_pos);

        // Calculate elapsed time
        Uint32 elapsedTime = (SDL_GetTicks() - startTime) / 1000;
        char timeText[20];
        sprintf(timeText, "Time: %02d:%02d", elapsedTime / 60, elapsedTime % 60);

        // Render time text
        SDL_Surface *timeSurface = TTF_RenderText_Solid(font, timeText, textColor);
        SDL_Rect timePos = {largeur_fenetre - 150, 10, 0, 0}; // Position en haut à droite
        SDL_BlitSurface(timeSurface, NULL, screen, &timePos);
        SDL_FreeSurface(timeSurface);

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

        SDL_Delay(20);
        SDL_Flip(screen);
    }

    SDL_FreeSurface(background);
    SDL_FreeSurface(sorry);
    SDL_FreeSurface(win);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
