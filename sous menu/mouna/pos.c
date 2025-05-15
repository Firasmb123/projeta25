#include "pos.h"

SDL_Rect getPuzzleBtnPos(int btnWidth, int btnHeight) {
    SDL_Rect pos;
    pos.x = SCREEN_WIDTH / 2 - btnWidth - 20;
    pos.y = SCREEN_HEIGHT / 2 - btnHeight / 2;
    pos.w = btnWidth;
    pos.h = btnHeight;
    return pos;
}

SDL_Rect getQcmBtnPos(int btnWidth, int btnHeight) {
    SDL_Rect pos;
    pos.x = SCREEN_WIDTH / 2 + 20;
    pos.y = SCREEN_HEIGHT / 2 - btnHeight / 2;
    pos.w = btnWidth;
    pos.h = btnHeight;
    return pos;
}
