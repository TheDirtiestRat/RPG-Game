#pragma once

#include <algorithm>
#include <vector>
#include <src/systems/collisions.hpp>
#include <src/systems/entity_factory.hpp>
#include <src/systems/movement_system.hpp>
#include <src/systems/player-controller.hpp>
#include <src/systems/projectile.hpp>
#include <src/systems/render_system.hpp>

class TestScene
{
private:
    Texture2D playerTexture{};
    Texture2D bossTexture{};
    Texture2D projectileTexture{};
    std::vector<Entity> entities;
    unsigned int nextEntityId = 1;
    unsigned int playerId = 0;

public:
    bool Load()
    {
        playerTexture = LoadTexture("src/images/Cat RPG.png");
        bossTexture = LoadTexture("src/images/Cat RPG.png");
        projectileTexture = LoadTexture("src/images/Item_Potion.png");

        if (playerTexture.id == 0 || bossTexture.id == 0 || projectileTexture.id == 0)
        {
            TraceLog(LOG_ERROR, "Failed to load one or more scene textures");
            Unload();
            return false;
        }

        entities.clear();
        entities.reserve(32);
        nextEntityId = 1;
        playerId = nextEntityId++;
        entities.push_back(CreatePlayer(
            playerId,
            {80.0f, (GetScreenHeight() - static_cast<float>(playerTexture.height)) * 0.5f},
            playerTexture));
        entities.push_back(CreateBoss(
            nextEntityId++,
            {GetScreenWidth() - static_cast<float>(bossTexture.width) - 80.0f,
             (GetScreenHeight() - static_cast<float>(bossTexture.height)) * 0.5f},
            bossTexture));
        return true;
    }

    void Unload()
    {
        entities.clear();
        if (playerTexture.id != 0) UnloadTexture(playerTexture);
        if (bossTexture.id != 0) UnloadTexture(bossTexture);
        if (projectileTexture.id != 0) UnloadTexture(projectileTexture);
        playerTexture = {};
        bossTexture = {};
        projectileTexture = {};
    }

    void Update(float _delta)
    {
        Entity *player = FindEntity(playerId);
        if (player != nullptr)
        {
            Entity projectile;
            if (PlayerController::Update(*player, nextEntityId, projectileTexture, projectile))
            {
                ++nextEntityId;
                entities.push_back(projectile);
            }
        }

        MovementSystem::Update(entities, _delta);
        ProjectileSystem::Update(entities);
        CollisionSystem::Update(entities, _delta);
        RemoveInactiveProjectiles();

        for (Entity &entity : entities)
        {
            if (entity.health.IsDead() && entity.health.Maximum() > 0)
            {
                entity.active = false;
            }
        }
    }

    void Draw()
    {
        RenderSystem::Draw(entities);

        const Entity *player = FindEntity(playerId);
        const Entity *boss = nullptr;
        for (const Entity &entity : entities)
        {
            if (entity.faction == Faction::Enemy)
            {
                boss = &entity;
                break;
            }
        }

        if (player != nullptr)
        {
            DrawText(TextFormat("Player HP: %d / %d", player->health.Current(), player->health.Maximum()), 16, 16, 20, DARKBLUE);
        }
        if (boss != nullptr)
        {
            DrawText(TextFormat("Boss HP: %d / %d", boss->health.Current(), boss->health.Maximum()), 16, 42, 20, MAROON);
        }
    }

private:
    Entity *FindEntity(unsigned int id)
    {
        for (Entity &entity : entities)
        {
            if (entity.id == id) return &entity;
        }
        return nullptr;
    }

    const Entity *FindEntity(unsigned int id) const
    {
        for (const Entity &entity : entities)
        {
            if (entity.id == id) return &entity;
        }
        return nullptr;
    }

    void RemoveInactiveProjectiles()
    {
        entities.erase(
            std::remove_if(entities.begin(), entities.end(), [](const Entity &entity) {
                return entity.projectile.enabled && !entity.active;
            }),
            entities.end());
    }
};
