#include "fonction.h"
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define global variables (these are declared as extern in the header)
SDL_Surface *background = NULL;
SDL_Surface *minimap = NULL;
SDL_Surface *backgroundMask = NULL;
TTF_Font *gameFont = NULL;

// Load and optimize image for faster blitting
SDL_Surface* load_image(const char *filename) {
    SDL_Surface *loadedImage = IMG_Load(filename);
    if (!loadedImage) {
        printf("Erreur chargement image %s: %s\n", filename, IMG_GetError());
        return NULL;
    }
    SDL_Surface *optimizedImage = SDL_DisplayFormat(loadedImage);
    SDL_FreeSurface(loadedImage);
    return optimizedImage;
}

// Function to initialize the menu
void init_menu(Menu *menu) {
    menu->font = TTF_OpenFont("arial.ttf", 24);  // Change to your font path
    menu->titleFont = TTF_OpenFont("arial.ttf", 48);  // Change to your title font path
    
    if (!menu->font || !menu->titleFont) {
        printf("Error loading menu fonts: %s\n", TTF_GetError());
        // Try a fallback if the specified font isn't found
        menu->font = TTF_OpenFont("FreeSans.ttf", 24);
        menu->titleFont = TTF_OpenFont("FreeSans.ttf", 48);
        
        if (!menu->font || !menu->titleFont) {
            printf("Critical error: Could not load any fonts\n");
            return;
        }
    }
    
    menu->selectedItem = 0;  // Set the first item as selected
    
    // Define colors
    SDL_Color white = {255, 255, 255};
    SDL_Color green = {0, 255, 0};
    
    // Initialize menu items
    menu->menuItems[MENU_NEW_GAME] = TTF_RenderText_Solid(menu->font, "New Game", white);
    menu->menuItems[MENU_LOAD_GAME] = TTF_RenderText_Solid(menu->font, "Load Game", white);
    menu->menuItems[MENU_QUIT] = TTF_RenderText_Solid(menu->font, "Quit", white);

    // Selected menu items (with green color for highlighting)
    menu->menuItemsSelected[MENU_NEW_GAME] = TTF_RenderText_Solid(menu->font, "New Game", green);
    menu->menuItemsSelected[MENU_LOAD_GAME] = TTF_RenderText_Solid(menu->font, "Load Game", green);
    menu->menuItemsSelected[MENU_QUIT] = TTF_RenderText_Solid(menu->font, "Quit", green);
    
    menu->title = TTF_RenderText_Solid(menu->titleFont, "Game Menu", white);
}

// Function to clean up the menu
void cleanup_menu(Menu *menu) {
    for (int i = 0; i < MENU_COUNT; i++) {
        SDL_FreeSurface(menu->menuItems[i]);
        SDL_FreeSurface(menu->menuItemsSelected[i]);
    }
    SDL_FreeSurface(menu->title);
    TTF_CloseFont(menu->font);
    TTF_CloseFont(menu->titleFont);
}

// Function to draw the menu
void draw_menu(SDL_Surface *screen, Menu *menu) {
    // Draw a dark blue background
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 64));
    
    // Draw the title in the center
    SDL_Rect titleRect = {(screen->w - menu->title->w) / 2, 50, menu->title->w, menu->title->h};
    SDL_BlitSurface(menu->title, NULL, screen, &titleRect);
    
    // Draw menu items
    for (int i = 0; i < MENU_COUNT; i++) {
        SDL_Rect rect = {(screen->w - menu->menuItems[i]->w) / 2, 150 + i * 50, menu->menuItems[i]->w, menu->menuItems[i]->h};
        
        if (i == menu->selectedItem) {
            // Draw the selected item with highlight
            SDL_BlitSurface(menu->menuItemsSelected[i], NULL, screen, &rect);
        } else {
            // Draw normal items
            SDL_BlitSurface(menu->menuItems[i], NULL, screen, &rect);
        }
    }
}

