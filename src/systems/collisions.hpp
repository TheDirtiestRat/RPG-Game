#pragma once

#include <raylib.h>
#include <algorithm>
#include <vector>
#include <src/systems/entity.hpp>

class CollisionSystem
{
public:
    static void Update(std::vector<Entity> &entities, float delta)
    {
        for (Entity &entity : entities)
        {
            if (entity.bossControl.enabled)
            {
                entity.bossControl.contactCooldown =
                    std::max(0.0f, entity.bossControl.contactCooldown - delta);
            }
        }

        for (Entity &projectile : entities)
        {
            if (!projectile.active || !projectile.projectile.enabled || !projectile.collision.enabled)
            {
                continue;
            }

            for (Entity &target : entities)
            {
                if (!target.active || !target.collision.enabled ||
                    target.faction != projectile.projectile.targetFaction ||
                    target.health.Maximum() == 0)
                {
                    continue;
                }

                if (CheckCollisionRecs(projectile.GetBounds(), target.GetBounds()))
                {
                    target.health.TakeDamage(projectile.projectile.damage);
                    projectile.active = false;
                    break;
                }
            }
        }

        Entity *player = nullptr;
        for (Entity &entity : entities)
        {
            if (entity.active && entity.faction == Faction::Player && entity.collision.enabled)
            {
                player = &entity;
                break;
            }
        }

        if (player == nullptr)
        {
            return;
        }

        for (Entity &boss : entities)
        {
            if (!boss.active || !boss.bossControl.enabled || !boss.collision.enabled ||
                boss.bossControl.contactCooldown > 0.0f)
            {
                continue;
            }

            if (CheckCollisionRecs(player->GetBounds(), boss.GetBounds()))
            {
                player->health.TakeDamage(boss.bossControl.contactDamage);
                boss.bossControl.contactCooldown = boss.bossControl.contactInterval;
            }
        }
    }
};
