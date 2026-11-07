#include <raylib.h>
#include <src/game.hpp>

int main()
{
    InitWindow(800, 600, "Conque the Castle");
    SetTargetFPS(60);

    Game game;
    if (!game.Load())
    {
        game.Unload();
        CloseWindow();
        return 1;
    }

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        game.Update(dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        game.Draw();

        EndDrawing();
    }

    game.Unload();
    CloseWindow();
    return 0;
}