#ifndef PUZZLE_H
// #ifndef PUZZLE_H: Hadi tshik itha fichier puzzle.h msh mdefinich mn kbal
#define PUZZLE_H
// #define PUZZLE_H: Hadi tdefini PUZZLE_H bash nshriw fichier mara w7da

#include <SDL/SDL.h>
// #include <SDL/SDL.h>: Hadi tshri bibliothèque SDL bash nsta3mlou SDL_Surface w SDL_Rect
#include <SDL/SDL_image.h>
// #include <SDL/SDL_image.h>: Hadi tshri bibliothèque SDL_image bash nshriw images
#include "time.h" // Ajout pour le timer
// #include "time.h": Hadi tshri fichier time.h bash nsta3mlou Timer

#define SCREEN_WIDTH 1700
// #define SCREEN_WIDTH 1700: Hadi tdefini largeur t3 écran b 1700 pixels
#define SCREEN_HEIGHT 900
// #define SCREEN_HEIGHT 900: Hadi tdefini hauteur t3 écran b 900 pixels

typedef struct {
    // typedef struct: Hna nbashrou ncreew structure esma PuzzlePiece
    SDL_Surface* image;     // Image de la pièce
    // SDL_Surface* image: Variable t5azen image t3 pièce puzzle
    SDL_Rect currentPos;    // Position actuelle
    // SDL_Rect currentPos: Variable t5azen position l7alia t3 pièce
    SDL_Rect targetPos;     // Position cible (pour reconstitution)
    // SDL_Rect targetPos: Variable t5azen position cible t3 pièce
    int isPlaced;           // Indique si la pièce est bien placée
    // int isPlaced: Variable tshri itha pièce mawjuda f positionha (1) wla la (0)
} PuzzlePiece;
// } PuzzlePiece: Hadi smya t3 structure, nsta3mlouha b PuzzlePiece

typedef struct {
    // typedef struct: Hna nbashrou ncreew structure esma Puzzle
    PuzzlePiece** pieces;   // Matrice dynamique des pièces
    // PuzzlePiece** pieces: Variable t5azen matrice t3 pièces puzzle
    SDL_Surface* fullImage; // Image complète chargée
    // SDL_Surface* fullImage: Variable t5azen image complète t3 puzzle
    int numRows;            // Nombre de lignes
    // int numRows: Variable t5azen 3adad lignes t3 puzzle
    int numCols;            // Nombre de colonnes
    // int numCols: Variable t5azen 3adad colonnes t3 puzzle
    int pieceWidth;         // Largeur d'une pièce
    // int pieceWidth: Variable t5azen largeur t3 pièce w7da
    int pieceHeight;        // Hauteur d'une pièce
    // int pieceHeight: Variable t5azen hauteur t3 pièce w7da
    int solved;             // Indique si le puzzle est résolu
    // int solved: Variable tshri itha puzzle résolu (1) wla la (0)
    Timer timer;            // Timer pour chronométrer le temps
    // Timer timer: Variable t5azen timer t3 puzzle
} Puzzle;
// } Puzzle: Hadi smya t3 structure, nsta3mlouha b Puzzle

int InitPuzzle(Puzzle* puzzle, const char* imagePath, int n);
// int InitPuzzle(Puzzle* puzzle, const char* imagePath, int n): Fonction tbdl puzzle
void RenderPuzzle(SDL_Surface* screen, Puzzle* puzzle);
// void RenderPuzzle(SDL_Surface* screen, Puzzle* puzzle): Fonction taffichi puzzle 3la écran
void FreePuzzle(Puzzle* puzzle);
// void FreePuzzle(Puzzle* puzzle): Fonction t7rr mémoire t3 puzzle
int CheckPuzzleSolved(Puzzle* puzzle);
// int CheckPuzzleSolved(Puzzle* puzzle): Fonction tshik itha puzzle résolu

#endif
// #endif: Hadi t5ls tshrik t3 fichier puzzle.h