// Function to handle menu events
int handle_menu_events(SDL_Event *event, Menu *menu) {
    if (event->type == SDL_MOUSEMOTION) {
        // Check if the mouse is hovering over any menu item
        for (int i = 0; i < MENU_COUNT; i++) {
            int x = event->motion.x;
            int y = event->motion.y;
            
            // Calculate menu item rectangle
            SDL_Surface *menuItem = menu->menuItems[i];
            SDL_Rect itemRect = {
                (800 - menuItem->w) / 2,  // Assuming 800 width
                150 + i * 50,
                menuItem->w,
                menuItem->h
            };
            
            // Check if mouse is inside the rectangle
            if (x >= itemRect.x && x <= itemRect.x + itemRect.w &&
                y >= itemRect.y && y <= itemRect.y + itemRect.h) {
                menu->selectedItem = i;
                return i;  // Return the hovered item index
            }
        }
    } else if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            // Check if a menu item was clicked
            return menu->selectedItem;  // Return the selected item index for action
        }
    }
    return -1;  // No action
}

// Function to create a button
void create_button(Button *button, const char *text, TTF_Font *font, int x, int y) {
    button->rect.x = x - 75;  // Center the button around the provided x coordinate
    button->rect.y = y;
    button->rect.w = 150;     // Fixed width for all buttons
    button->rect.h = 40;      // Fixed height for all buttons
    button->text = strdup(text);
    button->isHovered = 0;
    
    // Render text surfaces
    SDL_Color textColor = {255, 255, 255};  // White for normal state
    SDL_Color hoverColor = {0, 255, 0};     // Green for hover state
    
    button->surface = TTF_RenderText_Solid(font, text, textColor);
    button->surfaceHover = TTF_RenderText_Solid(font, text, hoverColor);
}

