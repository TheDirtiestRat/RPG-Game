#pragma once
#include "raylib.h"
#include <string>

enum class ItemType {
    HEAL,
    MANA,
    KEY_ITEM
};

struct Item {
    std::string name;
    ItemType type;
    int power; // How much it heals, or a specific ID for keys
    int quantity;

    // We don't load the Texture2D here directly to avoid loading the same 
    // image from the hard drive 50 times. Pass a pre-loaded texture in.
    Texture2D icon; 
};