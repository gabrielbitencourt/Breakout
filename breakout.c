#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

/* define constants and boolean type */
typedef int bool;
const int false = 0;
const int true = 1;

/* screen dimension */
const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 480;

/* SDL global variables */
SDL_Window* gWindow = NULL;
SDL_Surface* gSurface = NULL;
SDL_Event event;

/* players and npcs */
SDL_Surface* gBallSurface = NULL;
SDL_Surface* gPlayerSurface = NULL;

const int BALL_W = 20;
const int BALL_H = 20;

const int PLAYER_W = 100;
const int PLAYER_H = 20;

const int BRICK_W = 50;
const int BRICK_H = 30;

typedef struct _PLAYER{
  int x;
  int y;
  int stepX;
  int width;
  int height;
  SDL_Surface* image;

} PLAYER;

typedef struct _BALL {
    int x;
    int y;
    int stepX;
    int stepY;
    int width;
    int height;
    SDL_Surface* image;

} BALL;

typedef struct _NPC{
  int x;
  int y;
  int width;
  int height;
  SDL_Surface* image;

} NPC;

BALL ball;
PLAYER player;

/* player and npc functions */
PLAYER createPlayer(int x, int y, int stepX, SDL_Surface *);
void movePlayer(PLAYER *);
BALL createBall(int x, int y, int stepX, int stepY, SDL_Surface *);
void moveBall(BALL *);
NPC createNPCs(int x, int y, SDL_Surface *image);
void reflexion(BALL *b, PLAYER *p);

/* init and deinit functions */
bool init();
void deinit();

/* load media functions */
bool loadMedia();
SDL_Surface* loadSurface(char *path);

int main(int argc, char const *argv[]) {

  bool quit = false;
  SDL_Rect srcRect, dstRect;
  SDL_Rect srcRect2, dstRect2;

  if (!init()) {
    printf("SDL didn't init properly\n");
  }
  else{
    if (!loadMedia()) {
      printf("SDL didn't load media properly\n");
    }
    else{

      /* create ball and player */
      ball = createBall((SCREEN_WIDTH - BALL_W) / 2, (SCREEN_HEIGHT - BALL_H) / 2, rand() % 2 ? -1: 1, rand() % 2 ? -1: 1, gBallSurface);
      player = createPlayer((SCREEN_WIDTH - PLAYER_W) / 2, 400, 0, gPlayerSurface);

      while (!quit) {

        /* handle events */
        while (SDL_PollEvent(&event) != 0) {
          switch (event.type) {
            /* user quit window */
            case SDL_QUIT:
              quit = true;
              break;

            /* user press a key */
            case SDL_KEYDOWN:
              switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                  quit = true;
                  break;
                case SDLK_LEFT:
                  player.stepX = -10;
                  movePlayer(&player);

                  break;
                case SDLK_RIGHT:
                  player.stepX = 10;
                  movePlayer(&player);

                  break;
              }
          }
        }

        /* fill the window with a color */
        SDL_FillRect(gSurface, NULL, SDL_MapRGB(gSurface->format, 0x00, 0x00, 0xFF));

        /* move player */
        player.stepX = 0;
        movePlayer(&player);
        srcRect2.x = 0; srcRect2.y = 0;
        srcRect2.w = PLAYER_W;
        srcRect2.h = PLAYER_H;

        dstRect2.x = player.x;

        if(SDL_BlitSurface(player.image, &srcRect2, gSurface, &dstRect2) < 0 ) {
            printf( "SDL could not blit! SDL Error: %s\n", SDL_GetError() );
            quit = true;
        }

        /* mpve ball */
        moveBall(&ball);
        reflexion(&ball, &player);
        srcRect.x = 0; srcRect.y = 0;
        srcRect.w = BALL_W;
        srcRect.h = BALL_H;

        dstRect.x = ball.x;
        dstRect.y = ball.y;

        if(SDL_BlitSurface(ball.image, &srcRect, gSurface, &dstRect) < 0 ) {
            printf( "SDL could not blit! SDL Error: %s\n", SDL_GetError() );
            quit = true;
        }

        /* Update the surface */
        SDL_UpdateWindowSurface(gWindow);

        /* Not so good solution, depends on your computer */
        SDL_Delay(1000/25);
      }
    }
  }

  return 0;
}

