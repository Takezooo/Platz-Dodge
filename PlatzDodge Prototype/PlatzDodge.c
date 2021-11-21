#include <raylib.h>

#define MAX_ENEMIES 10

typedef struct Player{
    Rectangle rec;
    Vector2 speed;
    Color color;
} Player;

static Player player = { 0 };

int main(){

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    // Init window
    InitWindow(screenWidth, screenHeight, "PRACTICE");

    Texture2D sprite = LoadTexture("sprite.png"); //to fecth the sprite for player
    Texture2D sprite1 = LoadTexture("sprite1.png"); //to fecth the sprite for enemy

    Texture2D bg = LoadTexture("nebula03.png"); //to fecth the background
    Texture2D fg = LoadTexture("mountains.png");

    InitAudioDevice();  // Initialize audio device
    Sound sound = LoadSound("target.ogg"); // Sounds

    float x = 150.0f, y = 370.0f; //initializing and declaring the spawn position of the sprite

    float scrollingBack = 0.0f;// declaration for moving bg
    float scrollingFore = 0.0f;// declaration for moving fg

    // Initialize player
    player.rec.x =  20;
    player.rec.y = 50;
    player.rec.width = sprite.width;
    player.rec.height = sprite.height;
    player.speed.x = 20.0f;
    player.speed.y = 20.0f;
    player.color = BLACK;

    Rectangle enemyBounds[MAX_ENEMIES];
    int enemyRail[MAX_ENEMIES];
    int enemyType[MAX_ENEMIES];
    bool enemyActive[MAX_ENEMIES];
    float enemySpeed = 10;

    float timer = 0.0f; //timer

    // Init enemies variables
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        // Define enemy type (all same probability)
        enemyType[i] = GetRandomValue(0, 3);

        // Define enemy rail
        enemyRail[i] = GetRandomValue(0, 4);

        // Define enemy bounding box
        enemyBounds[i] = (Rectangle){ 1280 + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };
        enemyActive[i] = false;
    }

    int framesCounter = 0;

    SetTargetFPS(60); //setting 60 fps target

    //-----------------------------------------------------------------------------------------------------

    while(!WindowShouldClose()){ // a loop that will run as long as you don't close the window

        SetSoundVolume(sound, 0.9f); //for volume 1.00 is the maximum

        framesCounter++;

        scrollingBack -= 0.1f;
        scrollingFore -= 2.0f;

        // NOTE: Texture is scaled twice its size, so it sould be considered on scrolling
        if (scrollingBack <= -bg.width*2) scrollingBack = 0;
        if (scrollingFore <= -fg.width*2) scrollingFore = 0;

        //-----------------------------------------------
        // for the sound to loop
        if(!IsSoundPlaying(sound)){

            PlaySound(sound);

        }
        //-----------------------------------------------
        //Controls
        //-----------------------------------------------

        if (IsKeyDown(KEY_D)) player.rec.x += player.speed.x;
        if (IsKeyDown(KEY_A)) player.rec.x -= player.speed.x;
        if (IsKeyDown(KEY_W)) player.rec.y -= player.speed.y;
        if (IsKeyDown(KEY_S)) player.rec.y += player.speed.y;
        //----------------------------------------------------------------
        /*
        if(IsKeyDown(KEY_UP)){

            y1 -= GetFrameTime() * 200.0f;

        }

        if(IsKeyDown(KEY_LEFT)){

            x1 -= GetFrameTime() * 200.0f;

        }

        if(IsKeyDown(KEY_DOWN)){

            y1 += GetFrameTime() * 200.0f;

        }

        if (IsKeyDown(KEY_RIGHT)){

            x1 += GetFrameTime() * 200.0f;
        }
        */
        //-----------------------------------------------
        // Wall behaviour
        if (player.rec.x < 0) player.rec.x = 0;
        if (player.rec.x +  player.rec.width > 1280) player.rec.x = 1280 -  player.rec.width;
        if (player.rec.y < 0) player.rec.y = 0;
        if (player.rec.y + player.rec.height > 720) player.rec.y = 720 - player.rec.height;
        //-----------------------------------------------

        // Enemies activation logic (every 40 frames)
        if (framesCounter > 40)
        {
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemyActive[i] == false)
                {
                    enemyActive[i] = true;
                    i = MAX_ENEMIES;
                }
            }

                framesCounter = 0;
        }

        // Enemies logic
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemyActive[i])
            {
                enemyBounds[i].x -= enemySpeed;
            }

        // Enemies speed increase every frame
        if (!CheckCollisionRecs(player.rec, enemyBounds[i])) enemySpeed += 0.005;

        }

        // Check collision player vs enemies
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemyActive[i])
            {
                if (CheckCollisionRecs(player.rec, enemyBounds[i]))
            {
                DrawText("BOBO", 30, 30, 30, WHITE); //Timer (incrementing visual text)
            }

        //---------------------------------------------------------------------------

        BeginDrawing();

        ClearBackground(BLACK);

        DrawTextureEx(bg, (Vector2){ scrollingBack, 0}, 0.0f, 2.0f, WHITE); // for background (Vector2){0,0} if you don't want scrolling bg
        DrawTextureEx(bg, (Vector2){bg.width*2 + scrollingBack, 20 }, 0.0f, 2.0f, WHITE); //(Vector2){ bg.width*2, 20 } 100.0 if you don't want scrolling bg

        DrawTextureEx(fg, (Vector2){ scrollingFore, 450}, 0.0f, 2.0f, WHITE); // for background (Vector2){0,0} if you don't want scrolling fg
        DrawTextureEx(fg, (Vector2){fg.width*2 + scrollingFore, 450}, 0.0f, 2.0f, WHITE); //(Vector2){ bg.width*2, 20 } 100.0 if you don't want scrolling fg

        DrawText(TextFormat("TIME: %.02f", timer += GetFrameTime()), 30, 30, 30, WHITE); //Timer (incrementing visual text)

        //-----------------------------------------------
        // Draw player
        if (!CheckCollisionRecs(player.rec, enemyBounds[i])) DrawTexture(sprite, player.rec.x - 14, player.rec.y - 14, WHITE);
                            // Draw enemies
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemyActive[i])
            {
                // Draw enemies

                switch(enemyType[i])
                {
                    case 0: DrawTexture(sprite1, enemyBounds[i].x - 14, enemyBounds[i].y - 14, WHITE); break;
                    default: break;
                }
            }
        }



        EndDrawing();
    }
    }
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------

    // Unload textures
    UnloadTexture(sprite);
    UnloadTexture(sprite1);
    UnloadTexture(bg);
    UnloadTexture(fg);

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
return 0;
}

//----------------------------------------------------------------

/*
            // Player collision with enemy
            for (int i = 0; i < activeEnemies; i++)
            {
                if (CheckCollisionRecs(player.rec, enemy[i].rec)) gameOver = true;
            }
*/
