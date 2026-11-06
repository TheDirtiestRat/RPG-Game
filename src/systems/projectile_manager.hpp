#pragma once

#include <raylib.h>
#include <systems/projectile.hpp>
#include <vector>

class ProjectileManager
{
private:
    std::vector<Projectile> projectiles;

public:
    ProjectileManager();
    ~ProjectileManager();

    void ProjectileManager::AddProjectile(Texture2D _texture, Vector2 _pos, Vector2 _dir, float _speed)
    {
        projectiles.emplace_back(_texture, _pos, _dir, _speed);
    }

    void Update()
    {
        for (auto &projectile : projectiles)
            projectile.Update(GetFrameTime());
    }

    void Draw()
    {
        for (auto &projectile : projectiles)
            projectile.Draw();
    }
};

ProjectileManager::ProjectileManager()
{
}

ProjectileManager::~ProjectileManager()
{
}
