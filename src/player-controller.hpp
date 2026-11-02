#pragma once

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <utility>
#include "entity.hpp"

class PlayerController
{
public:
    Entity &player;

    Vector2 direction;

    float acceleration = 25.0f;
    float friction = 15.0f;

    // Animation frames: {row, column}

    std::vector<std::pair<int, int>> idleDown = {
        {1, 0}};

    std::vector<std::pair<int, int>> idleUp = {
        {1, 2}};


    std::vector<std::pair<int, int>> idleRight = {
        {1, 1}};

    std::vector<std::pair<int, int>> walkDown = {
        {0, 0}, {1, 0}, {2, 0},{3,0}};

    std::vector<std::pair<int, int>> walkUp = {
        {0, 2}, {1, 2}, {2, 2},{3,2}};


    std::vector<std::pair<int, int>> walkRight = {
        {0, 1}, {1, 1}, {2, 1},{3,1}};

    // Replace these example frames with your actual attack frames.
    std::vector<std::pair<int, int>> attackDown = {
        {0, 3}, {1, 3}, {0, 3}};

    std::vector<std::pair<int, int>> attackUp = {
        {0, 5}, {1, 5}, {0, 5}};

    std::vector<std::pair<int, int>> attackRight = {
        {0, 4}, {1, 4}, {0, 4}};

    PlayerController(Entity &entity) : player(entity)
    {
        direction = {0.0f, 0.0f};
    }

    void Update(float delta)
    {

        // 1. ATTACKING STATE
        if (player.currentState == ATTACKING)
        {

            bool finished = false;

            switch (player.facing)
            {
            case UP:
                finished = player.PlayAnimation(attackUp, delta, false);
                break;

            case DOWN:
                finished = player.PlayAnimation(attackDown, delta, false);
                break;

            case LEFT:
                finished = player.PlayAnimation(attackRight, delta, false);
                break;

            case RIGHT:
                finished = player.PlayAnimation(attackRight, delta, false);
                break;
            }

            // Stop movement while attacking
            direction = {0.0f, 0.0f};

            if (finished)
            {
                player.currentState = IDLE;
            }

            return;
        }

        // 2. CHECK ATTACK INPUT
        if (IsKeyPressed(KEY_SPACE))
        {
            player.currentState = ATTACKING;
            direction = {0.0f, 0.0f};
            return;
        }

        // 3. GET MOVEMENT INPUT
        Vector2 input = {0.0f, 0.0f};

        if (IsKeyDown(KEY_W))
            input.y -= 1.0f;
        if (IsKeyDown(KEY_S))
            input.y += 1.0f;
        if (IsKeyDown(KEY_A))
            input.x -= 1.0f;
        if (IsKeyDown(KEY_D))
            input.x += 1.0f;

        if (Vector2Length(input) > 0.0f)
        {
            input = Vector2Normalize(input);
        }

        // 4. UPDATE FACING DIRECTION
        if (input.x < 0)
        {
            player.facing = LEFT;
        }
        else if (input.x > 0)
        {
            player.facing = RIGHT;
        }
        else if (input.y < 0)
        {
            player.facing = UP;
        }
        else if (input.y > 0)
        {
            player.facing = DOWN;
        }

        // 5. MOVEMENT AND ANIMATION
        if (Vector2Length(input) > 0.0f)
        {

            player.currentState = MOVING;

            Vector2 targetVelocity = {
                input.x * player.speed,
                input.y * player.speed};

            direction = Vector2Lerp(
                direction,
                targetVelocity,
                acceleration * delta);
        }
        else
        {
            player.currentState = IDLE;

            direction = Vector2Lerp(
                direction,
                Vector2Zero(),
                friction * delta);

            if (Vector2LengthSqr(direction) < 1.0f)
            {
                direction = {0.0f, 0.0f};
            }
        }

        // 6. PLAY THE CORRECT ANIMATION
        if (player.currentState == MOVING)
        {

            switch (player.facing)
            {
            case UP:
                player.PlayAnimation(walkUp, delta);
                break;

            case DOWN:
                player.PlayAnimation(walkDown, delta);
                break;

            case LEFT:
                player.PlayAnimation(walkRight, delta);
                break;

            case RIGHT:
                player.PlayAnimation(walkRight, delta);
                break;
            }
        }
        else if (player.currentState == IDLE)
        {

            switch (player.facing)
            {
            case UP:
                player.PlayAnimation(idleUp, delta);
                break;

            case DOWN:
                player.PlayAnimation(idleDown, delta);
                break;

            case LEFT:
                player.PlayAnimation(idleRight, delta);
                break;

            case RIGHT:
                player.PlayAnimation(idleRight, delta);
                break;
            }
        }

        // 7. APPLY MOVEMENT
        player.Movement(delta, direction);
    }
};
