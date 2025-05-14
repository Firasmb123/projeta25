#include "header.h"

SDL_Color get_platform_color(PlatformType type) {
    switch(type) {
        case PLATFORM_FIXED: return (SDL_Color){0, 255, 0, 0};
        case PLATFORM_MOVING: return (SDL_Color){255, 255, 0, 0};
        case PLATFORM_DESTRUCTIBLE: return (SDL_Color){255, 0, 0, 0};
        default: return (SDL_Color){255, 255, 255, 0};
    }
}

void init_level(Level *level, const char *bg_path, int platform_count) {
    SDL_Surface* loaded = IMG_Load(bg_path);
    if (!loaded) {
        fprintf(stderr, "IMG_Load failed: %s\n", IMG_GetError());
        exit(1);
    }
    level->background = SDL_DisplayFormat(loaded);
    SDL_FreeSurface(loaded);
    
    if (!level->background) {
        fprintf(stderr, "SDL_DisplayFormat failed: %s\n", SDL_GetError());
        exit(1);
    }

    level->bg_pos.x = level->bg_pos.y = 0;
    level->platforms = malloc(platform_count * sizeof(Platform));
    if(!level->platforms) {
        fprintf(stderr, "Failed to allocate memory for platforms\n");
        exit(1);
    }
    level->platform_count = platform_count;
}

void add_platform(Level *level, int index, PlatformType type, int x, int y, int is_moving, int speed) {
    if(index < 0 || index >= level->platform_count) {
        fprintf(stderr, "Invalid platform index\n");
        return;
    }
    
    level->platforms[index].image = SDL_CreateRGBSurface(
        SDL_HWSURFACE, 
        PLATFORM_WIDTH, 
        PLATFORM_HEIGHT, 
        32, 
        0, 0, 0, 0
    );
    
    if(!level->platforms[index].image) {
        fprintf(stderr, "Failed to create platform surface\n");
        return;
    }
    
    SDL_Color color = get_platform_color(type);
    SDL_FillRect(
        level->platforms[index].image, 
        NULL, 
        SDL_MapRGB(level->platforms[index].image->format, color.r, color.g, color.b)
    );
    
    level->platforms[index].pos.x = x;
    level->platforms[index].pos.y = y;
    level->platforms[index].is_moving = is_moving;
    level->platforms[index].speed = speed;
    level->platforms[index].direction = 1;
    level->platforms[index].type = type;
    level->platforms[index].is_active = 1;
    level->platforms[index].health = (type == PLATFORM_DESTRUCTIBLE) ? 100 : 0;
}

void update_platforms(Level *level) {
    for (int i = 0; i < level->platform_count; i++) {
        if (!level->platforms[i].is_active) continue;
        
        if (level->platforms[i].is_moving) {
            level->platforms[i].pos.x += level->platforms[i].speed * level->platforms[i].direction;
            if (level->platforms[i].pos.x > SCREEN_WIDTH - PLATFORM_WIDTH || level->platforms[i].pos.x < 0) {
                level->platforms[i].direction *= -1;
            }
        }
    }
}

void update_player_camera(Player *player, Level *level, int screen_w, int screen_h) {
    // Center camera on player with some smoothing
    int target_x = player->x - screen_w/2;
    int target_y = player->y - screen_h/2;
    
    // Apply some smoothing to camera movement (optional)
    player->camera.x += (target_x - player->camera.x) * 0.1;
    player->camera.y += (target_y - player->camera.y) * 0.1;
    
    // Clamp camera to level boundaries
    if (level->background) {
        int max_x = level->background->w - screen_w;
        int max_y = level->background->h - screen_h;
        
        player->camera.x = (player->camera.x < 0) ? 0 : player->camera.x;
        player->camera.y = (player->camera.y < 0) ? 0 : player->camera.y;
        player->camera.x = (player->camera.x > max_x) ? max_x : player->camera.x;
        player->camera.y = (player->camera.y > max_y) ? max_y : player->camera.y;
    }
}

