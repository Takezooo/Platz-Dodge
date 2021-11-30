#include "raylib.h"

//----------------------------------------------------------------------------------
#define NUM_MAX_ENEMIES 50
#define WAVE 20
//----------------------------------------------------------------------------------

// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum { FIRST = 0 } EnemyWave;
typedef enum { TITLE = 0, GAMEPLAY, ENDING } GameScreen;

typedef struct Player
{
    Rectangle rec;
    Vector2 speed;
} Player;

typedef struct Enemy
{
    Rectangle rec;
    Vector2 speed;
    bool active;
} Enemy;
//----------------------------------------------------------------------------------

// Global Variables Definition
//----------------------------------------------------------------------------------
static const int screenWidth = 1280;
static const int screenHeight = 720;

static bool gameOver = false;
static bool pause =  false;

static Player player = { 0 };
static Enemy enemy[NUM_MAX_ENEMIES] = { 0 };

static int activeEnemies = 0;
static float alpha = 0.0f;
static bool smooth = false;

Font font;
Texture2D playerSprite;
Texture2D enemySprite;
Texture2D bg ;
Texture2D fg;
Texture2D title;
Texture2D playBtn;
Texture2D gameover;
Sound crash;

//------------------------------------------------------------------------------------

// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateDrawFrame(void);  // Update and Draw (one frame)
//------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Define scrolling variables
float scrollingBack = 0.0f;// declaration for moving bg
float scrollingFore = 0.0f;// declaration for moving fg

// Define current screen
GameScreen currentScreen = 0;
// Define additional game variables
float distance = 0.0f;
float hidistance = 0.0f;

//----------------------------------------------------------------------------------


// Module Functions Declaration
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void);     // Update and Draw one frame
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "Platz Dodge"); // Init window
    InitAudioDevice(); // Initialize audio device and context
    crash = LoadSound("Resources/PlatzDodge Prototype_CrashFx.ogg"); // Load sound from file
    font = LoadFont("Resources/gbb__.ttf");
    playerSprite = LoadTexture("Resources/player.png"); //to fecth the sprite for player
    enemySprite = LoadTexture("Resources/enemy.png"); //to fecth the sprite for enemy
    bg = LoadTexture("Resources/background.png"); //to fecth the background
    fg = LoadTexture("Resources/mountains.png");
    title = LoadTexture("Resources/Game Title 01.png"); // Load title texture
    playBtn= LoadTexture("Resources/Play Button 02.png"); // Load button texture
    gameover = LoadTexture("Resources/Game Over Purple 01.png"); // Load gameover texture

    //----------------------------------------------------------------------------------
    Image icon = LoadImage("Resources/PlatzDodgeLogoNoBG.png");
    SetWindowIcon(icon);

    InitGame();

    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    while (!WindowShouldClose())    // Loop: Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadImage(icon);
    UnloadTexture(playerSprite);
    UnloadTexture(enemySprite);
    UnloadTexture(bg);
    UnloadTexture(fg);
    UnloadFont(font);
    CloseAudioDevice();     // Close the audio device and context
    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Initialize game variables
