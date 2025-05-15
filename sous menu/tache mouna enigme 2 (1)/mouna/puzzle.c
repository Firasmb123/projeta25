#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "puzzle.h"

#define SCREEN_WIDTH 1700
#define SCREEN_HEIGHT 900

int InitPuzzle(Puzzle* puzzle, const char* imagePath, int n) {
    if (n < 2 || n > 10) {
        printf("ERROR: Puzzle size must be between 2 and 10.\n");
        return 1;
    }

    puzzle->numRows = n;
    puzzle->numCols = n;
    puzzle->fullImage = IMG_Load(imagePath);
    if (!puzzle->fullImage) {
        printf("ERROR: Failed to load image %s: %s\n", imagePath, IMG_GetError());
        return 1;
    }

    // Use image dimensions instead of hardcoding 600x600
    puzzle->pieceWidth = puzzle->fullImage->w / puzzle->numCols;
    puzzle->pieceHeight = puzzle->fullImage->h / puzzle->numRows;

    // Allocate pieces array
    puzzle->pieces = (PuzzlePiece**)malloc(puzzle->numRows * sizeof(PuzzlePiece*));
    if (!puzzle->pieces) {
        printf("ERROR: Failed to allocate pieces array.\n");
        SDL_FreeSurface(puzzle->fullImage);
        return 1;
    }

    for (int i = 0; i < puzzle->numRows; i++) {
        puzzle->pieces[i] = (PuzzlePiece*)malloc(puzzle->numCols * sizeof(PuzzlePiece));
        if (!puzzle->pieces[i]) {
            printf("ERROR: Failed to allocate pieces row %d.\n", i);
            for (int j = 0; j < i; j++) {
                free(puzzle->pieces[j]);
            }
            free(puzzle->pieces);
            SDL_FreeSurface(puzzle->fullImage);
            return 1;
        }
    }

    srand(time(NULL));

    // Generate random positions across the entire screen
    int positions[puzzle->numRows * puzzle->numCols][2];
    int index = 0;
    for (int i = 0; i < puzzle->numRows; i++) {
        for (int j = 0; j < puzzle->numCols; j++) {
            positions[index][0] = 10 + (rand() % (SCREEN_WIDTH - puzzle->pieceWidth - 20));
            positions[index][1] = 10 + (rand() % (SCREEN_HEIGHT - puzzle->pieceHeight - 20));
            index++;
        }
    }

    // Shuffle positions
    for (int i = puzzle->numRows * puzzle->numCols - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tempX = positions[i][0];
        int tempY = positions[i][1];
        positions[i][0] = positions[j][0];
        positions[i][1] = positions[j][1];
        positions[j][0] = tempX;
        positions[j][1] = tempY;
    }

    // Initialize pieces
    index = 0;
    for (int i = 0; i < puzzle->numRows; i++) {
        for (int j = 0; j < puzzle->numCols; j++) {
            PuzzlePiece* piece = &puzzle->pieces[i][j];
            piece->image = SDL_CreateRGBSurface(0, puzzle->pieceWidth, puzzle->pieceHeight, 32, 
                                               puzzle->fullImage->format->Rmask,
                                               puzzle->fullImage->format->Gmask,
                                               puzzle->fullImage->format->Bmask,
                                               puzzle->fullImage->format->Amask);
            if (!piece->image) {
                printf("ERROR: Failed to create piece surface: %s\n", SDL_GetError());
                FreePuzzle(puzzle);
                return 1;
            }

            SDL_Rect srcRect = {j * puzzle->pieceWidth, i * puzzle->pieceHeight, puzzle->pieceWidth, puzzle->pieceHeight};
            SDL_BlitSurface(puzzle->fullImage, &srcRect, piece->image, NULL);

            piece->currentPos.x = positions[index][0];
            piece->currentPos.y = positions[index][1];
            piece->currentPos.w = puzzle->pieceWidth;
            piece->currentPos.h = puzzle->pieceHeight;
            index++;

            // Center target area in the window
            int targetAreaWidth = puzzle->pieceWidth * puzzle->numCols;
            int targetAreaHeight = puzzle->pieceHeight * puzzle->numRows;
            int startX = (SCREEN_WIDTH - targetAreaWidth) / 2;
            int startY = (SCREEN_HEIGHT - targetAreaHeight) / 2;
            piece->targetPos.x = startX + j * puzzle->pieceWidth;
            piece->targetPos.y = startY + i * puzzle->pieceHeight;
            piece->targetPos.w = puzzle->pieceWidth;
            piece->targetPos.h = puzzle->pieceHeight;

            piece->isPlaced = 0;
        }
    }

    puzzle->solved = 0;
    Timer_Init(&puzzle->timer);
    Timer_Start(&puzzle->timer);

    return 0;
}

void RenderPuzzle(SDL_Surface* screen, Puzzle* puzzle) {
    for (int i = 0; i < puzzle->numRows; i++) {
        for (int j = 0; j < puzzle->numCols; j++) {
            PuzzlePiece* piece = &puzzle->pieces[i][j];
            if (piece->image) {
                SDL_BlitSurface(piece->image, NULL, screen, &piece->currentPos);
            }
        }
    }
}

void FreePuzzle(Puzzle* puzzle) {
    if (puzzle->pieces) {
        for (int i = 0; i < puzzle->numRows; i++) {
            if (puzzle->pieces[i]) {
                for (int j = 0; j < puzzle->numCols; j++) {
                    if (puzzle->pieces[i][j].image) {
                        SDL_FreeSurface(puzzle->pieces[i][j].image);
                        puzzle->pieces[i][j].image = NULL;
                    }
                }
                free(puzzle->pieces[i]);
                puzzle->pieces[i] = NULL;
            }
        }
        free(puzzle->pieces);
        puzzle->pieces = NULL;
    }
    if (puzzle->fullImage) {
        SDL_FreeSurface(puzzle->fullImage);
        puzzle->fullImage = NULL;
    }
    puzzle->numRows = 0;
    puzzle->numCols = 0;
    puzzle->pieceWidth = 0;
    puzzle->pieceHeight = 0;
    puzzle->solved = 0;
}

int CheckPuzzleSolved(Puzzle* puzzle) {
    for (int i = 0; i < puzzle->numRows; i++) {
        for (int j = 0; j < puzzle->numCols; j++) {
            if (!puzzle->pieces[i][j].isPlaced) {
                return 0;
            }
        }
    }
    return 1;
}