void handle_platform_collision(Platform *platform, Uint32 current_time) {
    if(platform->type == PLATFORM_DESTRUCTIBLE && platform->is_active) {
        platform->health -= 10;
        
        // Visual feedback - platform gets darker as health decreases
        Uint8 intensity = (Uint8)(platform->health * 2.55);
        SDL_FillRect(platform->image, NULL, SDL_MapRGB(platform->image->format, 
                    255, intensity, intensity));
        
        if(platform->health <= 0) {
            platform->is_active = 0;
        }
    }
    (void)current_time;
}

void init_game_screen(GameScreen *gs) {
    gs->screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!gs->screen) {
        fprintf(stderr, "SDL_SetVideoMode failed: %s\n", SDL_GetError());
        exit(1);
    }
    gs->viewport1.x = gs->viewport1.y = 0;
    gs->viewport1.w = SCREEN_WIDTH;
    gs->viewport1.h = SCREEN_HEIGHT;
    gs->is_split_screen = 0;
}

void enable_split_screen(GameScreen *gs) {
    gs->is_split_screen = !gs->is_split_screen;
    gs->viewport1.h = gs->screen->h / (gs->is_split_screen ? 2 : 1);
    gs->viewport2.x = 0;
    gs->viewport2.y = gs->screen->h / 2;
    gs->viewport2.w = gs->screen->w;
    gs->viewport2.h = gs->screen->h / 2;
}

void init_timer(GameTimer *timer) {
    timer->start_time = SDL_GetTicks();
    timer->current_time = 0;
    
    const char* font_paths[] = {
        "arial.ttf",
        "LiberationSans-Regular.ttf",
        "DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        NULL
    };
    
    for (int i = 0; font_paths[i]; i++) {
        timer->font = TTF_OpenFont(font_paths[i], 24);
        if (timer->font) return;
    }
    
    fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
    timer->font = NULL;
}

void render_timer(GameTimer *timer, SDL_Surface *screen, int player_num) {
    if (!timer->font) return;
    
    char time_str[20];
    sprintf(time_str, "P%d: %02d:%02d", 
           player_num + 1,
           (timer->current_time / 1000) / 60,
           (timer->current_time / 1000) % 60);
    
    SDL_Color white = {255, 255, 255, 0};
    SDL_Surface* text = TTF_RenderText_Solid(timer->font, time_str, white);
    if (text) {
        SDL_Rect pos = {10, 10 + (player_num * 30), 0, 0};
        SDL_BlitSurface(text, NULL, screen, &pos);
        SDL_FreeSurface(text);
    }
}

void draw_player(SDL_Surface *screen, int x, int y, int player_num) {
    SDL_Rect rect = {x - 10, y - 10, 20, 20};
    SDL_Color color = player_num == 0 ? (SDL_Color){255,0,0,0} : (SDL_Color){0,0,255,0};
    SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, color.r, color.g, color.b));
    
    // Player number indicator
    TTF_Font *font = TTF_OpenFont("arial.ttf", 12);
    if(font) {
        char num_text[2] = { '1'+player_num, '\0' };
        SDL_Surface* text = TTF_RenderText_Solid(font, num_text, (SDL_Color){255,255,255,0});
        if(text) {
            SDL_Rect text_pos = {x-3, y-15, 0, 0};
            SDL_BlitSurface(text, NULL, screen, &text_pos);
            SDL_FreeSurface(text);
        }
        TTF_CloseFont(font);
    }
}

void render_player_view(SDL_Surface *screen, Level *level, GameTimer *timer, Player *player, SDL_Rect viewport) {
    SDL_SetClipRect(screen, &viewport);
    
    // Draw background with camera offset
    SDL_Rect bg_dst = {
        viewport.x - player->camera.x,
        viewport.y - player->camera.y,
        level->background->w,
        level->background->h
    };
    SDL_BlitSurface(level->background, NULL, screen, &bg_dst);
    
    // Draw platforms
    for(int i = 0; i < level->platform_count; i++) {
        if(level->platforms[i].is_active) {
            SDL_Rect platform_pos = {
                viewport.x + level->platforms[i].pos.x - player->camera.x,
                viewport.y + level->platforms[i].pos.y - player->camera.y,
                PLATFORM_WIDTH,
                PLATFORM_HEIGHT
            };
            SDL_BlitSurface(level->platforms[i].image, NULL, screen, &platform_pos);
        }
    }
    
    // Draw player relative to viewport
    draw_player(screen, 
               viewport.x + player->x - player->camera.x,
               viewport.y + player->y - player->camera.y,
               0); // Always player 0 in single view
    
    // Draw timer for this view
    render_timer(timer, screen, 0);
    
    SDL_SetClipRect(screen, NULL);
}


