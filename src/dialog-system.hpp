#pragma once

#include "raylib.h"
#include <vector>
#include <string>
#include <sstream>

class DialogSystem
{
public:
    std::vector<std::string> lines;
    int currentLine;
    bool isActive;
    Font dialogFont;

    Texture2D textureBox;

    DialogSystem(Texture2D _textureB)
    {
        // lines = newLines;
        // currentLine = 0;
        // isActive = true;
        textureBox = _textureB;
    }

    void StartDialog(std::vector<std::string> newLines)
    {
        lines.clear(); // Clear old dialog
        currentLine = 0;
        isActive = true;

        int screenWidth = GetScreenWidth();
        float maxWidth = (float)screenWidth - 140.0f; // Box width (screenWidth - 100) minus 40 padding
        float fontSize = 20.0f; // Note: DrawTextEx uses a float for size
        float spacing = 1.0f;
        int maxLinesPerPage = 3; // Maximum lines that fit vertically in your 100px box

        // Pre-process the text to calculate wraps and pages
        for (const std::string& rawText : newLines)
        {
            std::istringstream stream(rawText);
            std::string word;
            std::string drawLine = "";
            std::string currentPage = "";
            int lineCount = 0;

            while (stream >> word)
            {
                std::string testLine = drawLine.empty() ? word : drawLine + " " + word;

                if (MeasureTextEx(dialogFont, testLine.c_str(), fontSize, spacing).x > maxWidth)
                {
                    // Add the completed line to the current page
                    currentPage += (currentPage.empty() ? "" : "\n") + drawLine;
                    lineCount++;
                    drawLine = word; // Move the overflowing word to the next line

                    // If the page hits the vertical limit, save it and start a new one
                    if (lineCount >= maxLinesPerPage)
                    {
                        lines.push_back(currentPage);
                        currentPage = "";
                        lineCount = 0;
                    }
                }
                else
                {
                    drawLine = testLine;
                }
            }

            // Append any leftover text in the stream
            if (!drawLine.empty())
            {
                currentPage += (currentPage.empty() ? "" : "\n") + drawLine;
            }
            if (!currentPage.empty())
            {
                lines.push_back(currentPage);
            }
        }
    }

    void Update()
    {
        if (!isActive)
            return;

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        {
            currentLine++;

            if (currentLine >= (int)lines.size())
            {
                isActive = false;
            }
        }
    }

    void Draw()
    {
        if (!isActive || lines.empty())
            return;

        // int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        // 1. Define the actual screen position for the top-left of the dialog box
        float screenPosX = 50.0f;
        float screenPosY = (float)screenHeight - 200.0f; // Matches your old Y position

        // 2. Draw the entire texture at that position
        // DrawTextureV is cleaner than DrawTextureRec when you aren't cropping the image
        DrawTextureV(textureBox, {screenPosX, screenPosY}, WHITE);

        // 3. Calculate text placement relative to the screen position
        float startX = screenPosX + 20.0f;
        float startY = screenPosY + 20.0f;
        int fontSize = 20;
        float spacing = 1.0f;

        // Draw the text
        DrawTextEx(dialogFont, lines[currentLine].c_str(), {startX, startY}, fontSize, spacing, BLACK);

        // 4. Position the indicator using the screen position and the texture's dimensions
        if ((int)(GetTime() * 2) % 2 == 0) // Toggles every half second
        {
            Vector2 indicatorPos = {
                screenPosX + textureBox.width - 30.0f, 
                screenPosY + textureBox.height - 30.0f
            };
            DrawTextEx(dialogFont, ">", indicatorPos, fontSize, spacing, BLACK);
        }
    }
};