// Function to draw a button
void draw_button(SDL_Surface *screen, Button *button) {
    // Draw button background
    SDL_Rect bgRect = button->rect;
    
    // Draw a darker background for the button
    Uint32 bgColor = SDL_MapRGB(screen->format, 50, 50, 80);
    if (button->isHovered) {
        bgColor = SDL_MapRGB(screen->format, 70, 70, 100);  // Lighter when hovered
    }
    
    SDL_FillRect(screen, &bgRect, bgColor);
    
    // Draw border
    SDL_Rect borderRect = bgRect;
    SDL_Rect innerRect = {
        bgRect.x + 2,
        bgRect.y + 2,
        bgRect.w - 4,
        bgRect.h - 4
    };
    
    SDL_FillRect(screen, &borderRect, SDL_MapRGB(screen->format, 100, 100, 150));
    SDL_FillRect(screen, &innerRect, bgColor);
    
    // Draw text centered on button
    SDL_Surface *textSurface = button->isHovered ? button->surfaceHover : button->surface;
    SDL_Rect textRect = {
        button->rect.x + (button->rect.w - textSurface->w) / 2,
        button->rect.y + (button->rect.h - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };
    
    SDL_BlitSurface(textSurface, NULL, screen, &textRect);
}

// Function to check if the mouse is over the button
int is_mouse_over_button(int mouseX, int mouseY, Button *button) {
    return (mouseX >= button->rect.x && mouseX <= button->rect.x + button->rect.w &&
            mouseY >= button->rect.y && mouseY <= button->rect.y + button->rect.h);
}



// Function to check for collision between AABB rectangles
int check_collision_AABB(int x, int y, int size, SDL_Rect obstacle) {
    SDL_Rect playerRect = {x - size / 2, y - size / 2, size, size};
    return collision_AABB(playerRect, obstacle);
}

// Function to update the player animation
void update_animation(Player *p) {
    animer_joueur(p);
}

// Function to display background
void afficherArrierePlan(SDL_Surface *screen) {
    SDL_BlitSurface(background, NULL, screen, NULL);
}

// Function to display obstacle
void afficher_obstacle(SDL_Surface *screen, SDL_Surface *obstacleSurface, SDL_Rect *obstacle) {
    SDL_BlitSurface(obstacleSurface, NULL, screen, obstacle);
}

// Function to display the minimap
void afficher_minimap(SDL_Surface *screen, Player *p) {
    draw_minimap(screen, minimap, p->x, p->y);
}

// Function to display pause menu
void afficher_menu_pause(SDL_Surface *screen, Button *b1, Button *b2, Button *b3) {
    // Draw a semi-transparent overlay
    SDL_Surface *overlay = SDL_CreateRGBSurface(SDL_SWSURFACE, screen->w, screen->h,
                                               32, 0, 0, 0, 0);
    SDL_FillRect(overlay, NULL, SDL_MapRGB(overlay->format, 0, 0, 0));
    SDL_SetAlpha(overlay, SDL_SRCALPHA, 180);
    SDL_BlitSurface(overlay, NULL, screen, NULL);
    SDL_FreeSurface(overlay);
    
    // Draw "PAUSED" text
    SDL_Color textColor = {255, 255, 255};
    SDL_Surface *pausedText = TTF_RenderText_Blended(gameFont, "PAUSED", textColor);
    SDL_Rect pausedPos = {
        (screen->w - pausedText->w) / 2,
        150,
        pausedText->w,
        pausedText->h
    };
    SDL_BlitSurface(pausedText, NULL, screen, &pausedPos);
    SDL_FreeSurface(pausedText);
    
    // Draw buttons
    draw_button(screen, b1);
    draw_button(screen, b2);
    draw_button(screen, b3);
}
// Function to draw the player
void draw_player(SDL_Surface *screen, Player *p) {
    // Create a surface for the player
    SDL_Surface *playerSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 
                                                     p->size, 
                                                     p->size, 
                                                     32, 
                                                     0x00FF0000, 
                                                     0x0000FF00, 
                                                     0x000000FF, 
                                                     0);
    
    if (!playerSurface) {
        printf("Error creating player surface: %s\n", SDL_GetError());
        return;
    }
    
    // Fill with player color
    SDL_FillRect(playerSurface, NULL, SDL_MapRGB(playerSurface->format, 
                                                p->color.r, 
                                                p->color.g, 
                                                p->color.b));
    
    // Position for blitting
    SDL_Rect pos = {p->x - p->size / 2, p->y - p->size / 2, p->size, p->size};
    
    // Blit player to screen
    SDL_BlitSurface(playerSurface, NULL, screen, &pos);
    
    // Free the surface
    SDL_FreeSurface(playerSurface);
}

// Function to draw the pause menu
void draw_pause_menu(SDL_Surface *screen, Button *resumeButton, Button *saveButton, Button *quitButton) {
    // Draw the semi-transparent overlay
    SDL_Surface *overlay = SDL_CreateRGBSurface(SDL_SWSURFACE, 
                                               screen->w, 
                                               screen->h,
                                               32, 
                                               0, 0, 0, 0);
    SDL_FillRect(overlay, NULL, SDL_MapRGB(overlay->format, 0, 0, 0));
    SDL_SetAlpha(overlay, SDL_SRCALPHA, 180);  // Set transparency level
    SDL_BlitSurface(overlay, NULL, screen, NULL);
    SDL_FreeSurface(overlay);
    
    // Draw the "PAUSED" text
    SDL_Color white = {255, 255, 255};
    SDL_Surface *pauseText = TTF_RenderText_Blended(gameFont, "PAUSED", white);
    SDL_Rect pausePos = {(screen->w - pauseText->w) / 2, 150, pauseText->w, pauseText->h};
    SDL_BlitSurface(pauseText, NULL, screen, &pausePos);
    SDL_FreeSurface(pauseText);
    
    // Draw the buttons
    draw_button(screen, resumeButton);
    draw_button(screen, saveButton);
    draw_button(screen, quitButton);
}

