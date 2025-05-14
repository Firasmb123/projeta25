#ifndef GUARDIAN_LEGEND_H
#define GUARDIAN_LEGEND_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    PLATFORM_FIXED,
    PLATFORM_MOVING,
    PLATFORM_DESTRUCTIBLE
} PlatformType;

typedef struct {
    SDL_Surface *image;
    SDL_Rect pos;
    int is_moving;
    int speed;
    int direction;
    PlatformType type;
    int is_active;
    int health;
} Platform;

typedef struct {
    SDL_Surface *background;
    SDL_Rect bg_pos;
    int bg_scroll_x;
    int bg_scroll_y;
    Platform *platforms;
    int platform_count;
} Level;

typedef struct {
    SDL_Surface *screen;
    SDL_Rect viewport1;
    SDL_Rect viewport2;
    int is_split_screen;
} GameScreen;

typedef struct {
    Uint32 start_time;
    Uint32 current_time;
    TTF_Font *font;
} GameTimer;

typedef struct {
    char name[50];
    int score;
} HighScore;

typedef struct {
    int x, y;
    int controls[4]; // UP, DOWN, LEFT, RIGHT
    SDL_Rect camera;
} Player;

#define MAX_HIGH_SCORES 10
#define PLATFORM_WIDTH 100
#define PLATFORM_HEIGHT 20
#define SCROLL_SPEED 5
#define PLAYER_SPEED 3
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define HELP_KEY SDLK_F1
#define HIGHSCORE_KEY SDLK_e
#define PLATFORM_DESTRUCTION_TIME 2000

// Game initialization
void init_level(Level *level, const char *bg_path, int platform_count);
void add_platform(Level *level, int index, PlatformType type, int x, int y, int is_moving, int speed);
void init_game_screen(GameScreen *gs);
void init_timer(GameTimer *timer);

// Game logic
void update_platforms(Level *level);
void update_player_camera(Player *player, Level *level, int screen_w, int screen_h);
void handle_platform_collision(Platform *platform, Uint32 current_time);

// Rendering
void render_timer(GameTimer *timer, SDL_Surface *screen, int player_num);
void draw_player(SDL_Surface *screen, int x, int y, int player_num);
void render_player_view(SDL_Surface *screen, Level *level, GameTimer *timer, Player *player, SDL_Rect viewport);
void render_split_screen(GameScreen *gs, Level *level, GameTimer *timer, Player players[2]);

// UI
void show_help_window(SDL_Surface *screen, TTF_Font *font, int *show_help);
void show_high_score_menu(SDL_Surface *screen, TTF_Font *font, int *running, int score);
void show_high_scores(SDL_Surface *screen, TTF_Font *font);
void save_high_score(const char *name, int score);

// Utilities
SDL_Color get_platform_color(PlatformType type);
void cleanup_level(Level *level);
void enable_split_screen(GameScreen *gs);

#endif
