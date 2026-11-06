
#pragma once

#include <raylib.h>
#include "entity.hpp"

class Projectile
{
public:
    Texture2D texture;
    Rectangle sourceRec;

    Vector2 position;
    Vector2 direction;

    float speed;

    int currentFrame;
    float frameTimer;
    float frameTime = 0.08f;
    int totalFrames = 3;

    bool active;

    Projectile(
        Texture2D _texture,
        Vector2 _position,
        FacingDirection facing,
        float _speed = 400.0f)
    {
        texture = _texture;
        // Each frame is 80 x 80
        sourceRec = {0, 0, (float)(texture.width / totalFrames), (float)texture.height};
        
        speed = _speed;

        currentFrame = 0;
        frameTimer = 0.0f;

        active = true;

        int spwn_offset = 50;

        // Determine throwing direction
        switch (facing)
        {
        case UP:
            direction = {0, -1};
            position = {
            _position.x + sourceRec.width / 2,
            _position.y + sourceRec.height / 2 - spwn_offset};
            break;

        case DOWN:
            direction = {0, 1};
            position = {
            _position.x + sourceRec.width / 2,
            _position.y + sourceRec.height / 2 + spwn_offset};
            break;

        case LEFT:
            direction = {-1, 0};
            position = {
            _position.x + sourceRec.width / 2 - spwn_offset,
            _position.y + sourceRec.height / 2};
            break;

        case RIGHT:
            direction = {1, 0};
            position = {
            _position.x + sourceRec.width / 2 + spwn_offset,
            _position.y + sourceRec.height / 2};
            break;
        }
    }

    void Update(float delta)
    {
        if (!active)
            return;

        // Move projectile
        position.x += direction.x * speed * delta;
        position.y += direction.y * speed * delta;

        // Animate axe
        frameTimer += delta;

        if (frameTimer >= frameTime)
        {
            frameTimer -= frameTime;

            currentFrame++;

            if (currentFrame >= 3)
                currentFrame = 0;

            sourceRec.x = currentFrame * sourceRec.width;
        }

        // Remove when outside screen
        if (position.x < -80 ||
            position.x > GetScreenWidth() + 80 ||
            position.y < -80 ||
            position.y > GetScreenHeight() + 80)
        {
            active = false;
        }
    }

    Rectangle GetCollisionRect()
    {
        return {
            position.x, position.y, sourceRec.width, sourceRec.height
        };
    }

    void Draw()
    {
        if (!active)
            return;

        //for debug
        // DrawRectangle(position.x, position.y, sourceRec.width, sourceRec.height, RED);
        //display the projectile
        DrawTextureRec(texture, sourceRec, position, WHITE);
    }

    
};