// Function to animate player (changing color and size)
void animer_joueur(Player *p) {
    Uint32 currentTime = SDL_GetTicks();
    
    // Only update if enough time has passed
    if (currentTime - p->lastUpdate > p->frameDelay) {
        // Cycle through the color scheme
        p->colorPhase = (p->colorPhase + 1) % 3;
        
        switch (p->colorPhase) {
            case 0:
                p->color.r = 255;
                p->color.g = 0;
                p->color.b = 0;
                break;
            case 1:
                p->color.r = 0;
                p->color.g = 255;
                p->color.b = 0;
                break;
            case 2:
                p->color.r = 0;
                p->color.g = 0;
                p->color.b = 255;
                break;
        }
        
        // Size animation (growing and shrinking)
        if (p->direction) {
            p->size++;
            if (p->size >= 25) {
                p->direction = 0;
            }
        } else {
            p->size--;
            if (p->size <= 15) {
                p->direction = 1;
            }
        }
        
        p->lastUpdate = currentTime;
    }
}

// Function to load background, minimap, and background mask
void chargerArrierePlans() {
    background = load_image("abir.png");
    if (!background) {
        printf("Error loading background.png\n");
    }
    
    minimap = load_image("abirmini.png");
    if (!minimap) {
        printf("Error loading minimap.png\n");
    }
    
    backgroundMask = load_image("nouvelle.jpg");
    if (!backgroundMask) {
        printf("Error loading background_mask.png\n");
    }
}

// Function to free background, minimap, and background mask
void libererArrierePlans() {
    if (background) {
        SDL_FreeSurface(background);
        background = NULL;
    }
    
    if (minimap) {
        SDL_FreeSurface(minimap);
        minimap = NULL;
    }
    
    if (backgroundMask) {
        SDL_FreeSurface(backgroundMask);
        backgroundMask = NULL;
    }
}

// Collision between two rectangles using AABB method
int collision_AABB(SDL_Rect rect1, SDL_Rect rect2) {
    // Check if the two rectangles overlap
    if (rect1.x + rect1.w <= rect2.x) return 0;
    if (rect1.x >= rect2.x + rect2.w) return 0;
    if (rect1.y + rect1.h <= rect2.y) return 0;
    if (rect1.y >= rect2.y + rect2.h) return 0;
    
    return 1;  // Collision detected
}

/// Add this function to fonction.c - it's missing from your first project
Uint32 GetPixel(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1: return *p;
        case 2: return *(Uint16 *)p;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
        case 4: return *(Uint32 *)p;
        default: return 0;
    }
}

// Replace the existing collisionParfaite function with this one from project 2
int collisionParfaite(Player p, SDL_Rect obstacle, SDL_Surface *backgroundMask) {
    if (!backgroundMask) return 0;
    
    Uint32 pixelColor;
    Uint8 r, g, b;
    
    // Create player rectangle (centered on player position)
    int x = p.x - p.size / 2;
    int y = p.y - p.size / 2;
    int w = p.size;
    int h = p.size;
    
    // Check 8 points around the player's bounding box (corners and midpoints of edges)
    int px[8] = { x, x + w/2, x + w, x, x, x + w/2, x + w, x + w };
    int py[8] = { y, y, y, y + h/2, y + h, y + h, y + h, y + h/2 };

    for (int i = 0; i < 8; i++) {
        if (px[i] < 0 || py[i] < 0 || px[i] >= backgroundMask->w || py[i] >= backgroundMask->h) continue;

        pixelColor = GetPixel(backgroundMask, px[i], py[i]);
        SDL_GetRGB(pixelColor, backgroundMask->format, &r, &g, &b);

        // Check for dark pixels in the mask (representing collision areas)
        if (r <= 60 && g <= 60 && b <= 60) {
            return 1;
        }
    }

    return 0;
}

