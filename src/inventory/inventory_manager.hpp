#pragma once
#include "raylib.h"
#include "item.hpp"
#include "src/entity.hpp"
#include <vector>

class Inventory {
public:
    std::vector<Item> items;
    bool isOpen;
    int selectedIndex;
    Font uiFont;
    Texture2D bgTexture;

    Inventory(Font font) : isOpen(false), selectedIndex(0), uiFont(font) {}

    void AddItem(Item newItem) {
        // Check if we already have this item to stack it
        for (auto& item : items) {
            if (item.name == newItem.name) {
                item.quantity += newItem.quantity;
                return;
            }
        }
        // If not found, add as a new slot
        items.push_back(newItem);
    }

    void Update(Entity& player) {
        // Toggle inventory with 'I' or 'TAB'
        if (IsKeyPressed(KEY_I) || IsKeyPressed(KEY_TAB)) {
            isOpen = !isOpen;
        }

        if (!isOpen || items.empty()) return;

        // UI Navigation (Assumes 5 columns as defined in Draw)
        int cols = 5;
        
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            selectedIndex++;
            if (selectedIndex >= (int)items.size()) selectedIndex = 0;
        }
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            selectedIndex--;
            if (selectedIndex < 0) selectedIndex = items.size() - 1;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            if (selectedIndex + cols < (int)items.size()) selectedIndex += cols;
            else selectedIndex = items.size() - 1; // Snap to very last item
        }
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            if (selectedIndex - cols >= 0) selectedIndex -= cols;
            else selectedIndex = 0; // Snap to first item
        }
    }

    void UseItem(int index, Entity& player) {
        Item& item = items[index];

        // Apply item effects
        if (item.type == ItemType::HEAL) {
            // player.health += item.power; // Assuming you have a health variable
        }

        // Consume the item
        item.quantity--;
        if (item.quantity <= 0) {
            items.erase(items.begin() + index);
            if (selectedIndex >= (int)items.size() && selectedIndex > 0) {
                selectedIndex--;
            }
        }
    }

    void Draw() {
        if (!isOpen) return;

        int fontSize = 20;
        // int screenW = GetScreenWidth();
        // int screenH = GetScreenHeight();
        
        // 1. Center the panel based on your custom texture's actual size
        Rectangle panel = { 
            50.0f, 
            50.0f, 
            (float)bgTexture.width, 
            (float)bgTexture.height 
        };
        
        // Draw the background texture instead of a colored rectangle
        DrawTextureV(bgTexture, { panel.x, panel.y }, WHITE);
        
        // Adjust these offsets (+130, +10) if your texture is a different size than 400x300
        DrawTextEx(uiFont, "INVENTORY", { panel.x + 25, panel.y + 25 }, fontSize, 1.0f, BLACK);

        if (items.empty()) {
            DrawTextEx(uiFont, "Empty", { panel.x + (panel.width/2), panel.y + (panel.height/2)}, fontSize, 1.0f, BLACK);
            return;
        }

        // 2. Grid Configuration
        int columns = 5;
        float slotSize = 48.0f;
        float spacing = 12.0f;
        float startX = panel.x + 55.0f; 
        float startY = panel.y + 60.0f;

        // 3. Draw Grid Items
        for (int i = 0; i < (int)items.size(); i++) {
            int row = i / columns;
            int col = i % columns;

            float x = startX + (col * (slotSize + spacing));
            float y = startY + (row * (slotSize + spacing));

            Rectangle slotRect = { x, y, slotSize, slotSize };

            // Optional: You can also replace this with a 'slotTexture' if you have one!
            DrawRectangleRec(slotRect, Fade(BLACK, 0.5f));
            
            if (i == selectedIndex) {
                DrawRectangleLinesEx(slotRect, 3.0f, YELLOW);
            } else {
                DrawRectangleLinesEx(slotRect, 1.0f, GRAY);
            }

            Rectangle source = { 0, 0, (float)items[i].icon.width, (float)items[i].icon.height };
            Rectangle dest = { x + 8, y + 8, slotSize - 16, slotSize - 16 };
            DrawTexturePro(items[i].icon, source, dest, {0,0}, 0.0f, WHITE);

            if (items[i].quantity > 1) {
                std::string qtyText = std::to_string(items[i].quantity);
                DrawTextEx(uiFont, qtyText.c_str(), { x + slotSize - 15, y + slotSize - 18 }, 16, 1.0f, WHITE);
            }
        }

        // 4. Show the selected item's name at the bottom of the panel
        if (selectedIndex >= 0 && selectedIndex < (int)items.size()) {
            std::string selectedName = items[selectedIndex].name;
            DrawTextEx(uiFont, selectedName.c_str(), { panel.x + 20, panel.y + panel.height - 35 }, 20, 1.0f, YELLOW);
        }
    }
};