void render_split_screen(GameScreen *gs, Level *level, GameTimer *timer, Player players[2]) {
    render_player_view(gs->screen, level, timer, &players[0], gs->viewport1);
    if(gs->is_split_screen) {
        render_player_view(gs->screen, level, timer, &players[1], gs->viewport2);
    }
}

void show_help_window(SDL_Surface *screen, TTF_Font *font, int *show_help) {
    if (!font) return;
    
    *show_help = !(*show_help);
    if (!(*show_help)) return;
    
    SDL_Surface *help_bg = SDL_CreateRGBSurface(SDL_HWSURFACE, 400, 250, 32, 0, 0, 0, 0);
    SDL_FillRect(help_bg, NULL, SDL_MapRGB(help_bg->format, 0, 0, 128));
    
    SDL_Rect pos = {(SCREEN_WIDTH - 400) / 2, (SCREEN_HEIGHT - 250) / 2, 400, 250};
    SDL_BlitSurface(help_bg, NULL, screen, &pos);
    
    SDL_Color white = {255, 255, 255, 0};
    const char *lines[] = {
        "Controls:", 
        "Player 1: Arrow Keys", 
        "Player 2: WASD (split-screen)", 
        "S: Toggle Split-Screen", 
        "F1: Toggle Help", 
        "e: High Scores Menu",
        "ESC: Quit",
        "Click platforms to interact"
    };
    
    for (int i = 0; i < 8; i++) {
        SDL_Surface *text = TTF_RenderText_Solid(font, lines[i], white);
        if (text) {
            SDL_Rect text_pos = {pos.x + 20, pos.y + 20 + i * 25, 0, 0};
            SDL_BlitSurface(text, NULL, screen, &text_pos);
            SDL_FreeSurface(text);
        }
    }
    
    SDL_FreeSurface(help_bg);
}

void save_high_score(const char *name, int score) {
    HighScore scores[MAX_HIGH_SCORES];
    int count = 0;
    
    FILE *file = fopen("highscores.dat", "rb");
    if (file) {
        count = fread(scores, sizeof(HighScore), MAX_HIGH_SCORES, file);
        fclose(file);
    }
    
    if (count < MAX_HIGH_SCORES) {
        strncpy(scores[count].name, name, 49);
        scores[count].name[49] = '\0';
        scores[count].score = score;
        count++;
    } else {
        int min_index = 0;
        for (int i = 1; i < count; i++) {
            if (scores[i].score < scores[min_index].score) min_index = i;
        }
        if (score > scores[min_index].score) {
            strncpy(scores[min_index].name, name, 49);
            scores[min_index].name[49] = '\0';
            scores[min_index].score = score;
        }
    }
    
    // Sort scores
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (scores[i].score < scores[j].score) {
                HighScore temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }
    
    file = fopen("highscores.dat", "wb");
    if (file) {
        fwrite(scores, sizeof(HighScore), count, file);
        fclose(file);
    } else {
        fprintf(stderr, "Failed to save high scores\n");
    }
}