// Update the move_player function to handle collisions
void move_player(Player *p, int up, int down, int left, int right) {
    // Save old position
    int old_x = p->x;
    int old_y = p->y;
    
    // Calculate new position
    int newX = p->x + (right - left) * 5;  // Multiply by a speed factor
    int newY = p->y + (down - up) * 5;  
    
    // Boundary checks to keep player inside screen
    if (newX < 20) newX = 20;
    if (newX > 780) newX = 780;  // Assuming 800 width
    if (newY < 20) newY = 20;
    if (newY > 580) newY = 580;  // Assuming 600 height
    
    // Update position
    p->x = newX;
    p->y = newY;
    
    // Check for collision with background mask
    if (backgroundMask && collisionParfaite(*p, (SDL_Rect){0, 0, 0, 0}, backgroundMask)) {
        // If collision detected, revert to old position
        p->x = old_x;
        p->y = old_y;
    }
}

// Update main game loop's collision handling part
// Add this to the game loop in main.c before drawing
void handle_collisions(Player *p, SDL_Rect obstacle) {
    static int collisionBB_detected = 0;
    static int collisionParfaite_detected = 0;
    
    // Check pixel-perfect collision with background
    if (backgroundMask && collisionParfaite(*p, (SDL_Rect){0, 0, 0, 0}, backgroundMask)) {
        if (!collisionParfaite_detected) {
            printf("🎯 Collision parfaite avec le fond !\n");
            collisionParfaite_detected = 1;
        }
    } else {
        collisionParfaite_detected = 0;
    }
    
    // Check collision with obstacle
    SDL_Rect playerRect = {p->x - p->size / 2, p->y - p->size / 2, p->size, p->size};
    if (collision_AABB(playerRect, obstacle)) {
        if (!collisionBB_detected) {
            printf("💥 Collision BB avec l'obstacle !\n");
            collisionBB_detected = 1;
            p->animating = 1;  // Start animation on collision
        }
    } else {
        if (collisionBB_detected) {
            // Reset animation when collision ends
            p->animating = 0;
        }
        collisionBB_detected = 0;
    }
}
// Function to draw the minimap
void draw_minimap(SDL_Surface *screen, SDL_Surface *minimap, int playerX, int playerY) {
    if (!minimap) return;
    // Minimap position (bottom right corner)
    SDL_Rect minimapPos = {screen->w - minimap->w - 20, screen->h - minimap->h - 20, 
                           minimap->w, minimap->h};
    
    // Blit minimap
    SDL_BlitSurface(minimap, NULL, screen, &minimapPos);
    
    // Draw player position on minimap
    // Calculate minimap coordinates based on player position
    float minimapX = minimapPos.x + (playerX / (float)screen->w) * minimap->w;
    float minimapY = minimapPos.y + (playerY / (float)screen->h) * minimap->h;
    
    // Draw a red dot for player
    SDL_Rect playerDot = {minimapX - 2, minimapY - 2, 4, 4};
    SDL_FillRect(screen, &playerDot, SDL_MapRGB(screen->format, 255, 0, 0));
}

// Function to draw the HUD
void draw_hud(SDL_Surface *screen, Player *p, TTF_Font *font) {
    SDL_Color white = {255, 255, 255};
    
    // Create score text
    char scoreText[50];
    sprintf(scoreText, "Score: %d", p->score);
    SDL_Surface *scoreSurface = TTF_RenderText_Blended(font, scoreText, white);
    SDL_Rect scorePos = {20, 20, scoreSurface->w, scoreSurface->h};
    SDL_BlitSurface(scoreSurface, NULL, screen, &scorePos);
    SDL_FreeSurface(scoreSurface);
    
    // Create collectibles text
    char collectText[50];
    sprintf(collectText, "Items: %d", p->collectibles);
    SDL_Surface *collectSurface = TTF_RenderText_Blended(font, collectText, white);
    SDL_Rect collectPos = {20, 50, collectSurface->w, collectSurface->h};
    SDL_BlitSurface(collectSurface, NULL, screen, &collectPos);
    SDL_FreeSurface(collectSurface);
    
    // Display position for debugging
    char posText[50];
    sprintf(posText, "Position: %d, %d", p->x, p->y);
    SDL_Surface *posSurface = TTF_RenderText_Blended(font, posText, white);
    SDL_Rect posPos = {20, 80, posSurface->w, posSurface->h};
    SDL_BlitSurface(posSurface, NULL, screen, &posPos);
    SDL_FreeSurface(posSurface);
}

