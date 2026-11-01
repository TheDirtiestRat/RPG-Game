#pragma once

#include <raylib.h>
#include <raymath.h> // Required for smooth Vector2 calculations
#include "entity.hpp"

class PlayerController{
public:
    Entity& player;
    Vector2 direction; // This now acts as your current momentum/velocity

    // Tweak these to change how "heavy" or "slippery" the player feels
    float acceleration = 25.0f; // High value for snappy starts
    float friction = 15.0f;     // Lower value for smooth sliding stops

    PlayerController(Entity& entity) : player(entity) {
        direction = {0.0f, 0.0f};
    }

    void HandleInput(float delta) {
        Vector2 input = {0.0f, 0.0f};

        // 1. Gather Input (Independent 'if' statements allow diagonal movement!)
        if (IsKeyDown(KEY_W)) input.y -= 1.0f;
        if (IsKeyDown(KEY_S)) input.y += 1.0f;
        if (IsKeyDown(KEY_A)) input.x -= 1.0f;
        if (IsKeyDown(KEY_D)) input.x += 1.0f;

        // Normalize input so moving diagonally isn't faster than moving straight
        if (Vector2Length(input) > 0.0f) {
            input = Vector2Normalize(input);
        }

        // 2. Calculate the target velocity based on input and player speed
        Vector2 targetVelocity = { input.x * player.speed, input.y * player.speed };

        // 3. Apply Snappy Acceleration and Smooth Friction
        if (input.x != 0.0f || input.y != 0.0f) {
            // Accelerate quickly when moving
            direction = Vector2Lerp(direction, targetVelocity, acceleration * delta);
        } else {
            // Slide to a smooth stop when keys are released
            direction = Vector2Lerp(direction, Vector2Zero(), friction * delta);
            
            // Hard stop if the movement is microscopic to prevent endless floating
            if (Vector2LengthSqr(direction) < 1.0f) {
                direction = {0.0f, 0.0f};
            }
        }

        // 4. Update Animations (Based on input, so turning is instant even while sliding)
        if (input.x < 0) {
            player.PlayAnimation(3, 5, delta);
            player.sourceRec.width = -player.frameWidth; // Face left
        } else if (input.x > 0) {
            player.PlayAnimation(3, 5, delta);
            player.sourceRec.width = player.frameWidth;  // Face right
        } else if (input.y < 0) {
            player.PlayAnimation(0, 2, delta);
        } else if (input.y > 0) {
            player.PlayAnimation(6, 8, delta);
        }

        // 5. Apply the final momentum to the entity
        player.Movement(delta, direction);
    }

    void Update(float delta)
    {
        HandleInput(delta);
    }
};