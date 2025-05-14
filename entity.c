#include "entity.h"

void initentity(entity *e) {
    e->entity = IMG_Load("bonus.png");
    e->pos_entity.x = 400;
    e->pos_entity.y = 100;

    e->single_EN.w = 100;
    e->single_EN.h = 96;
    e->single_EN.x = 0;
    e->single_EN.y = 0;
    e->EN_num = 0;

    e->direction = 0; 
    e->col = 0;       
    e->show = 1; 
    e->pos_entity.w = e->single_EN.w;
    e->pos_entity.h = e->single_EN.h;     
}

void afficherentity(entity e, SDL_Surface *screen) {
    SDL_BlitSurface(e.entity, &e.single_EN, screen, &e.pos_entity); 
}

void deplacer(entity *e) {
    srand(time(0)); 

    int maxi_down = 300 + rand() % 300; 
    int maxi_up = 50 + rand() % 300;    

    if (e->pos_entity.y >= maxi_down) {
        e->direction = 1;
    }
    if (e->pos_entity.y <= maxi_up) {
        e->direction = 0;
    }

    if (e->direction == 1) {
        e->pos_entity.y -= 5; 
    }
    if (e->direction == 0) {
        e->pos_entity.y += 7;
    }
}

void animerentity(entity *e) {
    if (e->EN_num >= 0 && e->EN_num < 7) {
        e->single_EN.x = e->EN_num * e->single_EN.w;
        e->EN_num++;
    }
    if (e->EN_num == 7) {
        e->EN_num = 0;
        e->single_EN.x = e->EN_num * e->single_EN.w;
    }
}

void update_entity(entity *e) {
    deplacer(e);
    animerentity(e);
}

void initialiser_perso(personnage *p) {
    p->perso = IMG_Load("per.png");
    p->perso_pos.x = 100;
    p->perso_pos.y = 100;
    p->perso_pos.h = 100;
    p->perso_pos.w = 56;
    TTF_Init();
    p->score.font = TTF_OpenFont("angelina.TTF", 50);

    // Updated SDL_Color initialization
    p->score.color = (SDL_Color){255, 0, 255, 0}; // Initialize 'unused' to 0
    p->score.text = TTF_RenderText_Blended(p->score.font, "score 0", p->score.color);

    p->score.postext.x = 100;
    p->score.postext.y = 0;

    p->Vscore = 0;
}

void afficher_perso(personnage p, SDL_Surface *screen) {
    SDL_BlitSurface(p.perso, NULL, screen, &p.perso_pos);
    char vsc[12];
    sprintf(vsc, "score: %d", p.Vscore);

    p.score.text = TTF_RenderText_Blended(p.score.font, vsc, p.score.color);
    SDL_BlitSurface(p.score.text, NULL, screen, &p.score.postext);
}

int collisionTri(entity e, SDL_Rect pose) {
    int X1 = e.pos_entity.x + e.pos_entity.w / 2;
    int Y1 = e.pos_entity.y + e.pos_entity.h / 2;
    int r1;
    if (e.pos_entity.w < e.pos_entity.h) {
        r1 = e.pos_entity.w / 2;
    } else {
        r1 = e.pos_entity.h / 2;
    }
    int X2 = pose.x + pose.w / 2;
    int Y2 = pose.y + pose.h / 2;
    int r2;
    if (pose.w < pose.h) {
        r2 = pose.w / 2;
    } else {
        r2 = pose.h / 2;
    }

    int x11 = X1 - X2;
    int y11 = Y1 - Y2;
    double d1 = sqrt(x11 * x11 + y11 * y11);
    double d2 = r1 + r2;

    if (d1 <= d2)
        return 1; 
    else
        return 0; 
}

int gestion(entity *e) {
    e->show = 0;
    return 0;
}
