#ifndef POS_H
// #ifndef POS_H: Hadi tshik itha fichier pos.h msh mdefinich mn kbal
#define POSIX_H
// #define POSIX_H: Hadi tdefini POSIX_H bash nshriw fichier mara w7da

#include <SDL/SDL.h>
// #include <SDL/SDL.h>: Hadi tshri bibliothèque SDL bash nsta3mlou SDL_Rect

#define SCREEN_WIDTH 1700
// #define SCREEN_WIDTH 1700: Hadi tdefini largeur t3 écran b 1700 pixels
#define SCREEN_HEIGHT 900
// #define SCREEN_HEIGHT 900: Hadi tdefini hauteur t3 écran b 900 pixels

#define BOX_WIDTH 600
// #define BOX_WIDTH 600: Hadi tdefini largeur t3 boîte t3 interface
#define BOX_HEIGHT 200
// #define BOX_HEIGHT 200: Hadi tdefini hauteur t3 boîte t3 interface
#define BOX_POS_X (SCREEN_WIDTH / 2 - BOX_WIDTH / 2)
// #define BOX_POS_X ...: Hadi tdefini position X t3 boîte (wst écran)
#define BOX_POS_Y (SCREEN_HEIGHT / 2 - BOX_HEIGHT / 2)
// #define BOX_POS_Y ...: Hadi tdefini position Y t3 boîte (wst écran)

#define TEXT_POS_X (BOX_POS_X + BOX_WIDTH / 2)
// #define TEXT_POS_X ...: Hadi tdefini position X t3 texte (wst boîte)
#define TEXT_POS_Y (BOX_POS_Y + BOX_HEIGHT / 2)
// #define TEXT_POS_Y ...: Hadi tdefini position Y t3 texte (wst boîte)

#define BUTTON_POS_X (SCREEN_WIDTH / 2 - 100)
// #define BUTTON_POS_X ...: Hadi tdefini position X t3 bouton
#define BUTTON_POS_Y (BOX_POS_Y + BOX_HEIGHT + 20)
// #define BUTTON_POS_Y ...: Hadi tdefini position Y t3 bouton (t7t boîte)

SDL_Rect getPuzzleBtnPos(int btnWidth, int btnHeight);
// SDL_Rect getPuzzleBtnPos(int btnWidth, int btnHeight): Fonction traja3 position bouton puzzle
SDL_Rect getQcmBtnPos(int btnWidth, int btnHeight);
// SDL_Rect getQcmBtnPos(int btnWidth, int btnHeight): Fonction traja3 position bouton QCM

#endif
// #endif: Hadi t5ls tshrik t3 fichier pos.h