// Save game function
void sauvegarder(const char *filename, Player joueur) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Error opening save file for writing\n");
        return;
    }
    
    // Write player properties
    fprintf(f, "x=%d\n", joueur.x);
    fprintf(f, "y=%d\n", joueur.y);
    fprintf(f, "size=%d\n", joueur.size);
    fprintf(f, "score=%d\n", joueur.score);
    fprintf(f, "collectibles=%d\n", joueur.collectibles);
    
    fclose(f);
}

// Load game function
int load_game(Player *joueur) {
    FILE *f = fopen("savegame.txt", "r");
    if (!f) {
        printf("Error opening save file for reading\n");
        return -1;
    }
    
    char line[100];
    char key[20];
    int value;
    
    // Read each line
    while (fgets(line, sizeof(line), f)) {
        // Parse key=value format
        if (sscanf(line, "%[^=]=%d", key, &value) == 2) {
            if (strcmp(key, "x") == 0) {
                joueur->x = value;
            } else if (strcmp(key, "y") == 0) {
                joueur->y = value;
            } else if (strcmp(key, "size") == 0) {
                joueur->size = value;
            } else if (strcmp(key, "score") == 0) {
                joueur->score = value;
            } else if (strcmp(key, "collectibles") == 0) {
                joueur->collectibles = value;
            }
        }
    }
    
    fclose(f);
    return 0;
}

// Function to display a message
void display_message(SDL_Surface *screen, TTF_Font *font, const char *message, int duration) {
    // Create text surface
    SDL_Color white = {255, 255, 255};
    SDL_Surface *messageSurface = TTF_RenderText_Blended(font, message, white);
    
    // Create a semi-transparent background
    SDL_Surface *background = SDL_CreateRGBSurface(SDL_SWSURFACE, 
                                                  messageSurface->w + 20, 
                                                  messageSurface->h + 20,
                                                  32, 
                                                  0, 0, 0, 0);
    SDL_FillRect(background, NULL, SDL_MapRGB(background->format, 0, 0, 0));
    SDL_SetAlpha(background, SDL_SRCALPHA, 180);
    
    // Position in center of screen
    SDL_Rect bgPos = {
        (screen->w - background->w) / 2,
        (screen->h - background->h) / 2,
        background->w,
        background->h
    };
    
    SDL_Rect textPos = {
        (screen->w - messageSurface->w) / 2,
        (screen->h - messageSurface->h) / 2,
        messageSurface->w,
        messageSurface->h
    };
    
    // Save current screen
    SDL_Surface *screenCopy = SDL_CreateRGBSurface(SDL_SWSURFACE, 
                                                  screen->w, 
                                                  screen->h,
                                                  32, 
                                                  0, 0, 0, 0);
    SDL_BlitSurface(screen, NULL, screenCopy, NULL);
    
    // Display message
    SDL_BlitSurface(background, NULL, screen, &bgPos);
    SDL_BlitSurface(messageSurface, NULL, screen, &textPos);
    SDL_Flip(screen);
    
    // Wait for duration
    SDL_Delay(duration);
    
    // Restore screen
    SDL_BlitSurface(screenCopy, NULL, screen, NULL);
    
    // Free surfaces
    SDL_FreeSurface(messageSurface);
    SDL_FreeSurface(background);
    SDL_FreeSurface(screenCopy);
}
      


