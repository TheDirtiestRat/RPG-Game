# Willowmere: A Tiny Adventure RPG

A small 2D adventure RPG prototype built with C++ and Raylib. Explore connected areas, talk to characters, collect items, and fight turn-based battles on the way to the final guardian.

## Build and run

The VS Code build tasks expect Raylib and the w64devkit toolchain under `C:\raylib`. Open the project workspace and run **Terminal → Run Build Task → build debug** (or **build release**), then launch the generated game executable.

## Controls

- **WASD** or **Arrow keys** — Move
- **E** — Interact with a nearby character, item, chest, or path
- **I** — Open or close inventory
- **Enter / Space** — Advance dialogue; use the selected inventory item
- **Up / Down**, then **Enter** — Choose a battle action
- **1 / 2 / 3** — Attack, use a potion, or try to flee
- **Esc** — Close dialogue or inventory
- **R** — Restart after victory or defeat

## Adventure

Speak with Elder Rowan in Willowmere, then follow the clues through the Whispering Woods and Sunken Ruins. Find the Rusty Key to open the ancient gate and sealed chest; the chest contains the Sun Sigil, which opens the path to the final guardian. Healing Potions restore health, including during battle.

Player movement is restricted to exploration. Dialogue, inventory, and battle each have their own input and display, and defeated enemies stay defeated until a new adventure begins.

## Asset pack

The existing `src/assets/` pack contains character, monster, and boss sprites; field and dungeon tilesets; potion, key, gem, and chest art; dialogue and wood-panel UI; a TrueType font; and music, sound effects, and jingles. It also includes additional animated backgrounds, effects, and UI icons that this prototype does not need.

The game uses the Boy and Villager/Hunter sprite sheets for characters, the Slime and Demon Cyclop idle sprites for enemies, the Field/Dungeon tilesets for area floors, the supplied item sprites for pickups and inventory, `DialogueBoxSimple.png` and the wood nine-patch for panels, and `NormalFont.ttf` for UI text. Exploration and battle use the included Adventure Begin and Fight music, with menu, combat, healing, victory, and defeat effects from the existing audio folders.

Assets are loaded centrally at startup from paths relative to the project root and unloaded when the game closes. Missing files are reported in the Raylib log; affected visuals fall back to simple drawing and missing audio or fonts are skipped.