bool init(){

  srand(time(NULL));

  /* init video */
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL couldn't init video, error: %s\n", SDL_GetError());

    return false;
  }
  else{
    /* init audio */
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
      printf("SDL couldn't init audio, error: %s\n", SDL_GetError());

      return false;
    }
    else{
      /* create window */
      gWindow = SDL_CreateWindow("breakout game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
      if (gWindow == NULL) {
        printf("SDL couldn't init window, error: %s\n", SDL_GetError());

        return false;
      }
      else{
        /* init surface */
        gSurface = SDL_GetWindowSurface(gWindow);
        if (gSurface == NULL) {
          printf("SDL couldn't init surface, error: %s\n", SDL_GetError());

          return false;
        }
        else{
          /* init JPG and PNG loading */
          if(!(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) & (IMG_INIT_JPG | IMG_INIT_PNG))) {
              printf("SDL couldn't initialize image loading, error: %s\n", IMG_GetError() );

              return false;
          }
        }
      }
    }
  }

  return true;
}

bool loadMedia(){

  /* init ball and player image */
  gBallSurface = loadSurface("./ball.jpeg");
  gPlayerSurface = loadSurface("./player.jpeg");

  if (gBallSurface == NULL) {
    printf("SDL couldn't load media, error: %s\n", SDL_GetError());

    return false;
  }

  return true;
}

SDL_Surface* loadSurface(char *path) {
    /* The final optimized image */
    SDL_Surface* optimizedSurface = NULL;

    /* Load image at specified path */
    SDL_Surface* loadedSurface = IMG_Load(path);
    if(loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    }
    else {
        /*Convert surface to screen format*/
        optimizedSurface = SDL_ConvertSurface(loadedSurface, gSurface->format, 0);


        /* transparency */
        SDL_SetColorKey(optimizedSurface, SDL_TRUE, SDL_MapRGB(optimizedSurface->format, 0xFF, 0xFF, 0xFF));

        if(optimizedSurface == NULL) {
            printf("Unable to optimize image %s, error: %s\n", path, SDL_GetError());
        }

        /* Get rid of old loaded surface */
        SDL_FreeSurface(loadedSurface);
    }

    return optimizedSurface;
}

PLAYER createPlayer(int posX, int posY, int stepX, SDL_Surface *image){

    PLAYER player;

    player.x = posX;
    player.y = posY;
    player.stepX = stepX;
    player.image = image;

    return player;

}

void movePlayer(PLAYER *player) {
    player->x += player->stepX;

    if ((player->x + PLAYER_W > SCREEN_WIDTH) || (player->x < 0)) {
        player->stepX = -player->stepX;
        player->x += player->stepX;
    }
}

BALL createBall(int posX, int posY, int stepX, int stepY, SDL_Surface *image){

  BALL ball;

  ball.x = posX;
  ball.y = posY;
  ball.stepX = stepX;
  ball.stepY = stepY;
  ball.image = image;

  return ball;
}
void moveBall(BALL *ball) {
    ball->x += ball->stepX;
    ball->y += ball->stepY;

    if ((ball->x + BALL_W > SCREEN_WIDTH) || (ball->x < 0)) {
        ball->stepX = -ball->stepX;
        ball->x += ball->stepX;
    }
    if ((ball->y + BALL_H > SCREEN_HEIGHT) || (ball->y < 0)) {
        ball->stepY = -ball->stepY;
        ball->y += ball->stepY;
    }
}
void reflexion(BALL *b, PLAYER *p){
    if ((b->x == p->x) && (b->y == p->y)) {
      printf("coe rapaizada\n");
    }
}
