#pragma once

#include <src/scenes/test_scene.hpp>

class Game
{
private:
    TestScene testScene;

public:
    bool Load()
    {
        return testScene.Load();
    }

    void Unload()
    {
        testScene.Unload();
    }

    void Update(float _delta)
    {
        testScene.Update(_delta);
    }

    void Draw()
    {
        testScene.Draw();
    }
};
