#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "puzzle.h"
#include "mouvement.h"
#include "magnet.h"
#include "time.h"

#define SCREEN_WIDTH 1700
#define SCREEN_HEIGHT 900
#define ASSET_PATH "assets/"
#define PUZZLE_BG ASSET_PATH "bg_interface.png"
#define PUZZLE_IMAGES_COUNT 3

int InitPuzzle(Puzzle* puzzle, const char* imagePath, int n, SDL_Surface* screen) {
    printf("Initializing puzzle with image: %s, size: %dx%d\n", imagePath, n, n);
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
    printf("Loaded image %s: %dx%d\n", imagePath, puzzle->fullImage->w, puzzle->fullImage->h);

    puzzle->pieceWidth = puzzle->fullImage->w / puzzle->numCols;
    puzzle->pieceHeight = puzzle->fullImage->h / puzzle->numRows;
    printf("Piece size: %dx%d\n", puzzle->pieceWidth, puzzle->pieceHeight);

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

    int positions[puzzle->numRows * puzzle->numCols][2];
    int index = 0;
    for (int i = 0; i < puzzle->numRows; i++) {
        for (int j = 0; j < puzzle->numCols; j++) {
            positions[index][0] = 10 + (rand() % (SCREEN_WIDTH - puzzle->pieceWidth - 20));
            positions[index][1] = 10 + (rand() % (SCREEN_HEIGHT - puzzle->pieceHeight - 20));
            printf("Piece [%d][%d] random pos: (%d, %d)\n", i, j, positions[index][0], positions[index][1]);
            index++;
        }
    }

    for (int i = puzzle->numRows * puzzle->numCols - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tempX = positions[i][0];
        int tempY = positions[i][1];
        positions[i][0] = positions[j][0];
        positions[i][1] = positions[j][1];
        positions[j][0] = tempX;
        positions[j][1] = tempY;
    }

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
                printf("ERROR: Failed to create piece surface [%d][%d]: %s\n", i, j, SDL_GetError());
                FreePuzzle(puzzle);
                return 1;
            }

            // Convert to screen format
            SDL_Surface* temp = piece->image;
            piece->image = SDL_DisplayFormat(temp);
            SDL_FreeSurface(temp);
            if (!piece->image) {
                printf("ERROR: Failed to convert piece surface [%d][%d]: %s\n", i, j, SDL_GetError());
                FreePuzzle(puzzle);
                return 1;
            }

            SDL_Rect srcRect = {j * puzzle->pieceWidth, i * puzzle->pieceHeight, puzzle->pieceWidth, puzzle->pieceHeight};
            if (SDL_BlitSurface(puzzle->fullImage, &srcRect, piece->image, NULL) < 0) {
                printf("ERROR: Failed to blit piece [%d][%d]: %s\n", i, j, SDL_GetError());
                FreePuzzle(puzzle);
                return 1;
            }

            piece->currentPos.x = positions[index][0];
            piece->currentPos.y = positions[index][1];
            piece->currentPos.w = puzzle->pieceWidth;
            piece->currentPos.h = puzzle->pieceHeight;
            index++;

            int targetAreaWidth = puzzle->pieceWidth * puzzle->numCols;
            int targetAreaHeight = puzzle->pieceHeight * puzzle->numRows;
            int startX = (SCREEN_WIDTH - targetAreaWidth) / 2;
            int startY = (SCREEN_HEIGHT - targetAreaHeight) / 2;
            piece->targetPos.x = startX + j * puzzle->pieceWidth;
            piece->targetPos.y = startY + i * puzzle->pieceHeight;
            piece->targetPos.w = puzzle->pieceWidth;
            piece->targetPos.h = puzzle->pieceHeight;
            printf("Piece [%d][%d] target pos: (%d, %d)\n", i, j, piece->targetPos.x, piece->targetPos.y);

            piece->isPlaced = 0;
        }
    }

    puzzle->solved = 0;
    Timer_Init(&puzzle->timer);
    Timer_Start(&puzzle->timer);
    printf("Puzzle initialized successfully\n");

    return 0;
}

void RenderPuzzle(SDL_Surface* screen, Puzzle* puzzle) {
    printf("Rendering puzzle\n");
    for (int i = 0; i < puzzle->numRows; i++) {
        for (int j = 0; j < puzzle->numCols; j++) {
            PuzzlePiece* piece = &puzzle->pieces[i][j];
            if (piece->image) {
                printf("Blitting piece [%d][%d] at (%d, %d)\n", i, j, piece->currentPos.x, piece->currentPos.y);
                if (SDL_BlitSurface(piece->image, NULL, screen, &piece->currentPos) < 0) {
                    printf("ERROR: Failed to blit piece [%d][%d]: %s\n", i, j, SDL_GetError());
                }
            } else {
                printf("WARNING: Piece [%d][%d] has no image\n", i, j);
            }
        }
    }
}

void FreePuzzle(Puzzle* puzzle) {
    printf("Freeing puzzle resources\n");
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
                printf("Piece [%d][%d] not placed\n", i, j);
                return 0;
            }
        }
    }
    printf("Puzzle solved\n");
    return 1;
}