void show_high_scores(SDL_Surface *screen, TTF_Font *font) {
    if (!font) return;
    
    HighScore scores[MAX_HIGH_SCORES];
    int count = 0;
    
    FILE *file = fopen("highscores.dat", "rb");
    if (file) {
        count = fread(scores, sizeof(HighScore), MAX_HIGH_SCORES, file);
        fclose(file);
    }
    
    SDL_Surface *bg = SDL_CreateRGBSurface(SDL_HWSURFACE, 400, 300, 32, 0, 0, 0, 0);
    SDL_FillRect(bg, NULL, SDL_MapRGB(bg->format, 0, 0, 128));
    
    SDL_Rect pos = {(SCREEN_WIDTH - 400) / 2, (SCREEN_HEIGHT - 300) / 2, 400, 300};
    SDL_BlitSurface(bg, NULL, screen, &pos);
    
    SDL_Color white = {255, 255, 255, 0};
    SDL_Surface *title = TTF_RenderText_Solid(font, "High Scores", white);
    if (title) {
        SDL_Rect title_pos = {pos.x + (400 - title->w) / 2, pos.y + 20, 0, 0};
        SDL_BlitSurface(title, NULL, screen, &title_pos);
        SDL_FreeSurface(title);
    }
    
    for (int i = 0; i < count && i < 10; i++) {
        char score_text[100];
        sprintf(score_text, "%d. %s - %d", i + 1, scores[i].name, scores[i].score);
        SDL_Surface *text = TTF_RenderText_Solid(font, score_text, white);
        if (text) {
            SDL_Rect text_pos = {pos.x + 20, pos.y + 70 + i * 25, 0, 0};
            SDL_BlitSurface(text, NULL, screen, &text_pos);
            SDL_FreeSurface(text);
        }
    }
    
    SDL_FreeSurface(bg);
}

