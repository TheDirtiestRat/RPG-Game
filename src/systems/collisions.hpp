#pragma once

#include <raylib.h>

class Collision{
    private:
    bool collided = false;
    Rectangle area_collision;
    Rectangle target_collision;

    public:
    Collision(Rectangle _area, Rectangle _target) {
        area_collision = _area;
        target_collision = _target;
    }
    ~Collision() {

    }

    void UpdatePosition(Vector2 _pos) {
        area_collision.x = _pos.x;
        area_collision.y = _pos.y;

        collided = CheckCollisionRecs(area_collision, target_collision);
    }

    void DrawArea() {
        DrawRectangleRec(area_collision, GREEN);
    }
};