#pragma once
#include "raylib.h"
#include "inventory_manager.hpp"

class ItemPickup {
public:
    Rectangle collisionBox;
    Item storedItem;
    bool isCollected;

    ItemPickup(Rectangle area, Item itemToGive) 
        : collisionBox(area), storedItem(itemToGive), isCollected(false) {}

    void Update(Rectangle playerRect, Inventory& playerInventory) {
        if (isCollected) return;

        // If the player walks over the item, pick it up automatically
        if (CheckCollisionRecs(playerRect, collisionBox)) {
            playerInventory.AddItem(storedItem);
            isCollected = true; 
        }
    }

    void Draw() {
        if (isCollected) return;
        
        // Draw the item sprite on the ground
        DrawTexture(storedItem.icon, collisionBox.x, collisionBox.y, WHITE);
    }
};