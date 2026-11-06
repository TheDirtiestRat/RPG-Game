
#pragma once

#include <raylib.h>

class Projectile
{
public:
    Texture2D texture;
    Rectangle sourceRec;

    Vector2 position;
    Vector2 direction;

    float size;
    float speed;

    bool active;
    bool debug;

    Projectile(
        Texture2D _texture,
        Vector2 _position,
        Vector2 _direction,
        float _speed = 400.0f,
        float _size = 80.0f,
        bool _isDebug = false)
    {
        texture = _texture;
        sourceRec = {0, 0, _size, _size};
        speed = _speed;
        size = _size;

        active = true;
        debug = _isDebug;

        direction = _direction;
        position = _position;
    }

    void Update(float delta)
    {
        if (!active)
            return;

        // Move projectile
        position.x += direction.x * speed * delta;
        position.y += direction.y * speed * delta;

        // Remove when outside screen
        if (position.x < -size ||
            position.x > GetScreenWidth() + size ||
            position.y < -size ||
            position.y > GetScreenHeight() + size)
        {
            active = false;
        }
    }

    void Draw()
    {
        if (!active)
            return;
        
        if (debug) {
            DrawRectangle(position.x, position.y, sourceRec.width, sourceRec.height, RED);
        }

        DrawTextureRec(texture, sourceRec, position, WHITE);
    }

    Rectangle GetCollisionRect()
    {
        return {
            position.x, position.y, sourceRec.width, sourceRec.height};
    }
};
