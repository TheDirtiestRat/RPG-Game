#include <raylib.h>
#include <vector> // Required for collections

#include "entity.hpp"
#include "player-controller.hpp"
#include "dialog-system.hpp"
#include "interactable.hpp"
#include "inventory/inventory_manager.hpp"
#include "inventory/item-pickup.hpp"

int main()
{
    InitWindow(800, 800, "Raylib Animation Class");
    SetTargetFPS(60);

    // ==========================================
    // 1. LOAD RESOURCES
    // ==========================================
    Font myFont = LoadFont("src/fonts/Ac437_IBM_BIOS.ttf");
    Texture2D ratText = LoadTexture("src/images/Rat RPG.png");
    Texture2D catText = LoadTexture("src/images/Cat RPG.png");
    Texture2D dialogBoxTex = LoadTexture("src/images/DialogBox RPG.png");
    Texture2D potionTex = LoadTexture("src/images/Item_Potion.png");
    Texture2D inventoryTex = LoadTexture("src/images/Inventory Box.png");

    // ==========================================
    // 2. INITIALIZE SYSTEMS
    // ==========================================
    DialogSystem dialog_sys(dialogBoxTex);
    dialog_sys.dialogFont = myFont;

    Inventory inventoryManager(myFont);
    inventoryManager.bgTexture = inventoryTex;

    // ==========================================
    // 3. CREATE ENTITIES & INTERACTIONS
    // ==========================================
    Vector2 centerPosition = {(float)(GetScreenWidth() / 2), (float)(GetScreenHeight() / 2)};
    
    Entity player(ratText, centerPosition, 200.0f, 9);
    PlayerController controller(player);

    Entity npcCat(catText, {600.0f, 100.0f}, 0.0f, 1);
    
    std::vector<std::string> npcText = {
        "Hi there player?",
        "This is your first time in my RPG",
        "well I hope you enjoy your stay.",
        "Let's Test this long text of paragraph to see how those text fit in the box."
    };
    Interactable catInteraction(npcCat.areaCollision, npcText, myFont);

    // ==========================================
    // 4. ITEM COLLECTION (Ground Drops)
    // ==========================================
    std::vector<ItemPickup> groundItems;
    
    // Easily add items by pushing them into the vector
    groundItems.push_back(ItemPickup({250.0f, 250.0f, (float)potionTex.width, (float)potionTex.height}, 
                                     {"Life Potion", ItemType::HEAL, 50, 1, potionTex}));
                                     
    groundItems.push_back(ItemPickup({500.0f, 400.0f, (float)potionTex.width, (float)potionTex.height}, 
                                     {"Damn Potion", ItemType::MANA, 50, 1, potionTex}));
                                     
    groundItems.push_back(ItemPickup({300.0f, 400.0f, (float)potionTex.width, (float)potionTex.height}, 
                                     {"What a potion", ItemType::KEY_ITEM, 50, 1, potionTex}));

    // ==========================================
    // MAIN GAME LOOP
    // ==========================================
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // --- UPDATE LOGIC ---
        
        // 1. Move Player (Only if both UIs are closed)
        if (!dialog_sys.isActive && !inventoryManager.isOpen)
        {
            controller.Update(dt);
            player.Update();
        }

        // 2. Update all ground items via a loop
        for (auto& drop : groundItems) 
        {
            drop.Update(player.areaCollision, inventoryManager);
        }

        // 3. UI State Machine (Mutually Exclusive)
        if (!inventoryManager.isOpen) 
        {
            // Only allow NPC interaction and dialog if inventory is closed
            catInteraction.Update(player.areaCollision, dialog_sys);
            dialog_sys.Update();
        }

        if (!dialog_sys.isActive) 
        {
            // Only allow inventory usage if dialog is closed
            inventoryManager.Update(player);
        }

        // --- DRAW LOGIC ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw World Objects First
        player.Draw();
        npcCat.Draw();
        
        for (auto& drop : groundItems) 
        {
            drop.Draw();
        }

        catInteraction.DrawPrompt();

        // Draw UI Last (So it renders on top of the world)
        dialog_sys.Draw();
        inventoryManager.Draw();

        EndDrawing();
    }

    // ==========================================
    // 5. CLEANUP MEMORY
    // ==========================================
    UnloadFont(myFont);
    UnloadTexture(ratText);
    UnloadTexture(catText);
    UnloadTexture(dialogBoxTex);
    UnloadTexture(potionTex);
    UnloadTexture(inventoryTex);
    
    CloseWindow();
    return 0;
}