void show_high_score_menu(SDL_Surface *screen, TTF_Font *font, int *running, int score) {
    if (!font) return;
    
    char player_name[50] = "Player";
    int name_cursor = 6;
    int menu_running = 1;
    SDL_Event event;
    SDL_Surface *bg = SDL_CreateRGBSurface(SDL_HWSURFACE, 500, 400, 32, 0, 0, 0, 0);
    SDL_FillRect(bg, NULL, SDL_MapRGB(bg->format, 50, 50, 150));
    
    SDL_Rect bg_pos = {(SCREEN_WIDTH - 500) / 2, (SCREEN_HEIGHT - 400) / 2, 500, 400};
    
    // Create buttons
    SDL_Rect validate_btn = {bg_pos.x + 50, bg_pos.y + 300, 120, 50};
    SDL_Rect back_btn = {bg_pos.x + 190, bg_pos.y + 300, 120, 50};
    SDL_Rect quit_btn = {bg_pos.x + 330, bg_pos.y + 300, 120, 50};
    SDL_Rect name_box = {bg_pos.x + 150, bg_pos.y + 200, 200, 30};
    
    SDL_Color white = {255, 255, 255, 0};
    SDL_Color hover_color = {200, 200, 0, 0};
    
    while (menu_running) {
        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);
        
        // Check button hover
        int validate_hover = (mouse_x >= validate_btn.x && mouse_x <= validate_btn.x + validate_btn.w &&
                            mouse_y >= validate_btn.y && mouse_y <= validate_btn.y + validate_btn.h);
        int back_hover = (mouse_x >= back_btn.x && mouse_x <= back_btn.x + back_btn.w &&
                         mouse_y >= back_btn.y && mouse_y <= back_btn.y + back_btn.h);
        int quit_hover = (mouse_x >= quit_btn.x && mouse_x <= quit_btn.x + quit_btn.w &&
                         mouse_y >= quit_btn.y && mouse_y <= quit_btn.y + quit_btn.h);
        
        // Draw background
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        SDL_BlitSurface(bg, NULL, screen, &bg_pos);
        
        // Draw title
        SDL_Surface *title = TTF_RenderText_Solid(font, "High Scores Menu", white);
        if (title) {
            SDL_Rect title_pos = {bg_pos.x + (500 - title->w) / 2, bg_pos.y + 20, 0, 0};
            SDL_BlitSurface(title, NULL, screen, &title_pos);
            SDL_FreeSurface(title);
        }
        
        // Show score
        char score_text[50];
        sprintf(score_text, "Your Score: %d", score);
        SDL_Surface *score_surface = TTF_RenderText_Solid(font, score_text, white);
        if (score_surface) {
            SDL_Rect score_pos = {bg_pos.x + (500 - score_surface->w) / 2, bg_pos.y + 70, 0, 0};
            SDL_BlitSurface(score_surface, NULL, screen, &score_pos);
            SDL_FreeSurface(score_surface);
        }
        
        // Draw name entry
        SDL_Surface *name_label = TTF_RenderText_Solid(font, "Enter Name:", white);
        if (name_label) {
            SDL_Rect label_pos = {bg_pos.x + 50, bg_pos.y + 205, 0, 0};
            SDL_BlitSurface(name_label, NULL, screen, &label_pos);
            SDL_FreeSurface(name_label);
        }
        
        SDL_FillRect(screen, &name_box, SDL_MapRGB(screen->format, 255, 255, 255));
        SDL_Surface *name_text = TTF_RenderText_Solid(font, player_name, (SDL_Color){0, 0, 0, 0});
        if (name_text) {
            SDL_Rect text_pos = {name_box.x + 5, name_box.y + 5, 0, 0};
            SDL_BlitSurface(name_text, NULL, screen, &text_pos);
            SDL_FreeSurface(name_text);
        }
        
        // Draw buttons with hover effect
        SDL_FillRect(screen, &validate_btn, 
                     SDL_MapRGB(screen->format, 
                                validate_hover ? hover_color.r : white.r,
                                validate_hover ? hover_color.g : white.g,
                                validate_hover ? hover_color.b : white.b));
        SDL_FillRect(screen, &back_btn, 
                     SDL_MapRGB(screen->format, 
                                back_hover ? hover_color.r : white.r,
                                back_hover ? hover_color.g : white.g,
                                back_hover ? hover_color.b : white.b));
        SDL_FillRect(screen, &quit_btn, 
                     SDL_MapRGB(screen->format, 
                                quit_hover ? hover_color.r : white.r,
                                quit_hover ? hover_color.g : white.g,
                                quit_hover ? hover_color.b : white.b));
        
        // Draw button text
        SDL_Surface *validate_text = TTF_RenderText_Solid(font, "Valider", (SDL_Color){0, 0, 0, 0});
        SDL_Surface *back_text = TTF_RenderText_Solid(font, "Retour", (SDL_Color){0, 0, 0, 0});
        SDL_Surface *quit_text = TTF_RenderText_Solid(font, "Quitter", (SDL_Color){0, 0, 0, 0});
        
        if (validate_text) {
            SDL_Rect text_pos = {validate_btn.x + (validate_btn.w - validate_text->w)/2, 
                                validate_btn.y + (validate_btn.h - validate_text->h)/2, 0, 0};
            SDL_BlitSurface(validate_text, NULL, screen, &text_pos);
            SDL_FreeSurface(validate_text);
        }
        if (back_text) {
            SDL_Rect text_pos = {back_btn.x + (back_btn.w - back_text->w)/2, 
                                back_btn.y + (back_btn.h - back_text->h)/2, 0, 0};
            SDL_BlitSurface(back_text, NULL, screen, &text_pos);
            SDL_FreeSurface(back_text);
        }
        if (quit_text) {
            SDL_Rect text_pos = {quit_btn.x + (quit_btn.w - quit_text->w)/2, 
                                quit_btn.y + (quit_btn.h - quit_text->h)/2, 0, 0};
            SDL_BlitSurface(quit_text, NULL, screen, &text_pos);
            SDL_FreeSurface(quit_text);
        }
        
        SDL_Flip(screen);
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                menu_running = 0;
                *running = 0;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    menu_running = 0;
                }
                else if (isalnum(event.key.keysym.unicode)) {
                    if (name_cursor < 49) {
                        player_name[name_cursor++] = (char)event.key.keysym.unicode;
                        player_name[name_cursor] = '\0';
                    }
                }
                else if (event.key.keysym.sym == SDLK_BACKSPACE && name_cursor > 0) {
                    player_name[--name_cursor] = '\0';
                }
                else if (event.key.keysym.sym == SDLK_RETURN) {
                    menu_running = 0;
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (validate_hover || back_hover) {
                    menu_running = 0;
                }
                if (quit_hover) {
                    menu_running = 0;
                    *running = 0;
                }
            }
        }
    }
    
    save_high_score(player_name, score);
    SDL_FreeSurface(bg);
}

void cleanup_level(Level *level) {
    for (int i = 0; i < level->platform_count; i++) {
        if (level->platforms[i].image) {
            SDL_FreeSurface(level->platforms[i].image);
        }
    }
    free(level->platforms);
    if (level->background) SDL_FreeSurface(level->background);
}
