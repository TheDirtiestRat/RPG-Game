#pragma once

#include <raylib.h>
#include <src/systems/entity.hpp>

inline Entity CreatePlayer(unsigned int id, Vector2 position, Texture2D texture)
{
    Entity player;
    player.id = id;
    player.faction = Faction::Player;
    player.transform.position = position;
    player.transform.size = {static_cast<float>(texture.width), static_cast<float>(texture.height)};
    player.render = {texture, {0.0f, 0.0f, static_cast<float>(texture.width), static_cast<float>(texture.height)}, WHITE, true};
    player.movement = {{0.0f, 0.0f}, 200.0f, true};
    player.playerControl.enabled = true;
    player.collision = {player.transform.size, true};
    player.health = HealthComponent(100);
    return player;
}

inline Entity CreateBoss(unsigned int id, Vector2 position, Texture2D texture)
{
    Entity boss;
    boss.id = id;
    boss.faction = Faction::Enemy;
    boss.transform.position = position;
    boss.transform.size = {static_cast<float>(texture.width), static_cast<float>(texture.height)};
    boss.render = {texture, {0.0f, 0.0f, static_cast<float>(texture.width), static_cast<float>(texture.height)}, WHITE, true};
    boss.movement = {{0.0f, 0.0f}, 55.0f, true};
    boss.bossControl.enabled = true;
    boss.collision = {boss.transform.size, true};
    boss.health = HealthComponent(250);
    return boss;
}

inline Entity CreateProjectile(
    unsigned int id,
    Vector2 position,
    Vector2 direction,
    Texture2D texture,
    Faction owner)
{
    Entity projectile;
    projectile.id = id;
    projectile.faction = owner;
    projectile.transform.position = position;
    projectile.transform.size = {32.0f, 32.0f};
    projectile.render = {
        texture,
        {0.0f, 0.0f, static_cast<float>(texture.width), static_cast<float>(texture.height)},
        WHITE,
        true};
    projectile.movement = {direction, 400.0f, true};
    projectile.collision = {{24.0f, 24.0f}, true};
    projectile.projectile = {
        direction,
        400.0f,
        25,
        owner == Faction::Player ? Faction::Enemy : Faction::Player,
        true};
    return projectile;
}
