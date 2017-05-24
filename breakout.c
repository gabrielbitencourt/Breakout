/* THINGS TO-DO

 *    1. remove brick
 *    2. score/lifes system
 *    3. reflection proportional to where the ball hit the platform
 *    4. fix bug when ball hits the corner of the platform/brick
 *    5. make the art
 *    6. game area
 *    7. menu/configurations
 *    8. mouse movement

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

/* define boolean type */
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

/* players and npcs definitions (ball, platform and bricks) */
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
  int velX;
  int width;
  int height;
  int lives;
  int points;
  SDL_Surface* image;

} PLAYER;
typedef struct _BALL {
    int x;
    int y;
    int velX;
    int velY;
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

/* player and npc functions */
PLAYER createPlayer(int x, int y, int velX, int width, int height, SDL_Surface *);
void movePlayer(PLAYER *);

BALL createBall(int width, int height, SDL_Surface *);
void moveBall(BALL *);

BRICK createBrick(int x, int y, int width, int height, SDL_Surface *image);

void reflexion(BALL *ball, PLAYER *player);
void collision(BALL *ball, BRICK *brick);

/* init and deinit functions */
bool init();
void deinit();

/* game and menu screen functions */
int screen = 1;
bool quit = false;
void menu();
void config();
void game();

/* Fixed framerate

 * Formula: 1000 / TICK_INTERVAL = desired fps
 * TICK_INTERVAL = 16 (~62 fps)
 * TICK_INTERVAL = 17 (~58 fps)
 * TICK_INTERVAL = 33 (~30 fps)

*/
const unsigned int TICK_INTERVAL = 17;

/* framerate functions */
unsigned time_left(void);

/* Control variable for optimal FPS handling */
static Uint32 next_time;

/* load media functions */
bool loadMedia();
SDL_Surface* loadSurface(char *path);

/* sound track */
Mix_Music *gMusic = NULL;

/* sound effects */
Mix_Chunk *gLostALife = NULL;
Mix_Chunk *gBounce = NULL;
Mix_Chunk *gGameOver = NULL;
Mix_Chunk *gDestroyedBrick = NULL;
Mix_Chunk *gDestroyedAllBricks = NULL;
Mix_Chunk *gWonALife = NULL;

/* true-type font declaration */
TTF_Font *gFont = NULL;

/* text declaration */
SDL_Color gTextColor = {255, 255, 255};
SDL_Surface *gMenuText = NULL;
SDL_Surface *gGameStartText = NULL;

int main(int argc, char const *argv[]) {

  if (!init() || !loadMedia()) {
    printf("SDL didn't init properly\n");
  }
  else{
    while (!quit) {
      switch (screen) {
        case 1:
          menu();
          break;
        case 2:

          break;
        case 3:
          game();
          break;
      }
    }
  }

  deinit();
  return 0;
}

/* menu, config and game screen functions */
void menu(){

  /* control flow */
  bool quitMenu = false;

  while (!quitMenu) {
    /* handles events */
    while (SDL_PollEvent(&event) != 0){
      /* user quit window */
      switch (event.type) {
        case SDL_QUIT:
          quit = true;
          quitMenu = true;
          break;

        /* user press a key */
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
              quit = true;
              quitMenu = true;
              break;

            case SDLK_RETURN:
              screen = 3;
              quitMenu = true;
              break;
          }
      }
    }

    /* fill the window with a color */
    SDL_FillRect(gSurface, NULL, SDL_MapRGB(gSurface->format, 0x4A, 0xCA, 0xEF));

    /* present text */
    if (SDL_BlitSurface(gMenuText, NULL, gSurface, NULL) < 0) {
      printf("SDL could not blit! SDL Error: %s\n", SDL_GetError());
      quit = true;
      quitMenu = true;
    }

    /* Update the surface */
    SDL_UpdateWindowSurface(gWindow);

    /* Normalize framerate */
    SDL_Delay(time_left());
    next_time += TICK_INTERVAL;

  }
}
void config(){

}
void game(){

  /* control flow */
  bool quitGame = false;

  /* bricks collumns and lines */
  int l, c;

  /* ball, platform and bricks rects */
  SDL_Rect ballSrcRect, ballDstRect;
  SDL_Rect playerSrcRect, playerDstRect;
  SDL_Rect brickSrcRect, brickDstRect;
  SDL_Rect textPosition;

  /* create ball and player */
  BALL ball;
  PLAYER player;
  BRICK brick;

  /* init ball and player */
  player = createPlayer((SCREEN_WIDTH - PLAYER_W) / 2, SCREEN_HEIGHT - 50, 0, PLAYER_W, PLAYER_H, gPlayerSurface);
  ball = createBall(BALL_W, BALL_H, gBallSurface);

  while (!quitGame) {

    /* handle events */
    while (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
        /* user quit window */
        case SDL_QUIT:
          quit = true;
          quitGame = true;
          break;

        /* user press a key */
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
              quit = true;
              quitGame = true;

              break;
            case SDLK_LEFT:
              player.velX = -10;
              movePlayer(&player);

              break;
            case SDLK_RIGHT:
              player.velX = 10;
              movePlayer(&player);
              break;

            case SDLK_SPACE:
              if (ball.velX == 0 && ball.velY == 0) {
                ball.velX = rand() % 2 ? -1: 1;
                ball.velY = -1;
              }

              break;
          }
      }
    }

    /* fill the window with a color */
    SDL_FillRect(gSurface, NULL, SDL_MapRGB(gSurface->format, 0x4A, 0xCA, 0xEF));

    /* move player */
    player.velX = 0;
    movePlayer(&player);

    playerSrcRect.x = 0; playerSrcRect.y = 0;
    playerSrcRect.w = player.width;
    playerSrcRect.h = player.height;

    playerDstRect.x = player.x;
    playerDstRect.y = player.y;

    if(SDL_BlitSurface(player.image, &playerSrcRect, gSurface, &playerDstRect) < 0 ) {
        printf("SDL could not blit! SDL Error: %s\n", SDL_GetError());
        quit = true;
        quitGame = true;
    }

    /* if the ball is not moving (because the game just started or because the player lost a live)
       the ball should be at the center of the platform */
    if (ball.velX == 0 && ball.velY == 0) {
      ball.x = player.x + (player.width / 2) - (ball.width / 2);
      ball.y = player.y - player.height;

      textPosition.x = SCREEN_WIDTH / 2;
      textPosition.y = SCREEN_HEIGHT / 2;

      if (SDL_BlitSurface(gGameStartText, NULL, gSurface, &textPosition) < 0) {
        printf("SDL could not blit! SDL Error: %s\n", SDL_GetError());
        quit = true;
        quitGame = true;
      }
    }
    /* move ball */
    else{
      moveBall(&ball);
      reflexion(&ball, &player);

    }

    ballSrcRect.x = 0; ballSrcRect.y = 0;
    ballSrcRect.w = ball.width;
    ballSrcRect.h = ball.height;

    ballDstRect.x = ball.x;
    ballDstRect.y = ball.y;

    if(SDL_BlitSurface(ball.image, &ballSrcRect, gSurface, &ballDstRect) < 0 ) {
        printf("SDL could not blit! SDL Error: %s\n", SDL_GetError());
        quit = true;
        quitGame = true;
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
          quitGame = true;
        }

        collision(&ball, &brick);
      }
    }

    /* Update the surface */
    SDL_UpdateWindowSurface(gWindow);

    /* Normalize framerate */
    SDL_Delay(time_left());
    next_time += TICK_INTERVAL;
  }
}

