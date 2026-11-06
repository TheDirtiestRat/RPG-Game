#pragma once

#include <raylib.h>

class Entity
{
private:
    Vector2 position;
    Texture2D texture;
    Rectangle sourceRec;

    bool isDebug = false;
public:
    Entity(Texture2D _texture, bool _debug = false) {
        isDebug = _debug;
        texture = _texture;
        sourceRec = {0,0,texture.width,texture.height};
    };
    ~Entity();

    void UpdatePosition(Vector2 _dir, float delta) {
        position.x += _dir.x * delta;
        position.y += _dir.y * delta; 
    }

    void Draw() {
        if (isDebug) {
            DrawRectangleRec(sourceRec, GREEN);
        }

        DrawTextureRec(texture, sourceRec, position, WHITE);
    }
};

Entity::~Entity()
{
}
