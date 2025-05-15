#ifndef POS_H
#define POS_H

#include <SDL/SDL.h>

#define SCREEN_WIDTH 1700
#define SCREEN_HEIGHT 900

#define BOX_WIDTH 600
#define BOX_HEIGHT 200
#define BOX_POS_X (SCREEN_WIDTH / 2 - BOX_WIDTH / 2)
#define BOX_POS_Y (SCREEN_HEIGHT / 2 - BOX_HEIGHT / 2)

#define TEXT_POS_X (BOX_POS_X + BOX_WIDTH / 2)
#define TEXT_POS_Y (BOX_POS_Y + BOX_HEIGHT / 2)

#define BUTTON_POS_X (SCREEN_WIDTH / 2 - 100)
#define BUTTON_POS_Y (BOX_POS_Y + BOX_HEIGHT + 20)

SDL_Rect getPuzzleBtnPos(int btnWidth, int btnHeight);
SDL_Rect getQcmBtnPos(int btnWidth, int btnHeight);

#endif
