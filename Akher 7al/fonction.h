#ifndef FONCTION_H
#define FONCTION_H
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

// External global variables
extern SDL_Surface *background;
extern SDL_Surface *minimap;
extern SDL_Surface *backgroundMask;
extern TTF_Font *gameFont;

// Game state enumeration
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSE
} GameState;

// Menu items enumeration
enum {
    MENU_NEW_GAME,
    MENU_LOAD_GAME,
    MENU_QUIT,
    MENU_COUNT
};

// Player structure
typedef struct {
    int x, y;           // Position
    int size;           // Size of the player
    int direction;      // 1 for growing, 0 for shrinking
    SDL_Color color;    // Color of the player
    int colorPhase;     // Current color phase
    Uint32 lastUpdate;  // Time of last update
    int frameDelay;     // Delay between frames
    int animating;      // Animation flag
    int score;          // Player score
    int collectibles;   // Number of collectibles
} Player;

// Menu structure
typedef struct {
    SDL_Surface *menuItems[MENU_COUNT];
    SDL_Surface *menuItemsSelected[MENU_COUNT];
    SDL_Surface *title;
    TTF_Font *font;
    TTF_Font *titleFont;
    int selectedItem;
} Menu;

// Button structure
typedef struct {
    SDL_Rect rect;
    char *text;
    SDL_Surface *surface;
    SDL_Surface *surfaceHover;
    int isHovered;
} Button;

// Function prototypes
SDL_Surface* load_image(const char *filename);
void init_menu(Menu *menu);
void cleanup_menu(Menu *menu);
void draw_menu(SDL_Surface *screen, Menu *menu);
int handle_menu_events(SDL_Event *event, Menu *menu);
void create_button(Button *button, const char *text, TTF_Font *font, int x, int y);
void draw_button(SDL_Surface *screen, Button *button);
int is_mouse_over_button(int mouseX, int mouseY, Button *button);
void move_player(Player *p, int up, int down, int left, int right);
int check_collision_AABB(int x, int y, int size, SDL_Rect obstacle);
void update_animation(Player *p);
void afficherArrierePlan(SDL_Surface *screen);
void afficher_obstacle(SDL_Surface *screen, SDL_Surface *obstacleSurface, SDL_Rect *obstacle);
void afficher_minimap(SDL_Surface *screen, Player *p);
void afficher_menu_pause(SDL_Surface *screen, Button *b1, Button *b2, Button *b3);
void draw_player(SDL_Surface *screen, Player *p);
void draw_pause_menu(SDL_Surface *screen, Button *resumeButton, Button *saveButton, Button *quitButton);
void animer_joueur(Player *p);
void chargerArrierePlans();
void libererArrierePlans();
int collision_AABB(SDL_Rect rect1, SDL_Rect rect2);
int collisionParfaite(Player p, SDL_Rect obstacle, SDL_Surface *backgroundMask);
void draw_minimap(SDL_Surface *screen, SDL_Surface *minimap, int playerX, int playerY);
void draw_hud(SDL_Surface *screen, Player *p, TTF_Font *font);
void sauvegarder(const char *filename, Player joueur);
int load_game(Player *joueur);
void display_message(SDL_Surface *screen, TTF_Font *font, const char *message, int duration);
void handle_collisions(Player *p, SDL_Rect obstacle);
Uint32 GetPixel(SDL_Surface *surface, int x, int y);

#endif /* FONCTION_H */