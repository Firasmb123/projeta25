#include "time.h"

void Timer_Init(Timer *timer) {
    timer->startTime = 0;
    timer->pausedTime = 0;
    timer->paused = 0;
}

void Timer_Start(Timer *timer) {
    timer->startTime = SDL_GetTicks();
    timer->paused = 0;
}

void Timer_Pause(Timer *timer) {
    if (!timer->paused) {
        timer->pausedTime = SDL_GetTicks() - timer->startTime;
        timer->paused = 1;
    }
}

void Timer_Resume(Timer *timer) {
    if (timer->paused) {
        timer->startTime = SDL_GetTicks() - timer->pausedTime;
        timer->paused = 0;
    }
}

Uint32 Timer_GetTime(Timer *timer) {
    if (timer->paused) {
        return timer->pausedTime;
    } else {
        return SDL_GetTicks() - timer->startTime;

    }
}
