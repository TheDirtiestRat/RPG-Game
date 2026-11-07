#pragma once

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <src/systems/entity_factory.hpp>

class PlayerController
{
public:
    static bool Update(Entity &player, unsigned int projectileId, Texture2D projectileTexture, Entity &projectile)
    {
        if (!player.active || !player.playerControl.enabled)
        {
            return false;
        }

        Vector2 direction{0.0f, 0.0f};
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) direction.y -= 1.0f;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) direction.y += 1.0f;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) direction.x -= 1.0f;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) direction.x += 1.0f;

        if (direction.x != 0.0f || direction.y != 0.0f)
        {
            player.movement.direction = Vector2Normalize(direction);
            player.playerControl.aimDirection = player.movement.direction;
        }
        else
        {
            player.movement.direction = {0.0f, 0.0f};
        }

        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER))
        {
            const Vector2 aim = player.playerControl.aimDirection;
            const Vector2 spawnPosition{
                player.transform.position.x + (player.transform.size.x - 32.0f) * 0.5f + aim.x * 24.0f,
                player.transform.position.y + (player.transform.size.y - 32.0f) * 0.5f + aim.y * 24.0f};
            projectile = CreateProjectile(projectileId, spawnPosition, aim, projectileTexture, player.faction);
            return true;
        }

        return false;
    }
};