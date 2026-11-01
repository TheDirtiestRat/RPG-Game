#pragma once
#include <raylib.h>

class Entity
{
public:
    float speed;
    Vector2 position;

    Texture2D texture;
    Rectangle sourceRec;
    Rectangle areaCollision;

    int numFrames;
    int currFrame;
    float frameTime;
    float frameTimer;

    float frameWidth;

    Entity(Texture2D _texture, Vector2 _position, float _speed, int frames)
    {
        speed = _speed;
        texture = _texture;

        numFrames = frames;
        currFrame = 0;
        // unsa ka paspas ang animation
        frameTime = 0.2f;
        frameTimer = 0.0f;

        frameWidth = (float)texture.width / numFrames;
        sourceRec = {0.0f, 0.0f, frameWidth, (float)texture.height};
        position = {(float)_position.x - (frameWidth / 2), (float)_position.y - (sourceRec.height / 2)};

        areaCollision = {position.x, position.y, frameWidth, sourceRec.height};
    }

    void PlayAnimation(int startFrame, int endFrame, float delta)
    {
        if (currFrame < startFrame || currFrame > endFrame)
        {
            currFrame = startFrame;
        }

        frameTimer += delta;
        if (frameTimer >= frameTime)
        {
            frameTimer = 0.0f;
            currFrame++;
            if (currFrame > endFrame)
                currFrame = startFrame;
        }

        sourceRec.x = currFrame * frameWidth;
    }

    void Movement(float delta, Vector2 direction)
    {
        position.x += direction.x * delta;
        position.y += direction.y * delta;
        
        Update();
    }

    void Update()
    {
        areaCollision = {position.x, position.y, frameWidth, sourceRec.height};
    }

    void Draw()
    {
        // for debuging
        //  DrawRectangleRec(areaCollision, GREEN);
        // draw the art
        DrawTextureRec(texture, sourceRec, position, WHITE);
    }
};