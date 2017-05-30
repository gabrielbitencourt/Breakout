/* THINGS TO-DO

 *    1. remove brick
 *    2. score/lifes system
 *    3. reflection proportional to where the ball hits the platform
 *    5. make the art
 *    6. game area
 *    7. menu/configurations
 *    8. mouse movement

 * BUGS *

 * 1. ball hits the corner of the platform/brick
 * 2. 2nd and 3rd lines not coliding with ball
 * 3. ball colision with ground and ceiling not working properly

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

/* define boolean type */
typedef int bool;
const int false = 0;
const int true = 1;

/* screen dimension */
const double SCREEN_WIDTH = 720.0;
const double SCREEN_HEIGHT = 480.0;

const double GAMEAREA_W = 720.0;
const double GAMEAREA_H = 480.0;

/* SDL global variables */
SDL_Window* gWindow = NULL;
SDL_Surface* gSurface = NULL;
SDL_Event event;
SDL_Renderer* gRenderer = NULL;

/* players and npcs definitions (ball, platform and bricks) */
SDL_Surface* gBallSurface = NULL;
SDL_Surface* gPlatformSurface = NULL;
SDL_Surface* gRedBrickSurface = NULL;
SDL_Surface* gBlueBrickSurface = NULL;
SDL_Surface* gYellowBrickSurface = NULL;
SDL_Surface* gGreenBrickSurface = NULL;

const double BALL_W = 20.0;
const double BALL_H = 20.0;

const double PLAYER_W = 100.0;
const double PLAYER_H = 20.0;

const double BRICK_W = 50.0;
const double BRICK_H = 30.0;

const int LINES = 9;
const int COLUMNS = (GAMEAREA_W - 5) / (BRICK_W + 5);

typedef struct _PLAYER{
  double x;
  double y;
  double velX;
  double width;
  double height;
  int lives;
  int points;
  SDL_Surface* image;

} PLAYER;
typedef struct _BALL {
    double x;
    double y;
    double r;
    double velX;
    double velY;
    double width;
    double height;
    double xCenter;
    double yCenter;
    SDL_Surface* image;

} BALL;
typedef struct _BRICK{
  double x;
  double y;
  double width;
  double height;
  int line;
  int column;
  SDL_Surface* image;

} BRICK;

/* player and npc functions */
PLAYER createPlayer(double x, double y, double velX, double width, double height, SDL_Surface *);
void movePlayer(PLAYER *);

BALL createBall(double width, double height, SDL_Surface *);
void moveBall(BALL *);

BRICK createBrick(double x, double y, double width, double height, int line, int column, SDL_Surface *image);

void reflexion(BALL *ball, PLAYER *player);
void collision(BALL *ball, BRICK *brick);

