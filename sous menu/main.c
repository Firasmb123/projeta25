#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include "menu.h"
#include "pos.h"

int main(int argc, char** argv) {
    SDL_Surface *screenSurface = NULL;
    SDL_Surface *backgroundImage = NULL;
    SDL_Surface *puzzleImage = NULL;
    SDL_Surface *quizImage = NULL;
    SDL_Surface *optionA = NULL;
    SDL_Surface *optionB = NULL;
    SDL_Surface *optionC = NULL;
    Mix_Music *backgroundMusic = NULL;
    Mix_Music *drumMusic = NULL;
    SDL_Surface *textSurface = NULL;
    TTF_Font *font = NULL;
    SDL_Surface *questionText = NULL;
    SDL_Surface *answerText = NULL;

    char questionMessage[] = "";
    char answerMessage[] = "";

    SDL_Event event;
    int quit = 1;
    int hideOptions = 0;
    int showText = 0;
    int runPuzzle = 0;

    if (!initialize_game(&screenSurface, &backgroundImage, &puzzleImage, &quizImage, &optionA, &optionB, &optionC, 
                         &backgroundMusic, &drumMusic, &font, &textSurface)) {
        fprintf(stderr, "Initialization failed, exiting.\n");
        return 1;
    }

    // Use pos.c functions for button positions
    SDL_Rect puzzleRect = getPuzzleBtnPos(500, 500); // Assuming button size 500x500
    SDL_Rect quizRect = getQcmBtnPos(500, 500);
    printf("Puzzle button pos: (%d, %d, %d, %d)\n", puzzleRect.x, puzzleRect.y, puzzleRect.w, puzzleRect.h);
    printf("Quiz button pos: (%d, %d, %d, %d)\n", quizRect.x, quizRect.y, quizRect.w, quizRect.h);

    SDL_Rect backgroundRect = {0, 0, 1700, 900};
    SDL_Rect optionARect = {400, (900 - 100) / 2 - 150, 200, 100};
    SDL_Rect optionBRect = {400, (900 - 100) / 2, 200, 100};
    SDL_Rect optionCRect = {400, (900 - 100) / 2 + 150, 200, 100};
    SDL_Rect secondQuizRect = {(1700 - 500) / 2, 20, 500, 500};
    SDL_Color questionTextColor = {255, 0, 0};
    SDL_Color answerTextColor = {0, 255, 0};
    SDL_Rect textRect = {900, 200, 500, 100};
    SDL_Rect answerTextRect = {900, 300, 500, 100};

    while (quit) {
        display_main_screen(&event, &quit, &showText, backgroundMusic, drumMusic, &puzzleRect, &quizRect, 
                           puzzleImage, quizImage, backgroundImage, screenSurface, &backgroundRect, &hideOptions, 
                           optionA, optionB, optionC, &optionARect, &optionBRect, &optionCRect, &runPuzzle);

        if (hideOptions) {
            run_quiz(screenSurface, font, &quit);
            hideOptions = 0; // Return to main menu
        } else if (runPuzzle) {
            printf("Running puzzle\n");
            RunPuzzle(screenSurface);
            runPuzzle = 0; // Return to main menu
        } else {
            display_question_screen(screenSurface, backgroundImage, puzzleImage, quizImage, &backgroundRect, &puzzleRect, 
                                   &quizRect, &secondQuizRect, font, showText, &hideOptions, optionA, optionB, optionC, 
                                   &optionARect, &optionBRect, &optionCRect, &textRect, questionText, 
                                   questionTextColor, questionMessage, &answerTextRect, answerText, 
                                   answerTextColor, answerMessage);
        }
    }

    cleanup(backgroundImage, puzzleImage, quizImage, backgroundMusic, optionA, optionB, optionC, textSurface);
    return 0;
}
