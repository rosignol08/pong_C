#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>

bool running = true;
int windowWidth, windowHeight;

// structure joueur
typedef struct {
    int x;
    int y;
    int width;
    int height; 
} Joueur;

//structure balle
typedef struct {
    int x;
    int y;
    int width;
    int height;
    int dx;
    int dy;
} Balle;

void colleur_dimage_joueur(SDL_Renderer* renderer, Joueur* joueur) {
    SDL_Rect rect = {joueur->x, joueur->y, joueur->width, joueur->height};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void colleur_dimage_balle(SDL_Renderer* renderer, Balle* ball) {
    SDL_Rect rect = {ball->x, ball->y, ball->width, ball->height};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void handleInput(Joueur* joueur_1, bool* running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                *running = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        *running = false;
                        break;
                    case SDLK_UP:
                        joueur_1->y -= 15;
                        break;
                    case SDLK_DOWN:
                        joueur_1->y += 15;
                        break;
                }
                break;
        }
    }
}
void replaceBall(Balle* ball);

void initBall(Balle* ball) {
    ball->x = windowWidth / 2 - ball->width / 2;
    ball->y = windowHeight / 2 - ball->height / 2;
    
    //generer une direction aleatoire
    int randomDirection = rand() % 4; //nb aleatoire entre 0 et 3
    switch (randomDirection) {
        case 0:
            ball->dx = 5; // Move right
            ball->dy = 0;
            break;
        case 1:
            ball->dx = -5; // Move left
            ball->dy = 0;
            break;
        case 2:
            ball->dx = 2;
            ball->dy = 5; // Move down
            break;
        case 3:
            ball->dx = 2;
            ball->dy = -5; // Move up
            break;
    }
}
void updateBall(Balle* ball, Joueur* joueur_1, Joueur* joueur_2, int* score1, int* score2) {
    // Update la position de la balle selon sa vitesse
    ball->x += ball->dx;
    ball->y += ball->dy;
    // Check collision avec les joueurs
    if (ball->x < joueur_1->x + joueur_1->width && ball->x + ball->width > joueur_1->x &&
        ball->y < joueur_1->y + joueur_1->height && ball->y + ball->height > joueur_1->y) {
        //inverse la direction de la balle
        ball->dx = -ball->dx;
    }
    // Check collision avec les joueurs
    if (ball->x < joueur_2->x + joueur_2->width && ball->x + ball->width > joueur_2->x &&
        ball->y < joueur_2->y + joueur_2->height && ball->y + ball->height > joueur_2->y) {
        //inverse la direction de la balle
        ball->dx = -ball->dx;
    }
    // Check collision avec les bords
    if (ball->y < 0 || ball->y + ball->height > windowHeight) {
        // Inverse la direction de la balle
        ball->dy = -ball->dy;
    }
    if (ball->x < 0) {
        // Incrémente le score du joueur 2 et réinitialise la position de la balle
        (*score2)++;
        replaceBall(ball);
    }
    if (ball->x + ball->width > windowWidth) {
        // Incrémente le score du joueur 1 et réinitialise la position de la balle
        (*score1)++;
        replaceBall(ball);
    }
}

void render(SDL_Renderer* renderer, Balle* ball, Joueur* joueur_1, Joueur* joueur_2,TTF_Font* font, int score1, int score2, SDL_Texture* texture_balle, SDL_Texture* texture_joueur) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    colleur_dimage_balle(renderer, ball);
    colleur_dimage_joueur(renderer, joueur_1);
    colleur_dimage_joueur(renderer, joueur_2);

     //scores
    SDL_Color color = {255, 255, 255};
    char score1_str[10];
    char score2_str[10];
    sprintf(score1_str, "%d - %d", score1, score2);
    SDL_Surface* surface = TTF_RenderText_Solid(font, score1_str, color);
    SDL_Texture* texture_message = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    SDL_Rect messageRect;
    messageRect.x = windowWidth / 2 - surface->w*1.5;
    messageRect.y = 0.1 * windowHeight;
    messageRect.w = 3*surface->w;
    messageRect.h = 3*surface->h;
    SDL_RenderCopy(renderer, texture_message, NULL, &messageRect);
    SDL_DestroyTexture(texture_message);
    SDL_Rect rect = {windowWidth / 2 - surface->w / 2, 10, surface->w, surface->h};

    SDL_RenderPresent(renderer);
}

