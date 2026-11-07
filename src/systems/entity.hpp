#pragma once

#include <raylib.h>
#include <src/systems/components.hpp>
#include <src/systems/health.hpp>

class Entity
{
public:
    unsigned int id = 0;
    Faction faction = Faction::Neutral;
    bool active = true;

    TransformComponent transform;
    RenderComponent render;
    MovementComponent movement;
    PlayerControlComponent playerControl;
    BossControlComponent bossControl;
    CollisionComponent collision;
    HealthComponent health;
    ProjectileComponent projectile;

    Rectangle GetBounds() const
    {
        const Vector2 boundsSize = collision.enabled ? collision.size : transform.size;
        return {transform.position.x, transform.position.y, boundsSize.x, boundsSize.y};
    }
};
