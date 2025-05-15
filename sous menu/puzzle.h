#ifndef PUZZLE_H
#define PUZZLE_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "time.h" // Include time.h for Timer definition

typedef struct {
    SDL_Surface* image;
    SDL_Rect currentPos;
    SDL_Rect targetPos;
    int isPlaced;
} PuzzlePiece;

typedef struct {
    SDL_Surface* fullImage;
    PuzzlePiece** pieces;
    int numRows;
    int numCols;
    int pieceWidth;
    int pieceHeight;
    int solved;
    Timer timer; // Timer type now defined via time.h
} Puzzle;

int InitPuzzle(Puzzle* puzzle, const char* imagePath, int n, SDL_Surface* screen);
void RenderPuzzle(SDL_Surface* screen, Puzzle* puzzle);
void FreePuzzle(Puzzle* puzzle);
int CheckPuzzleSolved(Puzzle* puzzle);
void RunPuzzle(SDL_Surface* screen);

#endif