/* init and deinit functions */
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

          /* Initialize SDL_mixer */
          if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
              printf("SDL couldn't initialize the audio mixer, error: %s\n", Mix_GetError());

              return false;
          }

          /* Initialize SDL_ttf */
          if(TTF_Init() == -1){
              printf("SDL couldn't initialize the true-type font, error: %s\n", TTF_GetError());

              return false;
          }
        }
      }
    }
  }

  return true;
}
void deinit(){
    /* Free loaded image */
    SDL_FreeSurface(gBallSurface);
    SDL_FreeSurface(gPlayerSurface);
    SDL_FreeSurface(gBrickSurface);

    gBallSurface = NULL;
    gPlayerSurface = NULL;
    gBrickSurface = NULL;

    /* Free the music */
    Mix_FreeMusic(gMusic);
    gMusic = NULL;

    /* Free global font */
    TTF_CloseFont(gFont);
    gFont = NULL;

    /* Destroy window */
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    /*Quit SDL subsystems*/
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

/* framerate functions */
unsigned time_left(void) {
    unsigned now = SDL_GetTicks();
    return (next_time <= now) ? 0 : next_time - now;
}

/* load media and image surfaces */
bool loadMedia(){

  /* init ball and player image */
  gBallSurface = loadSurface("./images/ball.png");
  gPlayerSurface = loadSurface("./images/player.jpeg");
  gBrickSurface = loadSurface("./images/brick.jpeg");

  /* transparency */
  SDL_SetColorKey(gBallSurface, SDL_TRUE, SDL_MapRGB(gBallSurface->format, 0xFF, 0xFF, 0xFF));

  /* Load sound effects */
  gLostALife = Mix_LoadWAV("./soundeffects/lostlife.wav");
  gBounce = Mix_LoadWAV("./soundeffects/bounce.wav");
  gGameOver = Mix_LoadWAV("./soundeffects/gameover.wav");
  gDestroyedBrick = Mix_LoadWAV("./soundeffects/destroyedbrick.wav");
  gDestroyedAllBricks = Mix_LoadWAV("./soundeffects/destroyedallbricks.wav");
  gWonALife = Mix_LoadWAV("./soundeffects/wonlife.wav");

  /* init text surface and font */
  gFont = TTF_OpenFont("./fonts/font.ttf", 28);
  gMenuText = TTF_RenderText_Solid(gFont, "Press enter to start game or esc to quit", gTextColor);
  gGameStartText = TTF_RenderText_Solid(gFont, "Press space, bitch!", gTextColor);

  /* check if images loaded */
  if (gBallSurface == NULL) {
    printf("SDL couldn't load media, error: %s\n", SDL_GetError());

    return false;
  }
  if (gPlayerSurface == NULL) {
    printf("SDL couldn't load media, error: %s\n", SDL_GetError());

    return false;
  }
  if (gBrickSurface == NULL) {
    printf("SDL couldn't load media, error: %s\n", SDL_GetError());

    return false;
  }

  /* check if sounds loaded */
  if(gLostALife == NULL){
    printf("SDL couldn't load media, error: %s\n", Mix_GetError());

    return false;
  }
  if(gBounce == NULL){
    printf("SDL couldn't load media, error: %s\n", Mix_GetError());

    return false;
  }
  if(gGameOver == NULL){
    printf("SDL couldn't load media, error: %s\n", Mix_GetError());

    return false;
  }
  if(gDestroyedBrick == NULL){
    printf("SDL couldn't load media, error: %s\n", Mix_GetError());

    return false;
  }
  if(gDestroyedAllBricks == NULL){
    printf("SDL couldn't load media, error: %s\n", Mix_GetError());

    return false;
  }
  if(gWonALife == NULL){
    printf("SDL couldn't load media, error: %s\n", Mix_GetError());

    return false;
  }

  /* check if text loaded */
  if(gFont == NULL){
      printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());

      return false;
  }
  if (gMenuText == NULL) {
    printf("Failed to load text, error: %s\n", TTF_GetError());

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

/* create player, ball and brick functions */
PLAYER createPlayer(int posX, int posY, int velX, int width, int height, SDL_Surface *image){

    PLAYER player;

    player.x = posX;
    player.y = posY;
    player.velX = velX;
    player.width = width;
    player.height = height;
    player.image = image;
    player.lives = 6;
    player.points = 0;

    return player;
}
BALL createBall(int width, int height, SDL_Surface *image){

  BALL ball;

  ball.velX = 0;
  ball.velY = 0;
  ball.width = width;
  ball.height = height;
  ball.image = image;

  return ball;
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

/* move player and ball */
void movePlayer(PLAYER *player) {
    /* if the player is not going out of bounds move it */
    if (!((player->x + player->width + player->velX > SCREEN_WIDTH) || (player->x + player->velX < 0))) {
      player->x += player->velX;
    }
}
void moveBall(BALL *ball) {
    /* increment ball with its speed */
    ball->x += ball->velX;
    ball->y += ball->velY;

    /* if the ball hits the screen limits, bounce it back */
    if ((ball->x + ball->width > SCREEN_WIDTH) || (ball->x < 0)) {
        ball->velX = -ball->velX;
        ball->x += ball->velX;
    }
    if ((ball->y + ball->height > SCREEN_HEIGHT) || (ball->y < 0)) {
        ball->velY = -ball->velY;
        ball->y += ball->velY;

        if (ball->y + ball->height == SCREEN_HEIGHT) {
          /* looses life */

          /* play lost a life sound */
          Mix_PlayChannel(-1, gLostALife, 0);

          /* resets position */
          ball->y = (SCREEN_HEIGHT / 2);
          ball->x = (SCREEN_WIDTH / 2);

          ball->velX = 0; ball->velY = 0;
        }
    }
}

/* collision functions */
/* BUG - when hit platform/brick  the corner */
void reflexion(BALL *ball, PLAYER *player){
  if (ball->y + ball->height == player->y) {
    if ((ball->x >= player->x) && (ball->x <= player->x + player->width)) {

      /* reflect ball when the platform is hit */
      ball->velY = -ball->velY;
      ball->y += ball->velY;

      /* play bounce sound */
      if (ball->velX != 0 && ball->velY != 0) {
        Mix_PlayChannel(-1, gBounce, 0);
      }


    }
  }
}
void collision(BALL *ball, BRICK *brick){
    if (ball->y == brick->y + brick->height) {
      if ((ball->x >= brick->x) && (ball->x <= brick->x + brick->width)) {

        /* reflect ball when the brick is hit */
        ball->velY = -ball->velY;
        ball->y += ball->velY;

        /* play boom sound */
        Mix_PlayChannel(-1, gDestroyedBrick, 0);

        /* remove brick */

      }
    }
}
