#include "raylib.h"

#include "touch.h"


int main(void)
{
        // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib win32 touch input example");
    
    InitTouch(GetWindowHandle(), false);

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText("touch the screen", 10, 10, 20, DARKGRAY);

            for (int i = 0; i < MAX_TOUCHES; i++)
            {
                if (TouchArray[i].TouchID != -1)
                {
                    Vector2 Pos = {(float)TouchArray[i].PointX,
                                   (float)TouchArray[i].PointY};
                    float Radius = 75.0f;
                    DrawCircleV(Pos, Radius, RED);
                }
            } // END for i

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}