void updatePaddle(Joueur* joueur) {
    if (joueur->y < 0) {
        joueur->y = 0;
    }
    if (joueur->y + joueur->height > windowHeight) {
        joueur->y = windowHeight - joueur->height;
    }
}

void replaceBall(Balle* ball) {
    ball->x = windowWidth / 2 - ball->width / 2;
    ball->y = windowHeight / 2 - ball->height / 2;
    ball->dx = 5;
    ball->dy = 5;
}

void update_player1_position(Joueur* joueur) {
    joueur->x = windowWidth * 0.05;
}

void update_player2_position(Joueur* joueur) {
    joueur->x = windowWidth - windowWidth * 0.05;
}

void ennemy_aim(Balle* ball, Joueur* joueur) {
    joueur->y = ball->y;
}
int score1, score2 = 0;

int frameCount = 0;
Uint32 lastTime = 0;


int main(int argc, char* argv[]) {
    SDL_Window* window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("Impossible de cree une fenetre: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        printf("Impossible de creer un renderer: %s\n", SDL_GetError());
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("Impossible d'initialiser SDL_image: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Surface* image_balle = IMG_Load("ball.png");
    if (image_balle == NULL) {
        printf("Impossible de charger l'image: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image_balle);

    SDL_FreeSurface(image_balle);

    SDL_Surface* image_joueur = IMG_Load("joueur.png");
    if (image_joueur == NULL) {
        printf("Impossible de charger l'image: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Texture* texture_joueur = SDL_CreateTextureFromSurface(renderer, image_joueur);

    SDL_FreeSurface(image_joueur);

    SDL_Surface* image_ia = IMG_Load("joueur.png");
    if (image_ia == NULL) {
        printf("Impossible de charger l'image: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Texture* texture_ia = SDL_CreateTextureFromSurface(renderer, image_ia);

    SDL_FreeSurface(image_ia);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    if (texture == NULL) {
        printf("Impossible de creer une texture: %s\n", SDL_GetError());
        return 1;
    }

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    //scores
    if (TTF_Init() == -1) {
        printf("Impossible d'initialiser SDL_ttf: %s\n", TTF_GetError());
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (font == NULL) {
        printf("Impossible de charger la police: %s\n", TTF_GetError());
        return 1;
    }

    
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    //creation du joueur 1
    Joueur joueur_1;
    joueur_1.x = 0;
    joueur_1.y = 0;
    joueur_1.width = 0.01*windowWidth;
    joueur_1.height = 0.2*windowHeight;
    
    //creation du joueur 2
    Joueur joueur_2;
    joueur_2.x = 0;
    joueur_2.y = 0;
    joueur_2.width = 0.01*windowWidth;
    joueur_2.height = 0.2*windowHeight;

    //creation de la balle
    Balle ball;
    ball.x = 0;
    ball.y = 0;
    ball.width = 30;
    ball.height = 30;
    ball.dx = 0;
    ball.dy = 0;
    
    colleur_dimage_balle(renderer, &ball);
    colleur_dimage_joueur(renderer, &joueur_1);
    colleur_dimage_joueur(renderer, &joueur_2);
    initBall(&ball);
    while (running) {
        handleInput(&joueur_1, &running);
        updateBall(&ball, &joueur_1, &joueur_2, &score1, &score2);
        update_player1_position(&joueur_1);
        update_player2_position(&joueur_2);
        ennemy_aim(&ball, &joueur_2);
        updatePaddle(&joueur_1);
        updatePaddle(&joueur_2);
        render(renderer, &ball, &joueur_1, &joueur_2, font, score1, score2, texture, texture_joueur);
        frameCount++;
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastTime >= 1000) {
            float fps = (float)frameCount / ((currentTime - lastTime) / 1000.0f);
            printf("FPS: %.2f\n", fps);
            frameCount = 0;
            lastTime = currentTime;
        }
        SDL_Delay(16); // Ajoutez une petite pause pour contrôler la vitesse du jeu
        
    }


    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(texture_joueur);
    SDL_DestroyTexture(texture_ia);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

