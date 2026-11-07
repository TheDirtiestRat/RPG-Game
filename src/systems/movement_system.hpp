#pragma once

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <src/systems/entity.hpp>

class MovementSystem
{
public:
    static void Update(std::vector<Entity> &entities, float delta)
    {
        const Entity *player = nullptr;
        for (const Entity &entity : entities)
        {
            if (entity.active && entity.faction == Faction::Player)
            {
                player = &entity;
                break;
            }
        }

        for (Entity &entity : entities)
        {
            if (!entity.active || !entity.movement.enabled)
            {
                continue;
            }

            if (entity.bossControl.enabled && player != nullptr)
            {
                const Vector2 offset{
                    player->transform.position.x - entity.transform.position.x,
                    player->transform.position.y - entity.transform.position.y};
                const float distance = Vector2Length(offset);
                entity.movement.direction = distance > entity.bossControl.preferredDistance
                    ? Vector2Scale(Vector2Normalize(offset), 1.0f)
                    : Vector2{0.0f, 0.0f};
            }

            entity.transform.position.x += entity.movement.direction.x * entity.movement.speed * delta;
            entity.transform.position.y += entity.movement.direction.y * entity.movement.speed * delta;

            if (entity.playerControl.enabled)
            {
                entity.transform.position.x = Clamp(
                    entity.transform.position.x, 0.0f, static_cast<float>(GetScreenWidth()) - entity.transform.size.x);
                entity.transform.position.y = Clamp(
                    entity.transform.position.y, 0.0f, static_cast<float>(GetScreenHeight()) - entity.transform.size.y);
            }
        }
    }
};
