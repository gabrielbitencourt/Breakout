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
SDL_Surface* gBrickSurface = NULL;

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

typedef struct _BRICK{
  int x;
  int y;
  int width;
  int height;
  int id;
  SDL_Surface* image;

} BRICK;

BALL ball;
PLAYER player;
BRICK brick;

/* player and npc functions */
PLAYER createPlayer(int x, int y, int stepX, int width, int height, SDL_Surface *);
void movePlayer(PLAYER *);
BALL createBall(int x, int y, int stepX, int stepY, int width, int height, SDL_Surface *);
void moveBall(BALL *);
BRICK createBrick(int x, int y, int width, int height, SDL_Surface *image);
void reflexion(BALL *ball, PLAYER *player);
void collision(BALL *ball, BRICK *brick);

/* init and deinit functions */
bool init();
void deinit();

/* load media functions */
bool loadMedia();
SDL_Surface* loadSurface(char *path);

int main(int argc, char const *argv[]) {

  bool quit = false;
  int l, c;
  SDL_Rect ballSrcRect, ballDstRect;
  SDL_Rect playerSrcRect, playerDstRect;
  SDL_Rect brickSrcRect, brickDstRect;

  if (!init()) {
    printf("SDL didn't init properly\n");
  }
  else{
    if (!loadMedia()) {
      printf("SDL didn't load media properly\n");
    }
    else{

      /* create ball and player */
      player = createPlayer((SCREEN_WIDTH - PLAYER_W) / 2, 420, 0, PLAYER_W, PLAYER_H, gPlayerSurface);
      ball = createBall((SCREEN_WIDTH - BALL_W) / 2, 420 - PLAYER_H, rand() % 2 ? -1: 1, -1, BALL_W, BALL_H, gBallSurface);

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

        playerSrcRect.x = 0; playerSrcRect.y = 0;
        playerSrcRect.w = player.width;
        playerSrcRect.h = player.height;

        playerDstRect.x = player.x;
        playerDstRect.y = player.y;

        if(SDL_BlitSurface(player.image, &playerSrcRect, gSurface, &playerDstRect) < 0 ) {
            printf("SDL could not blit! SDL Error: %s\n", SDL_GetError());
            quit = true;
        }

        /* mpve ball */
        moveBall(&ball);
        reflexion(&ball, &player);

        ballSrcRect.x = 0; ballSrcRect.y = 0;
        ballSrcRect.w = ball.width;
        ballSrcRect.h = ball.height;

        ballDstRect.x = ball.x;
        ballDstRect.y = ball.y;

        if(SDL_BlitSurface(ball.image, &ballSrcRect, gSurface, &ballDstRect) < 0 ) {
            printf( "SDL could not blit! SDL Error: %s\n", SDL_GetError() );
            quit = true;
        }

        /* loops to place all the bricks */
        for (c = 0; c < 13; c++) {
          for (l = 0; l < 4; l++) {
            brick = createBrick((c * (BRICK_W + 5) + 5), (l * (BRICK_H + 5) + 5), BRICK_W, BRICK_H, gBrickSurface);

            brickSrcRect.x = 0; brickSrcRect.y = 0;
            brickSrcRect.w = brick.width;
            brickSrcRect.h = brick.height;

            brickDstRect.x = brick.x;
            brickDstRect.y = brick.y;

            if (SDL_BlitSurface(brick.image, &brickSrcRect, gSurface, &brickDstRect)){
              printf("SDL could not blit! SDL Error: %s\n", SDL_GetError());
              quit = true;
            }

            collision(&ball, &brick);
          }
        }

        /* Update the surface */
        SDL_UpdateWindowSurface(gWindow);

        /* Not so good solution, depends on your computer */
        SDL_Delay(10);
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
  gBallSurface = loadSurface("./ball.png");
  gPlayerSurface = loadSurface("./player.jpeg");
  gBrickSurface = loadSurface("./brick.jpeg");

  /* transparency */
  SDL_SetColorKey(gBallSurface, SDL_TRUE, SDL_MapRGB(gBallSurface->format, 0, 0xFF, 0xFF));

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


        if(optimizedSurface == NULL) {
            printf("Unable to optimize image %s, error: %s\n", path, SDL_GetError());
        }

        /* Get rid of old loaded surface */
        SDL_FreeSurface(loadedSurface);
    }

    return optimizedSurface;
}

PLAYER createPlayer(int posX, int posY, int stepX, int width, int height, SDL_Surface *image){

    PLAYER player;

    player.x = posX;
    player.y = posY;
    player.stepX = stepX;
    player.width = width;
    player.height = height;
    player.image = image;

    return player;

}

void movePlayer(PLAYER *player) {
    player->x += player->stepX;

    if ((player->x + player->width > SCREEN_WIDTH) || (player->x < 0)) {
        player->stepX = -player->stepX;
        player->x += player->stepX;
    }
}

BALL createBall(int posX, int posY, int stepX, int stepY, int width, int height, SDL_Surface *image){

  BALL ball;

  ball.x = posX;
  ball.y = posY;
  ball.stepX = stepX;
  ball.stepY = stepY;
  ball.width = width;
  ball.height = height;
  ball.image = image;

  return ball;
}

void moveBall(BALL *ball) {
    ball->x += ball->stepX;
    ball->y += ball->stepY;

    if ((ball->x + ball->width > SCREEN_WIDTH) || (ball->x < 0)) {
        ball->stepX = -ball->stepX;
        ball->x += ball->stepX;
    }
    if ((ball->y + ball->height > SCREEN_HEIGHT) || (ball->y < 0)) {
        ball->stepY = -ball->stepY;
        ball->y += ball->stepY;
    }
}

BRICK createBrick(int posX, int posY, int width, int height, SDL_Surface *image){

    BRICK brick;

    brick.x = posX;
    brick.y = posY;
    brick.width = width;
    brick.height = height;
    brick.image = image;

    return brick;

}

void reflexion(BALL *ball, PLAYER *player){
  if (ball->y + ball->height == player->y) {
    if ((ball->x >= player->x) && (ball->x <= player->x + player->width)) {
      /* reflect ball when the platform is hit */
      /* bugged when hit in the corner */
      ball->stepY = -ball->stepY;
      ball->y += ball->stepY;

    }
  }
}

void collision(BALL *ball, BRICK *brick){
    if (ball->y == brick->y + brick->height) {
      if ((ball->x >= brick->x) && (ball->x <= brick->x + brick->width)) {
        /* reflect ball when the brick is hit */
        /* bugged when hit in the corner */
        ball->stepY = -ball->stepY;
        ball->y += ball->stepY;

        /* remove brick */

      }
    }
}
