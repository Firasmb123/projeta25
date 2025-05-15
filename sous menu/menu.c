#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"

int initialize_game(SDL_Surface** screen, SDL_Surface** backgroundImage, SDL_Surface** puzzleImage, SDL_Surface** quizImage, 
                   SDL_Surface** optionA, SDL_Surface** optionB, SDL_Surface** optionC, Mix_Music** backgroundMusic, 
                   Mix_Music** drumMusic, TTF_Font** font, SDL_Surface** textSurface) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 0;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "TTF initialization failed: %s\n", TTF_GetError());
        SDL_Quit();
        return 0;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Mixer initialization failed: %s\n", Mix_GetError());
        TTF_Quit();
        SDL_Quit();
        return 0;
    }

    SDL_Surface* tempBackground = IMG_Load("background.png");
    if (!tempBackground) {
        fprintf(stderr, "Failed to load background.png: %s\n", IMG_GetError());
        cleanup(tempBackground, *puzzleImage, *quizImage, *backgroundMusic, *optionA, *optionB, *optionC, *textSurface);
        return 0;
    }
    printf("Loaded background.png: %dx%d\n", tempBackground->w, tempBackground->h);

    *puzzleImage = IMG_Load("puzzle.png");
    *quizImage = IMG_Load("QUIZ.png");
    *optionA = IMG_Load("A.png");
    *optionB = IMG_Load("B.png");
    *optionC = IMG_Load("C.png");

    *screen = SDL_SetVideoMode(1700, 900, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!*screen) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        cleanup(tempBackground, *puzzleImage, *quizImage, *backgroundMusic, *optionA, *optionB, *optionC, *textSurface);
        return 0;
    }

    *backgroundImage = SDL_CreateRGBSurface(0, 1700, 900, 32, 0, 0, 0, 0);
    if (!*backgroundImage) {
        fprintf(stderr, "Failed to create scaled background surface: %s\n", SDL_GetError());
        cleanup(tempBackground, *puzzleImage, *quizImage, *backgroundMusic, *optionA, *optionB, *optionC, *textSurface);
        return 0;
    }

    SDL_Rect srcRect = {0, 0, tempBackground->w, tempBackground->h};
    SDL_Rect dstRect = {0, 0, 1700, 900};
    if (SDL_BlitSurface(tempBackground, &srcRect, *backgroundImage, &dstRect) < 0) {
        fprintf(stderr, "Failed to scale background image: %s\n", SDL_GetError());
        SDL_FreeSurface(*backgroundImage);
        cleanup(tempBackground, *puzzleImage, *quizImage, *backgroundMusic, *optionA, *optionB, *optionC, *textSurface);
        return 0;
    }
    SDL_FreeSurface(tempBackground);
    printf("Scaled background to 1700x900\n");

    if (!*puzzleImage || !*quizImage || !*optionA || !*optionB || !*optionC) {
        fprintf(stderr, "Image load failed: %s\n", IMG_GetError());
        cleanup(*backgroundImage, *puzzleImage, *quizImage, *backgroundMusic, *optionA, *optionB, *optionC, *textSurface);
        return 0;
    }

    *backgroundMusic = Mix_LoadMUS("music.mp3");
    *drumMusic = Mix_LoadMUS("tombour.mp3");
    if (!*backgroundMusic || !*drumMusic) {
        fprintf(stderr, "Music load failed: %s\n", Mix_GetError());
        cleanup(*backgroundImage, *puzzleImage, *quizImage, *backgroundMusic, *optionA, *optionB, *optionC, *textSurface);
        return 0;
    }

    *font = TTF_OpenFont("arial.ttf", 100);
    if (!*font) {
        fprintf(stderr, "Font load failed: %s\n", TTF_GetError());
        cleanup(*backgroundImage, *puzzleImage, *quizImage, *backgroundMusic, *optionA, *optionB, *optionC, *textSurface);
        return 0;
    }

    return 1;
}

