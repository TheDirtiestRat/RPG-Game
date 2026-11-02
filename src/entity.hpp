#pragma once
#include <raylib.h>
#include <vector>
#include <utility>

enum CharacterState
{
    IDLE,
    MOVING,
    ATTACKING
};

enum FacingDirection
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};
class Entity
{
public:
    float speed;
    Vector2 position;

    Texture2D texture;
    Rectangle sourceRec;
    Rectangle areaCollision;

    int atlasRows;
    int atlasColumns;

    float frameWidth;
    float frameHeight;

    int animationIndex;

    int numFrames;
    int currFrame;
    float frameTime;
    float frameTimer;

    CharacterState currentState = IDLE;
    FacingDirection facing = DOWN;

    std::vector<std::pair<int, int>> activeFrames;
    bool animationFinished = false;

    Entity(Texture2D _texture, Vector2 _position,
           float _speed, int columns, int rows)
    {

        texture = _texture;
        speed = _speed;

        atlasColumns = columns;
        atlasRows = rows;

        frameWidth = (float)texture.width / atlasColumns;
        frameHeight = (float)texture.height / atlasRows;

        currFrame = 0;
        frameTime = 0.2f;
        frameTimer = 0.0f;

        sourceRec = {0, 0, frameWidth, frameHeight};

        position = {
            _position.x - frameWidth / 2,
            _position.y - frameHeight / 2};

        areaCollision = {
            position.x, position.y,
            frameWidth, frameHeight};
    }

    bool PlayAnimation(
        const std::vector<std::pair<int, int>> &frames,
        float delta,
        bool loop = true)
    {
        if (frames.empty())
            return false;

        // Check if we're playing a different animation
        if (activeFrames != frames)
        {
            activeFrames = frames;
            animationIndex = 0;
            frameTimer = 0.0f;
            animationFinished = false;
        }

        // Advance animation
        if (!animationFinished)
        {
            frameTimer += delta;

            if (frameTimer >= frameTime)
            {
                frameTimer -= frameTime;

                if (animationIndex < (int)activeFrames.size() - 1)
                {
                    animationIndex++;
                }
                else if (loop)
                {
                    animationIndex = 0;
                }
                else
                {
                    animationFinished = true;
                }
            }
        }

        

        // Get the current frame's row and column
        int row = activeFrames[animationIndex].first;
        int column = activeFrames[animationIndex].second;

        // Set the sprite direction
        if (facing == LEFT)
        {
            sourceRec.width = -frameWidth;
            sourceRec.x = (column + 1) * frameWidth;
        }
        else
        {
            sourceRec.width = frameWidth;
            sourceRec.x = column * frameWidth;
        }

        sourceRec.y = row * frameHeight;    

        return animationFinished;
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