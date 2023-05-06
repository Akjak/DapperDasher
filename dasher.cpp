#include "raylib.h"

// Struct for both scarfy and nebula hazards for animation
struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

// Function that updates the animation of both scarfy and the nebula
AnimData updateAnimData(AnimData data, float deltatime, int maxFrame)
{
    // update running time
    data.runningTime += deltatime;
    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        // update the animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }
    return data;
}

int main()
{

    // Array containing window dimensions
    int windowDimensions[2];
    windowDimensions[0] = 1350;
    windowDimensions[1] = 1000;

    // Intializes the window
    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper Dasher!");

    // Acceleration due to gravity (pixels/frame)/frame
    const int gravity{1'200};

    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    // Determines the number of nebula
    const int sizeOfNebulae{10};

    // Array containing nebula
    AnimData nebulae[sizeOfNebulae]{};

    // Loop assigns variables to the nebula
    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].rec.height = nebula.height/8;
        nebulae[i].pos.y = windowDimensions[1] - nebula.height/8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 1.0/16.0;
        nebulae[i].pos.x = windowDimensions[0] + i * 600;  
    }

    // Finish line follows last nebula
    float finishLine{ nebulae[sizeOfNebulae - 1].pos.x };

    // Nebula directional (<-) velocity (pixels/sec)
    int nebVel{-375};
    
    // Scarfy variables
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData;
    scarfyData.rec.width = scarfy.width/6;
    scarfyData.rec.height = scarfy.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x = windowDimensions[0]/2 - scarfyData.rec.width/2;
    scarfyData.pos.y = windowDimensions[1] -scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.updateTime = 1.0/12.0;
    scarfyData.runningTime = 0.0;

    bool isInAir{};
    // Jump velocity (pixels/sec)
    const int jumpVel{-600};

    int Velocity{0};

    // Background textures
    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgX{};
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX{};

    bool collision{false};

    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        // DeltaTime
        const float dT{GetFrameTime()};

        // Start drawing
        BeginDrawing();
        ClearBackground(WHITE);

        // background image scroll speeds
        bgX -= 30 * dT; // background
        mgX -= 60 * dT; // midground
        fgX -= 90 * dT; // foreground

        // resets background images once end is reached
        if (bgX <= -background.width * 5.3)
        {
            bgX = 0.0;
        }
        if (mgX <= -midground.width * 5.3)
        {
            mgX = 0.0;
        }
        if (fgX <= -foreground.width * 5.3)
        {
            fgX = 0.0;
        }

        // Draw the background
        Vector2 bg1Pos{bgX, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 5.3, WHITE);
        Vector2 bg2Pos{bgX + background.width * 5.3, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 5.3, WHITE);

        // Draw the midground
        Vector2 mg1Pos{mgX, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 5.3, WHITE);
        Vector2 mg2Pos{mgX + midground.width * 5.3, 0.0};
        DrawTextureEx(midground, mg2Pos, 0.0, 5.3, WHITE);

        // Draw the foreground
        Vector2 fg1Pos{fgX, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 5.3, WHITE);
        Vector2 fg2Pos{fgX + foreground.width * 5.3, 0.0};
        DrawTextureEx(foreground, fg2Pos, 0.0, 5.3, WHITE);

        // Ground check
        if (isOnGround(scarfyData, windowDimensions[1]))
        {
            isInAir = false;
            Velocity = 0;
        }
        else
        {
            isInAir = true;
            // Apply gravity
            Velocity += gravity * dT;
        }

        // Jumping / Prevents double jumping
        if (IsKeyPressed(KEY_SPACE) && isInAir == false)
        {
            Velocity = jumpVel;
        }
        

        // Updates the position of the nebulas
        for(int i = 0; i < sizeOfNebulae; i++)
        {
            
            nebulae[i].pos.x += nebVel * dT;
        }

        // Update finishLine
        finishLine += nebVel * dT;

        // Update scarfy position
        scarfyData.pos.y += Velocity * dT;

    // Freezes scarfy animation if not on the ground
     if (!isInAir)
    {
        scarfyData = updateAnimData(scarfyData, dT, 5);
    }

    // Spawns the nebula
    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i] = updateAnimData(nebulae[i], dT, 7);
    }

    // Collision checker
    for (AnimData nebula : nebulae)
    {
        float pad{55}; // <-- Tightens up the hit box of the nebula
        Rectangle nebRec{
            nebula.pos.x + pad, // <-
            nebula.pos.y + pad, // <-
            nebula.rec.width - 2*pad, // <-
            nebula.rec.height - 2*pad // <-
        };
        Rectangle scarfyRec{
            scarfyData.pos.x,
            scarfyData.pos.y,
            scarfyData.rec.width,
            scarfyData.rec.height
        };
        if (CheckCollisionRecs(nebRec, scarfyRec))
        {
            collision = true;
        }
    }
        
    if (collision)
    {
        // Lose the game
        DrawText("Game Over!",
        windowDimensions[0] -900, 
        windowDimensions[1]/2, 
        100, 
        WHITE);
    }
    else if (!collision && scarfyData.pos.x >= finishLine)
    {
        // Win the gane
        DrawText("You Win!", 
        windowDimensions[0] -850, 
        windowDimensions[1]/2, 
        100, 
        GOLD);
    }
    else
    {
        // Characters are drawn only if win/lose conditions aren't met
        for (int i = 0; i < sizeOfNebulae; i++)
        {
            // Draw nebula
            DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
        }
       
            // Draw scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
    }

        // Stop drawing
        EndDrawing();
    }

    // Cleans up the memory before the game closes
    UnloadTexture(nebula);
    UnloadTexture(scarfy); 
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);   
    CloseWindow();
}