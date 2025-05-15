#include "fonction.h"
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }
    
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return -1;
    }
    
    // Disable key repeat for smoother input
    SDL_EnableKeyRepeat(0, 0);

    // Create main window
    SDL_Surface *screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!screen) {
        printf("Error setting video mode: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return -1;
    }
    
    // Set window caption
    SDL_WM_SetCaption("2D Game with Menu", NULL);
    
    // Load font for in-game text
    gameFont = TTF_OpenFont("arial.ttf", 18);
    if (!gameFont) {
        printf("Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // Load game assets
    chargerArrierePlans();
    if (!background || !minimap || !backgroundMask) {
        printf("Error loading game assets.\n");
        TTF_CloseFont(gameFont);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // Initialize player with default values
    Player joueur = {
        100, 100,        // x, y position
        20,              // size
        1,               // direction (growing)
        {255, 0, 0},     // color (red)
        0,               // colorPhase
        0,               // lastUpdate
        100,             // frameDelay
        0,               // animating
        0,               // score
        0                // collectibles
    };

    // Create obstacle
    SDL_Rect obstacle = {490, 450, 75, 64};
    SDL_Surface *obstacleSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 
                                                        obstacle.w, 
                                                        obstacle.h, 
                                                        32,
                                                        0x00FF0000, 
                                                        0x0000FF00, 
                                                        0x000000FF, 
                                                        0);
    if (!obstacleSurface) {
        printf("Error creating obstacle surface: %s\n", SDL_GetError());
        TTF_CloseFont(gameFont);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }
    
    // Fill obstacle with black color and set color key for transparency
    SDL_FillRect(obstacleSurface, NULL, SDL_MapRGB(obstacleSurface->format, 0, 0, 0));
    SDL_SetColorKey(obstacleSurface, SDL_SRCCOLORKEY, SDL_MapRGB(obstacleSurface->format, 0, 0, 0));

    // Initialize menu
    Menu mainMenu;
    init_menu(&mainMenu);
    
    // Initialize pause menu buttons
    Button resumeButton, saveButton, quitButton;
    create_button(&resumeButton, "Resume", gameFont, 400, 250);
    create_button(&saveButton, "Save Game", gameFont, 400, 325);
    create_button(&quitButton, "Quit to Menu", gameFont, 400, 400);

    // Game loop variables
    int running = 1;
    GameState gameState = STATE_MENU;
    SDL_Event event;
    int move_up = 0, move_down = 0, move_left = 0, move_right = 0;
    int mouseX = 0, mouseY = 0;

    // Main game loop
    while (running) {
        // Clear the screen
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        
        // Event handling
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            if (gameState == STATE_PLAYING) {
                                gameState = STATE_PAUSE;
                            } else if (gameState == STATE_PAUSE) {
                                gameState = STATE_PLAYING;
                            }
                            break;
                        case SDLK_UP: 
                            if (gameState == STATE_MENU) {
                                // Navigate menu up
                                if (mainMenu.selectedItem > 0) {
                                    mainMenu.selectedItem--;
                                }
                            } else {
                                move_up = 1; 
                            }
                            break;
                        case SDLK_DOWN: 
                            if (gameState == STATE_MENU) {
                                // Navigate menu down
                                if (mainMenu.selectedItem < MENU_COUNT - 1) {
                                    mainMenu.selectedItem++;
                                }
                            } else {
                                move_down = 1; 
                            }
                            break;
                        case SDLK_LEFT: move_left = 1; break;
                        case SDLK_RIGHT: move_right = 1; break;
                        case SDLK_RETURN:
                            if (gameState == STATE_MENU) {
                                // Handle menu selection
                                switch (mainMenu.selectedItem) {
                                    case MENU_NEW_GAME:
                                        gameState = STATE_PLAYING;
                                        // Reset player if needed
                                        joueur.x = 100;
                                        joueur.y = 100;
                                        joueur.score = 0;
                                        break;
                                    case MENU_LOAD_GAME:
                                        if (load_game(&joueur) == 0) {
                                            gameState = STATE_PLAYING;
                                            display_message(screen, gameFont, "Game loaded!", 1000);
                                        } else {
                                            display_message(screen, gameFont, "No saved game found!", 1000);
                                        }
                                        break;
                                    case MENU_QUIT:
                                        running = 0;
                                        break;
                                }
                            }
                            break;
                        default: break;
                    }
                    break;

                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_UP: move_up = 0; break;
                        case SDLK_DOWN: move_down = 0; break;
                        case SDLK_LEFT: move_left = 0; break;
                        case SDLK_RIGHT: move_right = 0; break;
                        default: break;
                    }
                    break;

                case SDL_MOUSEMOTION:
                    mouseX = event.motion.x;
                    mouseY = event.motion.y;
                    
                    // Update button hover states
                    if (gameState == STATE_PAUSE) {
                        resumeButton.isHovered = is_mouse_over_button(mouseX, mouseY, &resumeButton);
                        saveButton.isHovered = is_mouse_over_button(mouseX, mouseY, &saveButton);
                        quitButton.isHovered = is_mouse_over_button(mouseX, mouseY, &quitButton);
                    }
                    
                    // Check for menu item hovering
                    if (gameState == STATE_MENU) {
                        // Check if mouse is over any menu item
                        for (int i = 0; i < MENU_COUNT; i++) {
                            SDL_Rect itemRect = {
                                (screen->w - mainMenu.menuItems[i]->w) / 2,
                                150 + i * 50,
                                mainMenu.menuItems[i]->w,
                                mainMenu.menuItems[i]->h
                            };
                            
                            if (mouseX >= itemRect.x && mouseX <= itemRect.x + itemRect.w &&
                                mouseY >= itemRect.y && mouseY <= itemRect.y + itemRect.h) {
                                mainMenu.selectedItem = i;
                            }
                        }
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (gameState == STATE_MENU) {
                            // Check if a menu item was clicked
                            for (int i = 0; i < MENU_COUNT; i++) {
                                SDL_Rect itemRect = {
                                    (screen->w - mainMenu.menuItems[i]->w) / 2,
                                    150 + i * 50,
                                    mainMenu.menuItems[i]->w,
                                    mainMenu.menuItems[i]->h
                                };
                                
                                if (mouseX >= itemRect.x && mouseX <= itemRect.x + itemRect.w &&
                                    mouseY >= itemRect.y && mouseY <= itemRect.y + itemRect.h) {
                                    // Handle menu selection
                                    switch (i) {
                                        case MENU_NEW_GAME:
                                            gameState = STATE_PLAYING;
                                            // Reset player if needed
                                            joueur.x = 100;
                                            joueur.y = 100;
                                            joueur.score = 0;
                                            break;
                                        case MENU_LOAD_GAME:
                                            if (load_game(&joueur) == 0) {
                                                gameState = STATE_PLAYING;
                                                display_message(screen, gameFont, "Game loaded!", 1000);
                                            } else {
                                                display_message(screen, gameFont, "No saved game found!", 1000);
                                            }
                                            break;
                                        case MENU_QUIT:
                                            running = 0;
                                            break;
                                    }
                                }
                            }
                        } else if (gameState == STATE_PAUSE) {
                            if (is_mouse_over_button(mouseX, mouseY, &resumeButton)) {
                                gameState = STATE_PLAYING;
                            } else if (is_mouse_over_button(mouseX, mouseY, &saveButton)) {
                                sauvegarder("savegame.txt", joueur);
                                display_message(screen, gameFont, "Game saved!", 1000);
                            } else if (is_mouse_over_button(mouseX, mouseY, &quitButton)) {
                                gameState = STATE_MENU;
                            }
                        }
                    }
                    break;
            }
        }

        // Game state logic
        if (gameState == STATE_MENU) {
            // Draw menu with selected item highlighted
            SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 64)); // Dark blue background
            
            // Draw the title in the center
            SDL_Rect titleRect = {(screen->w - mainMenu.title->w) / 2, 50, mainMenu.title->w, mainMenu.title->h};
            SDL_BlitSurface(mainMenu.title, NULL, screen, &titleRect);
            
            // Draw menu items
            for (int i = 0; i < MENU_COUNT; i++) {
                SDL_Rect rect = {(screen->w - mainMenu.menuItems[i]->w) / 2, 150 + i * 50, mainMenu.menuItems[i]->w, mainMenu.menuItems[i]->h};
                
                if (i == mainMenu.selectedItem) {
                    // Draw the selected item with highlight
                    SDL_BlitSurface(mainMenu.menuItemsSelected[i], NULL, screen, &rect);
                } else {
                    // Draw normal items
                    SDL_BlitSurface(mainMenu.menuItems[i], NULL, screen, &rect);
                }
            }
        } else if (gameState == STATE_PLAYING) {
            // *** UPDATED: Use the improved move_player function that handles collision ***
            move_player(&joueur, move_up, move_down, move_left, move_right);

            // *** UPDATED: Check for collisions separately ***
            handle_collisions(&joueur, obstacle);

            // Update player animation if needed
            if (joueur.animating) {
                update_animation(&joueur);
            }

            // Draw game
            afficherArrierePlan(screen);
            afficher_obstacle(screen, obstacleSurface, &obstacle);
            draw_player(screen, &joueur);
            afficher_minimap(screen, &joueur);
            draw_hud(screen, &joueur, gameFont);
        } else if (gameState == STATE_PAUSE) {
            // First draw the game scene (as background)
            afficherArrierePlan(screen);
            afficher_obstacle(screen, obstacleSurface, &obstacle);
            draw_player(screen, &joueur);
            afficher_minimap(screen, &joueur);
            
            // Then draw pause menu overlay
            draw_pause_menu(screen, &resumeButton, &saveButton, &quitButton);
        }

        // Flip the screen
        SDL_Flip(screen);
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    SDL_FreeSurface(obstacleSurface);
    libererArrierePlans();
    cleanup_menu(&mainMenu);
    TTF_CloseFont(gameFont);
    TTF_Quit();
    SDL_Quit();

    return 0;
}