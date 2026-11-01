#pragma once
#include "raylib.h"
#include "dialog-system.hpp"
#include <vector>
#include <string>

class Interactable
{
public:
    Rectangle interactArea; // Replaces the Entity reference and radius
    std::vector<std::string> dialogLines;
    bool canInteract;
    Font theFont;

    Interactable(Rectangle area, std::vector<std::string> lines, Font font)
        : interactArea(area), dialogLines(lines), canInteract(false), theFont(font) {}

    // Update now accepts a Rectangle for the player instead of an Entity object
    void Update(Rectangle playerRect, DialogSystem& dialogSystem)
    {
        // Disable interaction checking if a dialog is already active[cite: 4]
        if (dialogSystem.isActive)
        {
            canInteract = false;
            return;
        }

        // Flag true if the player's rectangle overlaps the interaction rectangle
        canInteract = CheckCollisionRecs(playerRect, interactArea);

        // Trigger dialog if in range and ENTER or SPACE is pressed[cite: 4]
        if (canInteract && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)))
        {
            dialogSystem.StartDialog(dialogLines);
        }
    }

    void DrawPrompt()
    {
        // Only draw the prompt if the player is close enough[cite: 4]
        if (canInteract)
        {
            // Draw text slightly above the interaction area's top-left corner
            DrawTextEx(theFont, "Interact", {interactArea.x, interactArea.y - 40.0f}, 15, 1.0f, BLACK);
        }
    }
};