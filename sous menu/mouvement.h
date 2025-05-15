#ifndef MOUVEMENT_H
// #ifndef MOUVEMENT_H: Hadi tshik itha fichier mouvement.h msh mdefinich mn kbal
#define MOUVEMENT_H
// #define MOUVEMENT_H: Hadi tdefini MOUVEMENT_H bash nshriw fichier mara w7da

#include "puzzle.h"
// #include "puzzle.h": Hadi tshri fichier puzzle.h bash nsta3mlou structure Puzzle

typedef struct {
    // typedef struct: Hna nbashrou ncreew structure esma MovementManager
    PuzzlePiece* selectedPiece; // Pièce actuellement sélectionnée
    // PuzzlePiece* selectedPiece: Variable t5azen pièce l7alia m5tara
    int offsetX;                // Décalage X entre la souris et la pièce
    // int offsetX: Variable t5azen far9 X bin souris w pièce
    int offsetY;                // Décalage Y entre la souris et la pièce
    // int offsetY: Variable t5azen far9 Y bin souris w pièce
} MovementManager;
// } MovementManager: Hadi smya t3 structure, nsta3mlouha b MovementManager

void InitMovementManager(MovementManager* manager);
// void InitMovementManager(MovementManager* manager): Fonction tbdl MovementManager
void HandleMouseDown(MovementManager* manager, Puzzle* puzzle, int mouseX, int mouseY);
// void HandleMouseDown(...): Fonction t3alj click souris
void HandleMouseMove(MovementManager* manager, int mouseX, int mouseY);
// void HandleMouseMove(...): Fonction t3alj mouvement souris
void HandleMouseUp(MovementManager* manager);
// void HandleMouseUp(...): Fonction t3alj release souris

#endif
// #endif: Hadi t5ls tshrik t3 fichier mouvement.h
