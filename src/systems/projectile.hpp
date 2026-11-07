
#pragma once

#include <raylib.h>
#include <vector>
#include <src/systems/entity.hpp>

class ProjectileSystem
{
public:
    static void Update(std::vector<Entity> &entities)
    {
        for (Entity &entity : entities)
        {
            if (!entity.active || !entity.projectile.enabled)
            {
                continue;
            }

            const Vector2 position = entity.transform.position;
            const Vector2 size = entity.transform.size;
            if (position.x + size.x < 0.0f || position.x > GetScreenWidth() ||
                position.y + size.y < 0.0f || position.y > GetScreenHeight())
            {
                entity.active = false;
            }
        }
    }
};