void display_main_screen(SDL_Event* event, int* quit, int* showText, Mix_Music* backgroundMusic, Mix_Music* drumMusic,
                        SDL_Rect* puzzleRect, SDL_Rect* quizRect, SDL_Surface* puzzleImage, SDL_Surface* quizImage,
                        SDL_Surface* backgroundImage, SDL_Surface* screenSurface, SDL_Rect* backgroundRect, 
                        int* hideOptions, SDL_Surface* optionA, SDL_Surface* optionB, SDL_Surface* optionC, 
                        SDL_Rect* optionARect, SDL_Rect* optionBRect, SDL_Rect* optionCRect, int* runPuzzle) {
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));
    SDL_BlitSurface(backgroundImage, NULL, screenSurface, backgroundRect);

    int mouseX, mouseY;
    while (SDL_PollEvent(event)) {
        switch (event->type) {
            case SDL_QUIT:
                *quit = 0;
                break;

            case SDL_MOUSEMOTION:
                mouseX = event->motion.x;
                mouseY = event->motion.y;
                printf("Mouse moved to: (%d, %d)\n", mouseX, mouseY);
                if (*hideOptions == 0 && *runPuzzle == 0) {
                    if (mouseX >= puzzleRect->x && mouseX <= puzzleRect->x + puzzleRect->w &&
                        mouseY >= puzzleRect->y && mouseY <= puzzleRect->y + puzzleRect->h) {
                        Mix_PlayMusic(backgroundMusic, 1);
                    }
                    if (mouseX >= quizRect->x && mouseX <= quizRect->x + quizRect->w &&
                        mouseY >= quizRect->y && mouseY <= quizRect->y + quizRect->h) {
                        Mix_PlayMusic(backgroundMusic, 1);
                    }
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                mouseX = event->motion.x;
                mouseY = event->motion.y;
                printf("Mouse clicked at: (%d, %d)\n", mouseX, mouseY);
                if (*hideOptions == 0 && *runPuzzle == 0) {
                    if (mouseX >= quizRect->x && mouseX <= quizRect->x + quizRect->w &&
                        mouseY >= quizRect->y && mouseY <= quizRect->y + quizRect->h) {
                        printf("Quiz button clicked\n");
                        *hideOptions = 1;
                        Mix_PlayMusic(drumMusic, -1);
                    }
                    if (mouseX >= puzzleRect->x && mouseX <= puzzleRect->x + puzzleRect->w &&
                        mouseY >= puzzleRect->y && mouseY <= puzzleRect->y + puzzleRect->h) {
                        printf("Puzzle button clicked\n");
                        *runPuzzle = 1;
                        Mix_PlayMusic(drumMusic, -1);
                    }
                }
                break;
        }
    }

    if (*hideOptions == 0 && *runPuzzle == 0) {
        SDL_BlitSurface(puzzleImage, NULL, screenSurface, puzzleRect);
        SDL_BlitSurface(quizImage, NULL, screenSurface, quizRect);
    }
    SDL_Flip(screenSurface);
}

