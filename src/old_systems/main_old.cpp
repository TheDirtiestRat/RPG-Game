#include <raylib.h>
#include <vector> // Required for collections

#include "entity.hpp"
#include "player-controller.hpp"
#include "dialog-system.hpp"
#include "interactable.hpp"
#include "projectile.hpp"

void UpdateProjectiles(float delta, std::vector<Projectile> &projectiles)
{
    for (auto &projectile : projectiles)
    {
        projectile.Update(delta);
    }

    projectiles.erase(
        std::remove_if(
            projectiles.begin(),
            projectiles.end(),
            [](const Projectile &projectile)
            {
                return !projectile.active;
            }),
        projectiles.end());
}

void DrawProjectiles(std::vector<Projectile> &projectiles)
{
    for (auto &projectile : projectiles)
    {
        projectile.Draw();
    }
}

int main()
{
    InitWindow(800, 800, "Raylib Animation Class");
    SetTargetFPS(60);

    // ==========================================
    // 1. LOAD RESOURCES
    // ==========================================
    Font myFont = LoadFont("src/fonts/Ac437_IBM_BIOS.ttf");
    Texture2D ratText = LoadTexture("src/images/DRATS.png");
    Texture2D axeTexture = LoadTexture("src/images/AXE.png");
    Texture2D catText = LoadTexture("src/images/Cat RPG.png");
    Texture2D dialogBoxTex = LoadTexture("src/images/DialogBox RPG.png");

    // ==========================================
    // 2. INITIALIZE SYSTEMS
    // ==========================================
    DialogSystem dialog_sys(dialogBoxTex);
    dialog_sys.dialogFont = myFont;

    // ==========================================
    // 3. CREATE ENTITIES & INTERACTIONS
    // ==========================================

    // list of projectiles
    std::vector<Projectile> projectiles;

    Vector2 centerPosition = {(float)(GetScreenWidth() / 2), (float)(GetScreenHeight() / 2)};

    Entity player(ratText, centerPosition, 200.0f, 7, 4);
    PlayerController controller(player, projectiles);
    controller.projectileTex = axeTexture;

    Entity npcCat(catText, {600.0f, 100.0f}, 0.0f, 1, 1);

    std::vector<std::string> npcText = {
        "Hi there player?",
        "This is your first time in my RPG",
        "well I hope you enjoy your stay.",
        "Let's Test this long text of paragraph to see how those text fit in the box."};
    Interactable catInteraction(npcCat.areaCollision, npcText, myFont);

    // ==========================================
    // MAIN GAME LOOP
    // ==========================================
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // --- UPDATE LOGIC ---
        if (!dialog_sys.isActive)
        {
            controller.Update(dt);
            player.Update();
        }

        // Only allow NPC interaction and dialog if inventory is closed
        catInteraction.Update(player.areaCollision, dialog_sys);
        dialog_sys.Update();

        // update the projectiles
        UpdateProjectiles(dt, projectiles);

        // --- DRAW LOGIC ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw World Objects First
        player.Draw();
        npcCat.Draw();

        DrawProjectiles(projectiles);

        catInteraction.DrawPrompt();

        // Draw UI Last (So it renders on top of the world)
        dialog_sys.Draw();

        EndDrawing();
    }

    // ==========================================
    // 5. CLEANUP MEMORY
    // ==========================================
    UnloadFont(myFont);
    UnloadTexture(ratText);
    UnloadTexture(catText);
    UnloadTexture(dialogBoxTex);
    UnloadTexture(axeTexture);

    CloseWindow();
    return 0;
}