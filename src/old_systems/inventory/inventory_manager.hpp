#pragma once
#include "raylib.h"
#include "item.hpp"
#include "src/entity.hpp"
#include <vector>

class Inventory
{
public:
    std::vector<Item> items;
    bool isOpen;
    int selectedIndex;
    Font uiFont;
    Texture2D bgTexture;

    Inventory(Font font) : isOpen(false), selectedIndex(0), uiFont(font) {}

    void AddItem(Item newItem)
    {
        // Check if we already have this item to stack it
        for (auto &item : items)
        {
            if (item.name == newItem.name)
            {
                item.quantity += newItem.quantity;
                return;
            }
        }
        // If not found, add as a new slot
        items.push_back(newItem);
    }

    void Update(Entity &player)
    {
        // Toggle inventory with 'I' or 'TAB'
        if (IsKeyPressed(KEY_I) || IsKeyPressed(KEY_TAB))
        {
            isOpen = !isOpen;
        }

        if (!isOpen || items.empty())
            return;

        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        {
            selectedIndex++;

            if (selectedIndex >= (int)items.size())
                selectedIndex = 0;
        }

        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        {
            selectedIndex--;

            if (selectedIndex < 0)
                selectedIndex = (int)items.size() - 1;
        }
    }

    void UseItem(int index, Entity &player)
    {
        Item &item = items[index];

        // Apply item effects
        if (item.type == ItemType::HEAL)
        {
            // player.health += item.power; // Assuming you have a health variable
        }

        // Consume the item
        item.quantity--;
        if (item.quantity <= 0)
        {
            items.erase(items.begin() + index);
            if (selectedIndex >= (int)items.size() && selectedIndex > 0)
            {
                selectedIndex--;
            }
        }
    }

    void Draw()
    {
        if (!isOpen)
            return;

        Rectangle panel = {
            50.0f,
            50.0f,
            (float)bgTexture.width,
            (float)bgTexture.height};

        DrawTextureV(bgTexture, {panel.x, panel.y}, WHITE);

        DrawTextEx(uiFont, "INVENTORY",
                   {panel.x + 25, panel.y + 25}, 20, 1, BLACK);

        if (items.empty())
        {
            DrawTextEx(uiFont, "Empty",
                       {panel.x + 25, panel.y + 70}, 20, 1, BLACK);
            return;
        }

        float startY = panel.y + 65;
        float rowHeight = 40;
        float spacing = 5;

        for (int i = 0; i < (int)items.size(); i++)
        {

            float x = panel.x + 25;
            float y = startY + i * (rowHeight + spacing);

            Rectangle row = {
                x, y, panel.width - 50, rowHeight};

            // Highlight selected item
            if (i == selectedIndex)
            {
                DrawRectangleRec(row, Fade(YELLOW, 0.4f));
            }

            // Draw item icon
            DrawTexturePro(
                items[i].icon,
                {0, 0, (float)items[i].icon.width,
                 (float)items[i].icon.height},
                {x + 5, y + 5, 30, 30},
                {0, 0}, 0, WHITE);

            // Draw item name
            DrawTextEx(uiFont, items[i].name.c_str(),
                       {x + 45, y + 10}, 18, 1, BLACK);

            // Draw quantity
            if (items[i].quantity > 1)
            {
                std::string quantity = "x" +
                                       std::to_string(items[i].quantity);

                DrawTextEx(uiFont, quantity.c_str(),
                           {x + row.width - 45, y + 10},
                           18, 1, BLACK);
            }
        }
    }
};