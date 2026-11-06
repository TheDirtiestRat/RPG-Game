#include <raylib.h>
#include <game.hpp>

int main()
{
    InitWindow(800, 600, "Conque the Castle");
    SetTargetFPS(60);

    Font myFont = LoadFont("src/fonts/Ac437_IBM_BIOS.ttf");
    Texture2D ratText = LoadTexture("src/images/DRATS.png");
    Texture2D axeTexture = LoadTexture("src/images/AXE.png");
    Texture2D catText = LoadTexture("src/images/Cat RPG.png");
    Texture2D dialogBoxTex = LoadTexture("src/images/DialogBox RPG.png");

    Game game;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        game.Update();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        game.Draw();

        EndDrawing();
    }

    UnloadFont(myFont);
    UnloadTexture(ratText);
    UnloadTexture(catText);
    UnloadTexture(dialogBoxTex);
    UnloadTexture(axeTexture);

    CloseWindow();
    return 0;
}