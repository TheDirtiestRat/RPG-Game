#pragma once

#include <raylib.h>
#include <vector>
#include <src/systems/entity.hpp>

class RenderSystem
{
public:
    static void Draw(const std::vector<Entity> &entities)
    {
        for (const Entity &entity : entities)
        {
            if (!entity.active || !entity.render.enabled)
            {
                continue;
            }

            const Rectangle destination{
                entity.transform.position.x,
                entity.transform.position.y,
                entity.transform.size.x,
                entity.transform.size.y};
            DrawTexturePro(
                entity.render.texture,
                entity.render.source,
                destination,
                {0.0f, 0.0f},
                0.0f,
                entity.render.tint);
        }
    }
};