void display_question_screen(SDL_Surface* screenSurface, SDL_Surface* backgroundImage, SDL_Surface* puzzleImage, 
                            SDL_Surface* quizImage, SDL_Rect* backgroundRect, SDL_Rect* puzzleRect, 
                            SDL_Rect* quizRect, SDL_Rect* secondQuizRect, TTF_Font* font, int showText, 
                            int* hideOptions, SDL_Surface* optionA, SDL_Surface* optionB, SDL_Surface* optionC, 
                            SDL_Rect* optionARect, SDL_Rect* optionBRect, SDL_Rect* optionCRect, SDL_Rect* textRect, 
                            SDL_Surface* questionText, SDL_Color textColor, const char* questionMessage, 
                            SDL_Rect* answerTextRect, SDL_Surface* answerText, SDL_Color answerTextColor, 
                            const char* answerMessage) {
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));
    SDL_BlitSurface(backgroundImage, NULL, screenSurface, backgroundRect);

    if (*hideOptions == 0) {
        SDL_BlitSurface(quizImage, NULL, screenSurface, quizRect);
        SDL_BlitSurface(puzzleImage, NULL, screenSurface, puzzleRect);
    } else {
        SDL_BlitSurface(optionA, NULL, screenSurface, optionARect);
        SDL_BlitSurface(optionB, NULL, screenSurface, optionBRect);
        SDL_BlitSurface(optionC, NULL, screenSurface, optionCRect);
        SDL_BlitSurface(quizImage, NULL, screenSurface, secondQuizRect);

        questionText = TTF_RenderText_Blended(font, questionMessage, textColor);
        if (questionText) {
            SDL_BlitSurface(questionText, NULL, screenSurface, textRect);
            SDL_FreeSurface(questionText);
        }

        answerText = TTF_RenderText_Blended(font, answerMessage, answerTextColor);
        if (answerText) {
            SDL_BlitSurface(answerText, NULL, screenSurface, answerTextRect);
            SDL_FreeSurface(answerText);
        }
    }

    SDL_Flip(screenSurface);
}

void cleanup(SDL_Surface* backgroundImage, SDL_Surface* puzzleImage, SDL_Surface* quizImage, Mix_Music* backgroundMusic, 
             SDL_Surface* optionA, SDL_Surface* optionB, SDL_Surface* optionC, SDL_Surface* textSurface) {
    if (backgroundImage) SDL_FreeSurface(backgroundImage);
    if (quizImage) SDL_FreeSurface(quizImage);
    if (puzzleImage) SDL_FreeSurface(puzzleImage);
    if (optionA) SDL_FreeSurface(optionA);
    if (optionB) SDL_FreeSurface(optionB);
    if (optionC) SDL_FreeSurface(optionC);
    if (textSurface) SDL_FreeSurface(textSurface);
    if (backgroundMusic) Mix_FreeMusic(backgroundMusic);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
}

void run_quiz(SDL_Surface* screen, TTF_Font* font, int* quit) {
    enigmetf e;
    char nomfichier[100] = "questions.txt";
    InitEnigme(&e, nomfichier);

    int continuer = 1;
    SDL_Event event;
    float scale_x = 1700.0f / 750.0f;
    float scale_y = 900.0f / 650.0f;

    while (continuer && *quit) {
        afficherEnigme(&e, screen);
        SDL_Flip(screen);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    free_Surface_enigme(e);
                    continuer = 0;
                    *quit = 0;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {    
                        case SDLK_a:
                            e.pos_selected = 1;
                            break; 
                        case SDLK_b:
                            e.pos_selected = 2;
                            break; 
                        case SDLK_c:
                            e.pos_selected = 3;
                            break; 
                        case SDLK_RETURN:
                            verify_enigme(&e, screen); 
                            continuer = 0;
                            break;
                        case SDLK_ESCAPE:
                            continuer = 0;
                            break;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    printf("Mouse position: %d %d\n", event.motion.x, event.motion.y);
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (event.motion.x > 5 * scale_x && event.motion.x < 245 * scale_x && 
                            event.motion.y > 368 * scale_y && event.motion.y < 456 * scale_y) {
                            e.pos_selected = 1;
                            verify_enigme(&e, screen); 
                            continuer = 0;
                        } else if (event.motion.x > 411 * scale_x && event.motion.x < 645 * scale_x && 
                                   event.motion.y > 371 * scale_y && event.motion.y < 439 * scale_y) {
                            e.pos_selected = 2;
                            verify_enigme(&e, screen); 
                            continuer = 0;
                        } else if (event.motion.x > 202 * scale_x && event.motion.x < 607 * scale_x && 
                                   event.motion.y > 525 * scale_y && event.motion.y < 600 * scale_y) {
                            e.pos_selected = 3;
                            verify_enigme(&e, screen); 
                            continuer = 0;
                        }
                    }
                    break;
            }
        }
    }
    free_Surface_enigme(e);
}