void InitGame(void)
{
    // Initialize game variables
    pause = false;
    gameOver = false;
    activeEnemies = WAVE;
    smooth = false;
    alpha = 0;

    // Initialize player
    player.rec.x =  30;
    player.rec.y = 90;
    player.rec.width = playerSprite.width;
    player.rec.height = playerSprite.height;
    player.speed.x = 10;
    player.speed.y = 10;

    // Initialize enemies
    for (int i = 0; i < NUM_MAX_ENEMIES; i++)
    {
        enemy[i].rec.width = enemySprite.width; //Enemy Width
        enemy[i].rec.height = enemySprite.height; //Enemy Height
        enemy[i].rec.x = GetRandomValue(screenWidth, screenWidth + 1000);
        enemy[i].rec.y = GetRandomValue(0, screenHeight - enemy[i].rec.height);
        enemy[i].speed.x = 10;
        enemy[i].speed.y = 10;
        enemy[i].active = true;
    }
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void)
{
    // Update
    // Game screens management
    switch (currentScreen)
    {
    case TITLE:
    {
        // Mountain scrolling
        scrollingFore -= 5;
        if (scrollingFore <= -screenWidth) scrollingFore = 0;

    }
    break;
    case GAMEPLAY:
    {
        // Background scrolling logic
        scrollingBack -= 0.5f;
        if (scrollingBack <= -screenWidth) scrollingBack = 0;

        // Mountain scrolling logic
        scrollingFore -= 5.0f;
        if (scrollingFore <= -screenWidth) scrollingFore = 0;

        //Game Pause
        if (!gameOver)
        {
            if (IsKeyPressed('P')) pause = !pause;

            if (!pause)
            {

                if (!smooth)
                {
                    alpha += 0.02f;

                    if (alpha >= 1.0f) smooth = true;


                    if (smooth) alpha -= 0.02f;

                    if (0 == activeEnemies)
                    {

                        for (int i = 0; i < activeEnemies; i++)
                        {
                            if (!enemy[i].active) enemy[i].active = true;
                        }
                    }
                }

                // Player movement
                if (IsKeyDown('W')) player.rec.y -= player.speed.y;
                if (IsKeyDown('S')) player.rec.y += player.speed.y;
                if (IsKeyDown(KEY_UP)) player.rec.y -= player.speed.y;
                if (IsKeyDown(KEY_DOWN)) player.rec.y += player.speed.y;

                // Player collision with enemy
                for (int i = 0; i < activeEnemies; i++)
                {
                    if (!CheckCollisionRecs(player.rec, enemy[i].rec))
                    {
                        distance += 0.5f;
                        if (distance > hidistance) hidistance = distance;

                    }
                    else
                    {
                        PlaySound(crash);
                        gameOver = true;
                    }
                }

                // Enemy behaviour
                for (int i = 0; i < activeEnemies; i++)
                {
                    if (enemy[i].active)
                    {
                        enemy[i].rec.x -= enemy[i].speed.x;
                        enemy[i].speed.x += 0.005f;

                        if (enemy[i].rec.x < 0)
                        {
                            enemy[i].rec.x = GetRandomValue(screenWidth, screenWidth + 500);
                            enemy[i].rec.y = GetRandomValue(0, screenHeight - enemy[i].rec.height);
                        }
                    }
                }

                // Wall behaviour
                if (player.rec.x <= 0) player.rec.x = 0;
                if (player.rec.x + player.rec.width >= screenWidth) player.rec.x = screenWidth - player.rec.width;
                if (player.rec.y <= 0) player.rec.y = 0;
                if (player.rec.y + player.rec.height >= screenHeight) player.rec.y = screenHeight - player.rec.height;
            }
        }

        else
        {
            currentScreen = ENDING;
        }
    }
    break;
    case ENDING:
    {
        if(!IsSoundPlaying(crash))
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                InitGame();
                currentScreen = GAMEPLAY;
                distance = 0;
            }
        }
    }
    break;
    default:
        break;
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(BLACK);

    DrawTextureEx(bg, (Vector2)
    {
        scrollingBack, 0
    }, 0.0f, 2.0f, WHITE); // for background (Vector2){0,0} if you don't want scrolling bg
    DrawTextureEx(bg, (Vector2)
    {
        bg.width*2 + scrollingBack, 20
    }, 0.0f, 2.0f, WHITE); //(Vector2){ bg.width*2, 20 } 100.0 if you don't want scrolling bg
    DrawTextureEx(fg, (Vector2)
    {
        scrollingFore, 450
    }, 0.0f, 2.0f, WHITE); // for background (Vector2){0,0} if you don't want scrolling fg
    DrawTextureEx(fg, (Vector2)
    {
        fg.width*2 + scrollingFore, 450
    }, 0.0f, 2.0f, WHITE); //(Vector2){ bg.width*2, 20 } 100.0 if you don't want scrolling fg

    Vector2 mousePoint = { 0.0f, 0.0f };

    // Define frame rectangle for drawing
    Rectangle sourceRec = { 0, 0, (float)playBtn.width, (float)playBtn.height };

    // Define play button bounds on screen
    Rectangle playBtnBounds = { 582, 400, (float)playBtn.width, (float)playBtn.height };

    bool playBtnAction = false;         // Button action should be activated

    switch (currentScreen)
    {
    case TITLE:
    {
        mousePoint = GetMousePosition();
        DrawTexture(title, 443, 100, WHITE);
        DrawTextureRec(playBtn, sourceRec, (Vector2)
        {
            playBtnBounds.x, playBtnBounds.y
        }, WHITE); // Draw button frame
        // Check button state
        if (CheckCollisionPointRec(mousePoint, playBtnBounds)) //Change screen when the button is clicked
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) currentScreen = GAMEPLAY;
        }

    }
    break;
    case GAMEPLAY:
    {
        DrawTexture(playerSprite, player.rec.x, player.rec.y, WHITE);

        for (int i = 0; i < activeEnemies; i++)
        {
            if (enemy[i].active) DrawTexture(enemySprite, enemy[i].rec.x - 14, enemy[i].rec.y - 20, WHITE);
        }

        DrawTextEx(font, TextFormat("DISTANCE: %04i", (int)distance), (Vector2)
        {
            20, 20
        }, font.baseSize, 3, GRAY);

        if (pause)DrawTextEx(font, TextFormat("GAME PAUSED"), (Vector2)
        {
            screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40
        }, 50, 3, GRAY);

    }
    break;
    case ENDING:
    {
        DrawTextEx(font, TextFormat("DISTANCE: %04i", (int)distance), (Vector2)
        {
            420, 380
        }, 30, 3, YELLOW);
        DrawTextEx(font, TextFormat("LONGEST DISTANCE: %04i", (int)hidistance), (Vector2)
        {
            420, 430
        }, 30, 3, YELLOW);

        DrawTexture(gameover, 400, 100, WHITE);

        //DrawText("PRESS [ENTER] TO PLAY AGAIN", 420, GetScreenHeight()/2 - 50, 30, WHITE);
    }
    break;
    default:
        break;
    }

    EndDrawing();
    //----------------------------------------------------------------------------------
}