void RunPuzzle(SDL_Surface* screen) {
    printf("Starting RunPuzzle\n");
    printf("Screen format: bitsPerPixel=%d\n", screen->format->BitsPerPixel);
    SDL_Surface* bg = IMG_Load(PUZZLE_BG);
    if (!bg) {
        printf("ERROR: Failed to load %s: %s\n", PUZZLE_BG, IMG_GetError());
        return;
    }
    printf("Loaded background: %dx%d\n", bg->w, bg->h);

    TTF_Font* font = TTF_OpenFont(ASSET_PATH "arial.ttf", 24);
    if (!font) {
        printf("ERROR: Failed to load font: %s\n", TTF_GetError());
        SDL_FreeSurface(bg);
        return;
    }

    int quit = 0;
    while (!quit) {
        int n = 4;
        const char* puzzleImages[PUZZLE_IMAGES_COUNT] = {
            ASSET_PATH "puzzle.png",
            ASSET_PATH "puzzle2.png",
            ASSET_PATH "puzzle3.png"
        };
        srand(SDL_GetTicks());
        const char* selectedPuzzle = puzzleImages[rand() % PUZZLE_IMAGES_COUNT];
        printf("Selected puzzle image: %s\n", selectedPuzzle);

        Puzzle puzzle;
        if (InitPuzzle(&puzzle, selectedPuzzle, n, screen) != 0) {
            printf("ERROR: InitPuzzle failed\n");
            TTF_CloseFont(font);
            SDL_FreeSurface(bg);
            return;
        }

        MovementManager movementManager;
        InitMovementManager(&movementManager);
        int running = 1;
        while (running && !puzzle.solved) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = 0;
                    quit = 1;
                } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    printf("Mouse down at (%d, %d)\n", event.button.x, event.button.y);
                    HandleMouseDown(&movementManager, &puzzle, event.button.x, event.button.y);
                } else if (event.type == SDL_MOUSEMOTION && movementManager.selectedPiece) {
                    HandleMouseMove(&movementManager, event.motion.x, event.motion.y);
                } else if (event.type == SDL_MOUSEBUTTONUP) {
                    HandleMouseUp(&movementManager);
                    MagnetizePieces(&puzzle);
                    if (CheckPuzzleSolved(&puzzle)) {
                        puzzle.solved = 1;
                        Timer_Pause(&puzzle.timer);
                    }
                }
            }

            SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 255)); // Blue for debugging
            if (SDL_BlitSurface(bg, NULL, screen, NULL) < 0) {
                printf("ERROR: Failed to blit background: %s\n", SDL_GetError());
            }
            RenderPuzzle(screen, &puzzle);

            SDL_Color textColor = {255, 255, 255, 255};
            char timeMsg[50];
            Uint32 time = Timer_GetTime(&puzzle.timer) / 1000;
            sprintf(timeMsg, "Temps : %u s", time);
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, timeMsg, textColor);
            if (textSurface) {
                SDL_Rect textPos = {10, 10, 0, 0};
                SDL_BlitSurface(textSurface, NULL, screen, &textPos);
                SDL_FreeSurface(textSurface);
            } else {
                printf("WARNING: Failed to render timer text\n");
            }

            if (SDL_Flip(screen) < 0) {
                printf("ERROR: Failed to flip screen: %s\n", SDL_GetError());
            }
        }

        if (puzzle.solved) {
            int waiting = 1;
            while (waiting) {
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                        waiting = 0;
                        quit = 1;
                    } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
                        waiting = 0;
                    }
                }

                SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 255)); // Blue for debugging
                if (SDL_BlitSurface(bg, NULL, screen, NULL) < 0) {
                    printf("ERROR: Failed to blit background: %s\n", SDL_GetError());
                }
                RenderPuzzle(screen, &puzzle);
                SDL_Color textColor = {255, 255, 255, 255};
                char msg[50];
                Uint32 time = Timer_GetTime(&puzzle.timer) / 1000;
                sprintf(msg, "Bravo ! Puzzle résolu en %u s !", time);
                SDL_Surface* textSurface = TTF_RenderText_Solid(font, msg, textColor);
                if (textSurface) {
                    SDL_Rect textPos = {SCREEN_WIDTH / 2 - textSurface->w / 2, SCREEN_HEIGHT / 2 - textSurface->h / 2, 0, 0};
                    SDL_BlitSurface(textSurface, NULL, screen, &textPos);
                    SDL_FreeSurface(textSurface);
                } else {
                    printf("WARNING: Failed to render win message\n");
                }
                if (SDL_Flip(screen) < 0) {
                    printf("ERROR: Failed to flip screen: %s\n", SDL_GetError());
                }
                SDL_Delay(16);
            }
        }

        FreePuzzle(&puzzle);
    }

    TTF_CloseFont(font);
    SDL_FreeSurface(bg);
    printf("Exiting RunPuzzle\n");
}
