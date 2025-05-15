#ifndef TIME_H
// #ifndef TIME_H: Hadi tshik itha fichier time.h msh mdefinich mn kbal bash njiw nshriwh mara w7da bark
#define TIME_H
// #define TIME_H: Hadi tdefini TIME_H bash nshriw fichier mara w7da

#include <SDL/SDL.h>
// #include <SDL/SDL.h>: Hadi tshri bibliothèque SDL bash nista3mlou fonctions t3ha

typedef struct {
    // typedef struct: Hna nbashrou ncreew structure esma Timer
    Uint32 startTime;
    // Uint32 startTime: Variable t5azen wa9t lbdya mta3 timer (b milliseconds)
    Uint32 pausedTime;
    // Uint32 pausedTime: Variable t5azen wa9t lwa9fa mta3 timer (itha kan paused)
    int paused;
    // int paused: Variable tshri itha timer paused (1) wla la (0)
} Timer;
// } Timer: Hadi smya t3 structure, nsta3mlouha b Timer

void Timer_Init(Timer *timer);
// void Timer_Init(Timer *timer): Fonction tbdl timer b valeurs initiales
void Timer_Start(Timer *timer);
// void Timer_Start(Timer *timer): Fonction tbda timer mn jdid
void Timer_Pause(Timer *timer);
// void Timer_Pause(Timer *timer): Fonction tw9f timer
void Timer_Resume(Timer *timer);
// void Timer_Resume(Timer *timer): Fonction tkomel timer mn b3d lwa9fa
Uint32 Timer_GetTime(Timer *timer);
// Uint32 Timer_GetTime(Timer *timer): Fonction traja3 wa9t l3ab (b milliseconds)

#endif
// #endif: Hadi t5ls tshrik t3 fichier time.h
