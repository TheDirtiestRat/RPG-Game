#pragma once

#include <raylib.h>

enum class Faction
{
    Neutral,
    Player,
    Enemy
};

struct TransformComponent
{
    Vector2 position{};
    Vector2 size{};
};

struct RenderComponent
{
    Texture2D texture{};
    Rectangle source{};
    Color tint = WHITE;
    bool enabled = false;
};

struct MovementComponent
{
    Vector2 direction{};
    float speed = 0.0f;
    bool enabled = false;
};

struct PlayerControlComponent
{
    Vector2 aimDirection{0.0f, -1.0f};
    bool enabled = false;
};

struct BossControlComponent
{
    float preferredDistance = 140.0f;
    float contactCooldown = 0.0f;
    float contactInterval = 1.0f;
    int contactDamage = 10;
    bool enabled = false;
};

struct CollisionComponent
{
    Vector2 size{};
    bool enabled = false;
};

struct ProjectileComponent
{
    Vector2 direction{};
    float speed = 400.0f;
    int damage = 20;
    Faction targetFaction = Faction::Enemy;
    bool enabled = false;
};