bool **removeBricks;

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
unsigned int TICK_INTERVAL = 50;

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
          config();
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

  SDL_Rect textPosition;

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
    textPosition.x = (GAMEAREA_W / 2) - (265);
    textPosition.y = GAMEAREA_H / 2;

    if (SDL_BlitSurface(gMenuText, NULL, gSurface, &textPosition) < 0) {
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
  printf("configuraçoes\n");
}
void game(){

  /* control flow */
  bool quitGame = false;
  bool gameStart = false;

  /* bricks collumns and lines */
  int l, c;

  /* ball, platform and bricks rects */
  SDL_Rect ballSrcRect, ballDstRect;
  SDL_Rect playerSrcRect, playerDstRect;
  SDL_Rect brickSrcRect, brickDstRect;
  SDL_Rect textPosition;

  /* create ball, player and brick */
  BALL ball;
  PLAYER player;
  BRICK brick;
  SDL_Surface *colorBrickSurface = NULL;

  /* init ball and player */
  player = createPlayer((GAMEAREA_W - PLAYER_W) / 2, GAMEAREA_H - 50.0, 0.0, PLAYER_W, PLAYER_H, gPlatformSurface);
  ball = createBall(BALL_W, BALL_H, gBallSurface);

  removeBricks = malloc(COLUMNS * sizeof(bool*));
  for (c = 0; c < COLUMNS; c++) {
    removeBricks[c] = malloc(LINES * sizeof(bool));
  }

  for (c = 0; c < COLUMNS; c++) {
    for (l = 0; l < LINES; l++) {
        removeBricks[c][l] = false;
    }
  }

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

              break;
            case SDLK_RIGHT:
              player.velX = 10;

              break;

            case SDLK_SPACE:
              gameStart = true;
              if (ball.velX == 0 && ball.velY == 0) {
                ball.velX = rand() % 2 ? 3.0: 3.0;
                ball.velY = -3.0;
              }

              break;

            case SDLK_UP:
              TICK_INTERVAL -= 10;
              if (TICK_INTERVAL <= 0) {
                TICK_INTERVAL = 1;
              }

              break;
            case SDLK_DOWN:
              TICK_INTERVAL += 10;

              break;
          }
          break;

        case SDL_KEYUP:
          switch (event.key.keysym.sym) {
            case SDLK_RIGHT: case SDLK_LEFT:
              player.velX = 0.0;

              break;
          }
      }
    }

    /* fill the window with a color */
    SDL_FillRect(gSurface, NULL, SDL_MapRGB(gSurface->format, 0, 0, 0));

    /* move player */
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

      textPosition.x = GAMEAREA_W / 2;
      textPosition.y = GAMEAREA_H / 2;

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
    for (c = 0; c < COLUMNS; c++) {
      for (l = 0; l < LINES; l++) {

        if (removeBricks[c][l] == false) {

          /* color bricks */
          switch ((l % 4)) {
            case 0:
              colorBrickSurface = gRedBrickSurface;
              break;

            case 1:
              colorBrickSurface = gGreenBrickSurface;
              break;

            case 2:
              colorBrickSurface = gBlueBrickSurface;
              break;

            case 3:
              colorBrickSurface = gYellowBrickSurface;
              break;

            default:
              printf("SDL couldn't color blocks, %d\n", l % LINES);
              quit = true;
              quitGame = true;

          }

          brick = createBrick((c * (BRICK_W + 5) + 5), (l * (BRICK_H + 5) + 5), BRICK_W, BRICK_H, l, c, colorBrickSurface);
          colorBrickSurface = NULL;

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
          if (gameStart) {
            collision(&ball, &brick);
          }
        }
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
        gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_SOFTWARE);
        gSurface = SDL_GetWindowSurface(gWindow);
        if (gSurface == NULL) {
          printf("SDL couldn't init surface, error: %s\n", SDL_GetError());

          return false;
        }
        if (gRenderer == NULL) {
          printf("SDL couldn't init renderer, error: %s\n", SDL_GetError());

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
    SDL_FreeSurface(gPlatformSurface);
    SDL_FreeSurface(gRedBrickSurface);
    SDL_FreeSurface(gGreenBrickSurface);
    SDL_FreeSurface(gYellowBrickSurface);
    SDL_FreeSurface(gBlueBrickSurface);

    gBallSurface = NULL;
    gPlatformSurface = NULL;
    gRedBrickSurface = NULL;
    gGreenBrickSurface = NULL;
    gYellowBrickSurface = NULL;
    gBlueBrickSurface = NULL;

    /* Free the music */
    Mix_FreeMusic(gMusic);
    gMusic = NULL;

    /* Free global font */
    TTF_CloseFont(gFont);
    gFont = NULL;

    /* destroy renderer */
    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;

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
  gPlatformSurface = loadSurface("./images/platform.png");
  gRedBrickSurface = loadSurface("./images/brickRed.png");
  gGreenBrickSurface = loadSurface("./images/brickGreen.png");
  gBlueBrickSurface = loadSurface("./images/brickBlue.png");
  gYellowBrickSurface = loadSurface("./images/brickYellow.png");

  /* transparency */
  SDL_SetColorKey(gBallSurface, SDL_TRUE, SDL_MapRGB(gBallSurface->format, 0, 0xFF, 0xFF));
  SDL_SetColorKey(gPlatformSurface, SDL_TRUE, SDL_MapRGB(gPlatformSurface->format, 0, 0xFF, 0xFF));

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
  if (gPlatformSurface == NULL) {
    printf("SDL couldn't load media, error: %s\n", SDL_GetError());

    return false;
  }
  if (gRedBrickSurface == NULL) {
    printf("SDL couldn't load media, error: %s\n", SDL_GetError());

    return false;
  }
  if (gBlueBrickSurface == NULL) {
    printf("SDL couldn't load media, error: %s\n", SDL_GetError());

    return false;
  }
  if (gGreenBrickSurface == NULL) {
    printf("SDL couldn't load media, error: %s\n", SDL_GetError());

    return false;
  }
  if (gYellowBrickSurface == NULL) {
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
  if (gGameStartText == NULL) {
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
PLAYER createPlayer(double posX, double posY, double velX, double width, double height, SDL_Surface *image){

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
BALL createBall(double width, double height, SDL_Surface *image){

  BALL ball;

  ball.r = 10;
  ball.velX = 0;
  ball.velY = 0;
  ball.width = width;
  ball.height = height;
  ball.image = image;

  return ball;
}
BRICK createBrick(double posX, double posY, double width, double height, int line, int column, SDL_Surface *image){

    BRICK brick;

    brick.x = posX;
    brick.y = posY;
    brick.width = width;
    brick.height = height;
    brick.image = image;
    brick.line = line;
    brick.column = column;

    return brick;
}

/* move player and ball */
void movePlayer(PLAYER *player) {
    /* if the player is not going out of bounds move it */
    if (!((player->x + player->width + player->velX > GAMEAREA_W) || (player->x + player->velX < 0))) {
      player->x += player->velX;
    }
}
void moveBall(BALL *ball) {

    /* increment ball with its speed */
    ball->x += ball->velX;
    ball->y += ball->velY;

    /* update the center position */
    ball->xCenter = ball->x + ball->width / 2;
    ball->yCenter = ball->y + ball->height / 2;


    /* if the ball hits the screen limits, bounce it back */
    if ((ball->x + ball->width > GAMEAREA_W) || (ball->x < 0.0)) {
        ball->velX = -ball->velX;
        ball->x += ball->velX;
    }

    /* ball hits the ceilling */
    if (ball->y <= 0.0) {
      ball->velY = -ball->velY;
      ball->y += ball->velY;
    }

    /* ball hits ground */
    if (ball->y + ball->height >= GAMEAREA_H) {
      /* looses life */

      /* play lost a life sound */
      Mix_PlayChannel(-1, gLostALife, 0);

      /* resets position and velocity */
      ball->y = (GAMEAREA_H / 2);
      ball->x = (GAMEAREA_W / 2);

      ball->velX = 0.0; ball->velY = 0.0;
    }
}

/* collision functions */
void reflexion(BALL *ball, PLAYER *player){

  int xCollision, yCollision;

  /* check the x coordinate of the collision */
  if (ball->xCenter < player->x){
    xCollision = player->x;
  }
  else if (ball->xCenter > player->x + player->width){
    xCollision = player->x + player->width;
  }
  else{
    xCollision = ball->xCenter;
  }

  /* check the y coordinate of the collision */
  if (ball->yCenter < player->y) {
    yCollision = player->y;
  }
  else if (ball->yCenter > player->y + player->height) {
    yCollision = player->y + player->height;
  }
  else{
    yCollision = ball->yCenter;
  }

  /* if the collision is inside the circle, it happened */
  if (pow((ball->xCenter - xCollision), 2) + pow((ball->yCenter - yCollision), 2) < pow(ball->r, 2)) {

    /* reflect ball when the platform is hit */
    ball->velY = -ball->velY;
    ball->y += ball->velY;

    /* play bounce sound */
    if (ball->velX != 0.0 && ball->velY != 0.0) {
      Mix_PlayChannel(-1, gBounce, 0.0);
    }
  }
}
void collision(BALL *ball, BRICK *brick){

  int xCollision, yCollision;
  int topOrBottom = -1, rightOrLeft = -1; /*0or1*/
  int collisionSide;
  bool quina = true;

  /* check the x coordinate of the collision */
  if (ball->xCenter < brick->x){ /* collision at left side or left corner */
    xCollision = brick->x;
    rightOrLeft = 0;
    collisionSide = 1;

  }
  else if (ball->xCenter > brick->x + brick->width){ /* collision at right side or right corner */
    xCollision = brick->x + brick->width;
    rightOrLeft = 1;
    collisionSide = 1;

  }
  else{ /* collision at the top or bottom */
    xCollision = ball->xCenter;
    quina = false;
    collisionSide = 0;

  }

  /* check the y coordinate of the collision */
  if (ball->yCenter < brick->y) { /* collision at the top or up corner */
    yCollision = brick->y;
    topOrBottom = 0;
    collisionSide = 0;

  }
  else if (ball->yCenter > brick->y + brick->height) { /* collision at bottom or down corner */
    yCollision = brick->y + brick->height;
    topOrBottom = 1;
    collisionSide = 0;

  }
  else{ /* collision at left or right side */
    yCollision = ball->yCenter;
    quina = false;
    collisionSide = 1;

  }

  /* if the collision is inside the circle, it happened */
  if (pow((ball->xCenter - xCollision), 2) + pow((ball->yCenter - yCollision), 2) < pow(ball->r, 2)) {

    /* collision left/right -> invert velX */
    /* collision upside/downside -> invert velY */
    /* collision corner -> invert both velX and velY */
    if (quina) {
      /* COLLISION IN THE CORNER CAN ONLY HAPPEN IF THE CIRCUNSTANCES ARE OK (VIDE V1) */
      /* printf("quina do bloco [%d][%d], ponto de colisao: (%d, %d), vx = %lf/vy = %lf, tb[%d]/rl[%d]/cs[%d]", brick->column, brick->line, xCollision, yCollision, ball->velX, ball->velY, topOrBottom, rightOrLeft, collisionSide); */

      /* y invert */
      if ((topOrBottom == 1 && ball->velY < 0 ) || (topOrBottom == 0 && ball->velY > 0)) {
        ball->velY = -ball->velY;
        ball->y += ball->velY;
      }

      /* x invert */
      if ((rightOrLeft == 1 && ball->velX < 0) || (rightOrLeft == 0 && ball->velX > 0)) {
        ball->velX = -ball->velX;
        ball->x += ball->velX;
      }
    }
    else{
      switch (collisionSide) {
        case 0:
          /* y invert */
          ball->velY = -ball->velY;
          ball->y += ball->velY;

          break;

        case 1:
          /* x invert */
          ball->velX = -ball->velX;
          ball->x += ball->velX;

          break;
      }
    }

    /* play boom sound */
    Mix_PlayChannel(-1, gDestroyedBrick, 0);

    /* remove brick */
    removeBricks[brick->column][brick->line] = true;
  }
}
