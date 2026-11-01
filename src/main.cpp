#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace
{
constexpr int ScreenWidth = 960;
constexpr int ScreenHeight = 720;
constexpr int FieldTop = 78;
constexpr int FieldBottom = 640;
constexpr float PlayerSpeed = 220.0f;
constexpr int MaxInventoryStack = 99;

enum class GameState
{
    Exploration,
    Dialogue,
    Inventory,
    Battle,
    GameOver,
    Victory
};

enum class EntityType
{
    Npc,
    Pickup,
    Chest,
    Enemy,
    Portal
};

enum class ItemId
{
    None,
    HealingPotion,
    RustyKey,
    SunSigil
};

struct ItemDefinition
{
    ItemId id;
    const char *name;
    const char *description;
};

const ItemDefinition &GetItemDefinition(ItemId id)
{
    static const ItemDefinition potion = {
        ItemId::HealingPotion, "Healing Potion", "Restores 18 HP. Usable in battle or exploration."};
    static const ItemDefinition key = {
        ItemId::RustyKey, "Rusty Key", "Opens the old gate and the chest in the ruins."};
    static const ItemDefinition sigil = {
        ItemId::SunSigil, "Sun Sigil", "The mark that opens the path to the final guardian."};
    static const ItemDefinition none = {ItemId::None, "Empty", ""};

    switch (id)
    {
    case ItemId::HealingPotion:
        return potion;
    case ItemId::RustyKey:
        return key;
    case ItemId::SunSigil:
        return sigil;
    default:
        return none;
    }
}

struct InventoryEntry
{
    ItemId item;
    int quantity;
};

class Inventory
{
public:
    void Add(ItemId item, int quantity = 1)
    {
        if (item == ItemId::None || quantity <= 0)
        {
            return;
        }

        for (InventoryEntry &entry : entries)
        {
            if (entry.item == item)
            {
                entry.quantity = std::min(MaxInventoryStack, entry.quantity + quantity);
                return;
            }
        }

        entries.push_back({item, std::min(MaxInventoryStack, quantity)});
    }

    bool Has(ItemId item) const
    {
        return Count(item) > 0;
    }

    int Count(ItemId item) const
    {
        for (const InventoryEntry &entry : entries)
        {
            if (entry.item == item)
            {
                return entry.quantity;
            }
        }
        return 0;
    }

    bool Consume(ItemId item, int quantity = 1)
    {
        for (auto entry = entries.begin(); entry != entries.end(); ++entry)
        {
            if (entry->item == item && entry->quantity >= quantity && quantity > 0)
            {
                entry->quantity -= quantity;
                if (entry->quantity == 0)
                {
                    entries.erase(entry);
                }
                return true;
            }
        }
        return false;
    }

    const std::vector<InventoryEntry> &Entries() const
    {
        return entries;
    }

private:
    std::vector<InventoryEntry> entries;
};

struct CombatStats
{
    int hp = 1;
    int maxHp = 1;
    int attack = 1;
    int defense = 0;
};

struct Entity
{
    std::string id;
    std::string name;
    EntityType type = EntityType::Npc;
    Rectangle bounds = {0, 0, 32, 32};
    bool active = true;
    ItemId item = ItemId::None;
    ItemId requiredItem = ItemId::None;
    int destination = -1;
    bool boss = false;
    CombatStats combat;
    std::vector<std::vector<std::string>> conversations;
};

struct Area
{
    std::string id;
    std::string name;
    std::string description;
    Color background;
    Color accent;
    std::vector<Entity> entities;
    std::vector<Rectangle> obstacles;
};

Entity MakeEntity(const std::string &id, const std::string &name, EntityType type,
                 float x, float y, float width, float height)
{
    Entity entity;
    entity.id = id;
    entity.name = name;
    entity.type = type;
    entity.bounds = {x, y, width, height};
    return entity;
}

Entity MakePortal(const std::string &id, const std::string &name, float x, int destination,
                  ItemId requiredItem = ItemId::None)
{
    Entity portal = MakeEntity(id, name, EntityType::Portal, x, 326, 46, 72);
    portal.destination = destination;
    portal.requiredItem = requiredItem;
    return portal;
}

Entity MakeEnemy(const std::string &id, const std::string &name, float x, float y,
                 CombatStats combat, bool boss = false)
{
    Entity enemy = MakeEntity(id, name, EntityType::Enemy, x, y, boss ? 58.0f : 38.0f,
                              boss ? 58.0f : 38.0f);
    enemy.combat = combat;
    enemy.boss = boss;
    return enemy;
}

std::vector<Area> CreateWorld()
{
    std::vector<Area> areas;

    Area village;
    village.id = "village";
    village.name = "Willowmere Village";
    village.description = "A quiet village at the edge of the old forest.";
    village.background = {81, 142, 105, 255};
    village.accent = {197, 171, 106, 255};

    Entity elder = MakeEntity("elder", "Elder Rowan", EntityType::Npc, 182, 232, 34, 52);
    elder.conversations = {
        {"The forest path is east of here. A rusty key lies beyond the old stones.",
         "The key opens the ancient gate. Search the forest carefully."},
        {"You found the key! The ruins lie beyond the eastern forest gate.",
         "A sealed chest in the ruins holds the Sun Sigil. Bring it to the far gate."},
        {"The Sun Sigil is yours. The final guardian waits beyond the ruins.",
         "Stay steady, traveler. A potion can turn the tide of a battle."}};
    village.entities.push_back(std::move(elder));

    Entity villagePotion = MakeEntity("village-potion", "Healing Potion", EntityType::Pickup,
                                     300, 455, 24, 24);
    villagePotion.item = ItemId::HealingPotion;
    village.entities.push_back(std::move(villagePotion));
    village.entities.push_back(MakePortal("village-east", "Forest Path", 878, 1));
    areas.push_back(std::move(village));

    Area forest;
    forest.id = "forest";
    forest.name = "Whispering Woods";
    forest.description = "An old trail winds between the trees.";
    forest.background = {52, 112, 80, 255};
    forest.accent = {153, 183, 116, 255};

    Entity ranger = MakeEntity("ranger", "Mira the Ranger", EntityType::Npc, 280, 186, 32, 50);
    ranger.conversations = {
        {"A green slime guards the clearing ahead. Keep your distance until you are ready.",
         "I saw a glint of metal near the northern rocks. That may be the gate key."}};
    forest.entities.push_back(std::move(ranger));
    forest.entities.push_back(MakeEnemy("forest-slime", "Forest Slime", 515, 350,
                                       {24, 24, 7, 1}));

    Entity key = MakeEntity("rusty-key", "Rusty Key", EntityType::Pickup, 712, 176, 24, 24);
    key.item = ItemId::RustyKey;
    forest.entities.push_back(std::move(key));
    forest.entities.push_back(MakePortal("forest-west", "Village Path", 34, 0));
    forest.entities.push_back(MakePortal("forest-east", "Ancient Gate", 878, 2, ItemId::RustyKey));
    areas.push_back(std::move(forest));

    Area ruins;
    ruins.id = "ruins";
    ruins.name = "Sunken Ruins";
    ruins.description = "Weathered stonework surrounds a sealed chest.";
    ruins.background = {116, 107, 92, 255};
    ruins.accent = {202, 177, 115, 255};
    ruins.obstacles.push_back({342, 212, 48, 72});
    ruins.obstacles.push_back({580, 446, 54, 44});

    Entity chest = MakeEntity("sigil-chest", "Sealed Chest", EntityType::Chest, 446, 367, 52, 42);
    chest.requiredItem = ItemId::RustyKey;
    chest.item = ItemId::SunSigil;
    ruins.entities.push_back(std::move(chest));
    ruins.entities.push_back(MakePortal("ruins-west", "Forest Gate", 34, 1));
    ruins.entities.push_back(MakePortal("ruins-east", "Guardian's Path", 878, 3,
                                        ItemId::SunSigil));
    areas.push_back(std::move(ruins));

    Area sanctum;
    sanctum.id = "sanctum";
    sanctum.name = "Sunken Sanctum";
    sanctum.description = "The last guardian protects the ancient sanctuary.";
    sanctum.background = {79, 73, 100, 255};
    sanctum.accent = {225, 190, 103, 255};
    sanctum.entities.push_back(MakeEnemy("final-guardian", "Sun Guardian", 656, 326,
                                        {58, 58, 11, 3}, true));
    sanctum.entities.push_back(MakePortal("sanctum-west", "Return to Ruins", 34, 2));
    areas.push_back(std::move(sanctum));

    return areas;
}

float DistanceSquared(Vector2 a, Vector2 b)
{
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

Vector2 CenterOf(Rectangle rectangle)
{
    return {rectangle.x + rectangle.width / 2.0f, rectangle.y + rectangle.height / 2.0f};
}

class AssetManager
{
public:
    AssetManager() = default;

    void Load()
    {
        LoadTextureAsset("player", "Actor/Character/Boy/SpriteSheet.png");
        LoadTextureAsset("villager", "Actor/Character/Villager/SpriteSheet.png");
        LoadTextureAsset("ranger", "Actor/Character/Hunter/SpriteSheet.png");
        LoadTextureAsset("slime", "Actor/Monster/Slime/Slime.png");
        LoadTextureAsset("guardian", "Actor/Boss/DemonCyclop/Idle.png");
        LoadTextureAsset("field-tiles", "Backgrounds/Tilesets/TilesetField.png");
        LoadTextureAsset("dungeon-tiles", "Backgrounds/Tilesets/TilesetDungeon.png");
        LoadTextureAsset("potion", "Items/Potion/LifePot.png");
        LoadTextureAsset("key", "Items/Treasure/GoldKey.png");
        LoadTextureAsset("sigil", "Items/Resource/GemYellow.png");
        LoadTextureAsset("chest", "Items/Treasure/LittleTreasureChest.png");
        LoadTextureAsset("dialogue-panel", "Ui/Dialog/DialogueBoxSimple.png");
        LoadTextureAsset("wood-panel", "Ui/Theme/Theme Wood/nine_path_panel_2.png");
        if (font.texture.id == 0)
        {
            const std::string fontPath = Path("Ui/Font/NormalFont.ttf");
            if (FileExists(fontPath.c_str()))
            {
                const Font candidate = LoadFontEx(fontPath.c_str(), 32, nullptr, 0);
                if (candidate.texture.id != 0 &&
                    candidate.texture.id != GetFontDefault().texture.id)
                {
                    font = candidate;
                    fontLoaded = true;
                }
                else
                {
                    TraceLog(LOG_WARNING, "Could not load font asset: %s", fontPath.c_str());
                }
            }
            else
            {
                TraceLog(LOG_WARNING, "Missing font asset: %s", fontPath.c_str());
            }
        }

        if (IsAudioDeviceReady())
        {
            LoadMusicAsset("adventure", "Audio/Musics/1 - Adventure Begin.ogg");
            LoadMusicAsset("battle", "Audio/Musics/17 - Fight.ogg");
            LoadSoundAsset("confirm", "Audio/Sounds/Menu/Accept.wav");
            LoadSoundAsset("menu-move", "Audio/Sounds/Menu/Move1.wav");
            LoadSoundAsset("pickup", "Audio/Sounds/Bonus/Bonus.wav");
            LoadSoundAsset("battle-start", "Audio/Sounds/Hit & Impact/Impact.wav");
            LoadSoundAsset("attack", "Audio/Sounds/Hit & Impact/Hit1.wav");
            LoadSoundAsset("heal", "Audio/Sounds/Magic & Skill/Heal.wav");
            LoadSoundAsset("victory", "Audio/Jingles/Success1.wav");
            LoadSoundAsset("game-over", "Audio/Jingles/GameOver.wav");
        }
        else
        {
            TraceLog(LOG_WARNING, "Audio device unavailable; continuing without audio.");
        }

        for (auto &entry : textures)
        {
            SetTextureFilter(entry.second, TEXTURE_FILTER_POINT);
        }
    }

    ~AssetManager()
    {
        StopMusic();
        for (auto &entry : textures)
        {
            UnloadTexture(entry.second);
        }
        for (auto &entry : sounds)
        {
            UnloadSound(entry.second);
        }
        for (auto &entry : musicTracks)
        {
            UnloadMusicStream(entry.second);
        }
        if (fontLoaded)
        {
            UnloadFont(font);
        }
    }

    AssetManager(const AssetManager &) = delete;
    AssetManager &operator=(const AssetManager &) = delete;

    void Update()
    {
        if (!activeMusic.empty())
        {
            const auto track = musicTracks.find(activeMusic);
            if (track != musicTracks.end())
            {
                UpdateMusicStream(track->second);
            }
        }
    }

    const Texture2D *GetTexture(const std::string &key) const
    {
        const auto texture = textures.find(key);
        return texture == textures.end() ? nullptr : &texture->second;
    }

    void DrawRegion(const std::string &key, Rectangle source, Rectangle destination,
                    Color tint = WHITE) const
    {
        const Texture2D *texture = GetTexture(key);
        if (texture != nullptr)
        {
            DrawTexturePro(*texture, source, destination, {0, 0}, 0.0f, tint);
        }
    }

    void DrawPanel(const std::string &key, Rectangle destination) const
    {
        const Texture2D *texture = GetTexture(key);
        if (texture != nullptr)
        {
            const NPatchInfo patch = {{0, 0, static_cast<float>(texture->width),
                                       static_cast<float>(texture->height)},
                                      4, 4, 4, 4, NPATCH_NINE_PATCH};
            DrawTextureNPatch(*texture, patch, destination, {0, 0}, 0.0f, WHITE);
        }
    }

    void DrawText(const char *text, int x, int y, float fontSize, Color color) const
    {
        if (fontLoaded)
        {
            DrawTextEx(font, text, {static_cast<float>(x), static_cast<float>(y)},
                       fontSize, 1.0f, color);
        }
        else
        {
            ::DrawText(text, x, y, static_cast<int>(fontSize), color);
        }
    }

    int MeasureText(const char *text, float fontSize) const
    {
        if (fontLoaded)
        {
            return static_cast<int>(MeasureTextEx(font, text, fontSize, 1.0f).x);
        }
        return ::MeasureText(text, static_cast<int>(fontSize));
    }

    void PlaySound(const std::string &key) const
    {
        const auto sound = sounds.find(key);
        if (sound != sounds.end())
        {
            ::PlaySound(sound->second);
        }
    }

    void PlayMusic(const std::string &key)
    {
        if (activeMusic == key)
        {
            return;
        }

        StopMusic();
        const auto track = musicTracks.find(key);
        if (track != musicTracks.end())
        {
            activeMusic = key;
            PlayMusicStream(track->second);
        }
    }

    void StopMusic()
    {
        if (activeMusic.empty())
        {
            return;
        }

        const auto track = musicTracks.find(activeMusic);
        if (track != musicTracks.end())
        {
            StopMusicStream(track->second);
        }
        activeMusic.clear();
    }

private:
    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Sound> sounds;
    std::unordered_map<std::string, Music> musicTracks;
    Font font = {};
    bool fontLoaded = false;
    std::string activeMusic;

    std::string Path(const std::string &relativePath) const
    {
        return "src/assets/" + relativePath;
    }

    void LoadTextureAsset(const std::string &key, const std::string &relativePath)
    {
        if (textures.find(key) != textures.end())
        {
            return;
        }

        const std::string path = Path(relativePath);
        if (!FileExists(path.c_str()))
        {
            TraceLog(LOG_WARNING, "Missing texture asset: %s", path.c_str());
            return;
        }

        const Texture2D texture = LoadTexture(path.c_str());
        if (texture.id == 0)
        {
            TraceLog(LOG_WARNING, "Could not load texture asset: %s", path.c_str());
            return;
        }
        textures.emplace(key, texture);
    }

    void LoadSoundAsset(const std::string &key, const std::string &relativePath)
    {
        if (sounds.find(key) != sounds.end())
        {
            return;
        }

        const std::string path = Path(relativePath);
        if (!FileExists(path.c_str()))
        {
            TraceLog(LOG_WARNING, "Missing sound asset: %s", path.c_str());
            return;
        }

        const Sound sound = LoadSound(path.c_str());
        if (sound.frameCount == 0)
        {
            TraceLog(LOG_WARNING, "Could not load sound asset: %s", path.c_str());
            return;
        }
        sounds.emplace(key, sound);
    }

    void LoadMusicAsset(const std::string &key, const std::string &relativePath)
    {
        if (musicTracks.find(key) != musicTracks.end())
        {
            return;
        }

        const std::string path = Path(relativePath);
        if (!FileExists(path.c_str()))
        {
            TraceLog(LOG_WARNING, "Missing music asset: %s", path.c_str());
            return;
        }

        Music music = LoadMusicStream(path.c_str());
        if (music.frameCount == 0)
        {
            TraceLog(LOG_WARNING, "Could not load music asset: %s", path.c_str());
            return;
        }
        music.looping = true;
        musicTracks.emplace(key, music);
    }
};

void DrawWrappedText(const AssetManager &assets, const std::string &text, int x, int y, int width,
                     int fontSize, Color color)
{
    std::istringstream words(text);
    std::string word;
    std::string line;
    int lineY = y;

    while (words >> word)
    {
        const std::string candidate = line.empty() ? word : line + " " + word;
        if (!line.empty() && assets.MeasureText(candidate.c_str(), static_cast<float>(fontSize)) > width)
        {
            assets.DrawText(line.c_str(), x, lineY, static_cast<float>(fontSize), color);
            line = word;
            lineY += fontSize + 6;
        }
        else
        {
            line = candidate;
        }
    }

    if (!line.empty())
    {
        assets.DrawText(line.c_str(), x, lineY, static_cast<float>(fontSize), color);
    }
}

class AdventureGame
{
public:
    AdventureGame()
    {
        assets.Load();
        Reset();
        assets.PlayMusic("adventure");
    }

    void Update(float deltaTime)
    {
        animationTimer += deltaTime;
        assets.Update();

        if (statusTimer > 0.0f)
        {
            statusTimer = std::max(0.0f, statusTimer - deltaTime);
        }

        switch (state)
        {
        case GameState::Exploration:
            UpdateExploration(deltaTime);
            break;
        case GameState::Dialogue:
            UpdateDialogue();
            break;
        case GameState::Inventory:
            UpdateInventory();
            break;
        case GameState::Battle:
            UpdateBattle(deltaTime);
            break;
        case GameState::GameOver:
        case GameState::Victory:
            if (IsKeyPressed(KEY_R))
            {
                Reset();
            }
            break;
        }

        SyncAudioForState();
    }

    void Draw() const
    {
        BeginDrawing();
        ClearBackground({24, 28, 35, 255});

        DrawWorld();
        DrawHud();

        switch (state)
        {
        case GameState::Dialogue:
            DrawDialogue();
            break;
        case GameState::Inventory:
            DrawInventory();
            break;
        case GameState::Battle:
            DrawBattle();
            break;
        case GameState::GameOver:
            DrawEndScreen(false);
            break;
        case GameState::Victory:
            DrawEndScreen(true);
            break;
        case GameState::Exploration:
            break;
        }

        EndDrawing();
    }

private:
    AssetManager assets;
    std::vector<Area> areas;
    Inventory inventory;
    GameState state = GameState::Exploration;
    int currentArea = 0;
    Rectangle player = {84, 346, 28, 36};
    CombatStats playerCombat = {48, 48, 10, 2};
    CombatStats battleEnemy;
    std::string battleEnemyName;
    std::string battleEnemyId;
    std::string battleMessage;
    std::string statusMessage;
    std::string dialogueSpeaker;
    std::vector<std::string> activeDialogue;
    int dialogueLine = 0;
    int selectedInventoryItem = 0;
    int selectedBattleAction = 0;
    float battleMessageTimer = 0.0f;
    float statusTimer = 0.0f;
    float animationTimer = 0.0f;
    bool battleEnemyIsBoss = false;
    float battleEnemyCenterX = 0.0f;
    bool forestGateUnlocked = false;
    bool guardianGateUnlocked = false;
    bool endStateAudioPlayed = false;
    bool playerMoving = false;
    int playerFacingRow = 0;

    Area &CurrentArea()
    {
        return areas[currentArea];
    }

    const Area &CurrentArea() const
    {
        return areas[currentArea];
    }

    void Reset()
    {
        areas = CreateWorld();
        inventory = Inventory();
        inventory.Add(ItemId::HealingPotion);
        state = GameState::Exploration;
        currentArea = 0;
        player = {84, 346, 28, 36};
        playerCombat = {48, 48, 10, 2};
        selectedInventoryItem = 0;
        selectedBattleAction = 0;
        forestGateUnlocked = false;
        guardianGateUnlocked = false;
        endStateAudioPlayed = false;
        playerFacingRow = 0;
        statusMessage = "Speak with Elder Rowan. Find the key, then follow the clues east.";
        statusTimer = 5.0f;
    }

    void SetStatus(const std::string &message)
    {
        statusMessage = message;
        statusTimer = 4.0f;
    }

    void SyncAudioForState()
    {
        if (state == GameState::GameOver || state == GameState::Victory)
        {
            assets.StopMusic();
            if (!endStateAudioPlayed)
            {
                assets.PlaySound(state == GameState::Victory ? "victory" : "game-over");
                endStateAudioPlayed = true;
            }
        }
        else
        {
            assets.PlayMusic(state == GameState::Battle ? "battle" : "adventure");
        }
    }

    int FindNearbyEntity(float maximumDistance = 88.0f) const
    {
        const Vector2 playerCenter = CenterOf(player);
        int nearestIndex = -1;
        float nearestDistance = maximumDistance * maximumDistance;

        const std::vector<Entity> &entities = CurrentArea().entities;
        for (std::size_t index = 0; index < entities.size(); ++index)
        {
            const Entity &entity = entities[index];
            if (!entity.active || entity.type == EntityType::Enemy)
            {
                continue;
            }

            const float distance = DistanceSquared(playerCenter, CenterOf(entity.bounds));
            if (distance < nearestDistance)
            {
                nearestDistance = distance;
                nearestIndex = static_cast<int>(index);
            }
        }
        return nearestIndex;
    }

    bool CanUsePortal(const Entity &portal) const
    {
        if (portal.id == "forest-east" && forestGateUnlocked)
        {
            return true;
        }
        if (portal.id == "ruins-east" && guardianGateUnlocked)
        {
            return true;
        }
        return portal.requiredItem == ItemId::None || inventory.Has(portal.requiredItem);
    }

    void UpdateExploration(float deltaTime)
    {
        if (IsKeyPressed(KEY_I))
        {
            state = GameState::Inventory;
            return;
        }

        if (IsKeyPressed(KEY_E))
        {
            const int nearbyEntity = FindNearbyEntity();
            if (nearbyEntity >= 0)
            {
                Interact(CurrentArea().entities[static_cast<std::size_t>(nearbyEntity)]);
                return;
            }
        }

        float moveX = 0.0f;
        float moveY = 0.0f;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        {
            moveX -= 1.0f;
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        {
            moveX += 1.0f;
        }
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        {
            moveY -= 1.0f;
        }
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        {
            moveY += 1.0f;
        }

        const float length = std::sqrt(moveX * moveX + moveY * moveY);
        if (length > 0.0f)
        {
            playerMoving = true;
            if (std::abs(moveX) > std::abs(moveY))
            {
                playerFacingRow = moveX > 0.0f ? 2 : 1;
            }
            else
            {
                playerFacingRow = moveY > 0.0f ? 0 : 3;
            }
            moveX = moveX / length * PlayerSpeed * deltaTime;
            moveY = moveY / length * PlayerSpeed * deltaTime;
            MovePlayer(moveX, moveY);
        }
        else
        {
            playerMoving = false;
        }

        for (Entity &entity : CurrentArea().entities)
        {
            if (entity.active && entity.type == EntityType::Enemy &&
                DistanceSquared(CenterOf(player), CenterOf(entity.bounds)) < 64.0f * 64.0f)
            {
                StartBattle(entity);
                return;
            }
        }
    }

    void MovePlayer(float moveX, float moveY)
    {
        Rectangle candidate = player;
        candidate.x += moveX;
        candidate.x = std::max(34.0f, std::min(candidate.x, ScreenWidth - 34.0f - candidate.width));
        if (!HitsObstacle(candidate))
        {
            player.x = candidate.x;
        }

        candidate = player;
        candidate.y += moveY;
        candidate.y = std::max(static_cast<float>(FieldTop + 16),
                               std::min(candidate.y, static_cast<float>(FieldBottom - 16) - candidate.height));
        if (!HitsObstacle(candidate))
        {
            player.y = candidate.y;
        }
    }

    bool HitsObstacle(Rectangle bounds) const
    {
        for (const Rectangle obstacle : CurrentArea().obstacles)
        {
            if (CheckCollisionRecs(bounds, obstacle))
            {
                return true;
            }
        }
        return false;
    }

    void Interact(Entity &entity)
    {
        assets.PlaySound("confirm");
        switch (entity.type)
        {
        case EntityType::Npc:
            OpenDialogue(entity);
            break;
        case EntityType::Pickup:
            inventory.Add(entity.item);
            entity.active = false;
            assets.PlaySound("pickup");
            SetStatus(std::string("Picked up ") + GetItemDefinition(entity.item).name + ".");
            break;
        case EntityType::Chest:
            if (entity.active && inventory.Consume(entity.requiredItem))
            {
                inventory.Add(entity.item);
                entity.active = false;
                assets.PlaySound("pickup");
                SetStatus("The key opens the chest. You found the Sun Sigil!");
            }
            else if (entity.active)
            {
                SetStatus("The chest is locked. It needs a Rusty Key.");
            }
            else
            {
                SetStatus("The chest is empty.");
            }
            break;
        case EntityType::Portal:
        {
            if (!CanUsePortal(entity))
            {
                SetStatus("The path is sealed. You need the " +
                          std::string(GetItemDefinition(entity.requiredItem).name) + ".");
            }
            else
            {
                if (entity.id == "forest-east")
                {
                    forestGateUnlocked = true;
                }
                else if (entity.id == "ruins-east")
                {
                    guardianGateUnlocked = true;
                }
                TravelTo(entity.destination);
            }
            break;
        }
        case EntityType::Enemy:
            break;
        }
    }

    void TravelTo(int destination)
    {
        if (destination < 0 || destination >= static_cast<int>(areas.size()))
        {
            SetStatus("That path does not lead anywhere.");
            return;
        }

        const bool travellingEast = destination > currentArea;
        currentArea = destination;
        player.x = travellingEast ? 80.0f : 832.0f;
        player.y = 346.0f;
        SetStatus(CurrentArea().description);
    }

    void OpenDialogue(const Entity &entity)
    {
        dialogueSpeaker = entity.name;
        if (entity.id == "elder" && inventory.Has(ItemId::SunSigil))
        {
            activeDialogue = entity.conversations[2];
        }
        else if (entity.id == "elder" && inventory.Has(ItemId::RustyKey))
        {
            activeDialogue = entity.conversations[1];
        }
        else
        {
            activeDialogue = entity.conversations.empty() ? std::vector<std::string>() :
                             entity.conversations.front();
        }
        dialogueLine = 0;
        state = GameState::Dialogue;
    }

    void UpdateDialogue()
    {
        if (IsKeyPressed(KEY_ESCAPE))
        {
            state = GameState::Exploration;
        }
        else if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        {
            ++dialogueLine;
            if (dialogueLine >= static_cast<int>(activeDialogue.size()))
            {
                state = GameState::Exploration;
            }
        }
    }

    void UpdateInventory()
    {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_I))
        {
            state = GameState::Exploration;
            return;
        }

        const int itemCount = static_cast<int>(inventory.Entries().size());
        if (itemCount == 0)
        {
            selectedInventoryItem = 0;
            return;
        }

        selectedInventoryItem = std::max(0, std::min(selectedInventoryItem, itemCount - 1));
        if (IsKeyPressed(KEY_UP))
        {
            assets.PlaySound("menu-move");
            selectedInventoryItem = (selectedInventoryItem + itemCount - 1) % itemCount;
        }
        if (IsKeyPressed(KEY_DOWN))
        {
            assets.PlaySound("menu-move");
            selectedInventoryItem = (selectedInventoryItem + 1) % itemCount;
        }
        if (IsKeyPressed(KEY_ENTER))
        {
            UseInventoryItem(inventory.Entries()[static_cast<std::size_t>(selectedInventoryItem)].item);
        }
    }

    void UseInventoryItem(ItemId item)
    {
        if (item != ItemId::HealingPotion)
        {
            SetStatus("That item is used by interacting with the right place.");
            return;
        }

        if (playerCombat.hp >= playerCombat.maxHp)
        {
            SetStatus("Your health is already full.");
            return;
        }

        if (inventory.Consume(ItemId::HealingPotion))
        {
            playerCombat.hp = std::min(playerCombat.maxHp, playerCombat.hp + 18);
            assets.PlaySound("heal");
            SetStatus("You used a Healing Potion and restored 18 HP.");
        }
    }

    void StartBattle(const Entity &enemy)
    {
        battleEnemy = enemy.combat;
        battleEnemyName = enemy.name;
        battleEnemyId = enemy.id;
        battleEnemyIsBoss = enemy.boss;
        battleEnemyCenterX = CenterOf(enemy.bounds).x;
        assets.PlaySound("battle-start");
        battleMessage = enemy.boss ? "The Sun Guardian challenges you!" :
                                     enemy.name + " attacks!";
        battleMessageTimer = 1.0f;
        selectedBattleAction = 0;
        state = GameState::Battle;
    }

    void UpdateBattle(float deltaTime)
    {
        if (battleMessageTimer > 0.0f)
        {
            battleMessageTimer = std::max(0.0f, battleMessageTimer - deltaTime);
            return;
        }

        if (IsKeyPressed(KEY_UP))
        {
            assets.PlaySound("menu-move");
            selectedBattleAction = (selectedBattleAction + 2) % 3;
        }
        if (IsKeyPressed(KEY_DOWN))
        {
            assets.PlaySound("menu-move");
            selectedBattleAction = (selectedBattleAction + 1) % 3;
        }
        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1))
        {
            selectedBattleAction = 0;
            ResolveBattleAction();
        }
        else if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2))
        {
            selectedBattleAction = 1;
            ResolveBattleAction();
        }
        else if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3))
        {
            selectedBattleAction = 2;
            ResolveBattleAction();
        }
        else if (IsKeyPressed(KEY_ENTER))
        {
            ResolveBattleAction();
        }
    }

    void ResolveBattleAction()
    {
        assets.PlaySound("confirm");
        if (selectedBattleAction == 0)
        {
            assets.PlaySound("attack");
            const int damage = std::max(1, playerCombat.attack - battleEnemy.defense +
                                           GetRandomValue(-1, 2));
            battleEnemy.hp = std::max(0, battleEnemy.hp - damage);
            if (battleEnemy.hp == 0)
            {
                FinishBattle();
                return;
            }

            battleMessage = "You strike " + battleEnemyName + " for " +
                            std::to_string(damage) + " damage. ";
            battleMessage += EnemyTurn();
        }
        else if (selectedBattleAction == 1)
        {
            if (!inventory.Has(ItemId::HealingPotion))
            {
                battleMessage = "You have no Healing Potions.";
                battleMessageTimer = 1.0f;
                return;
            }
            if (playerCombat.hp >= playerCombat.maxHp)
            {
                battleMessage = "Your health is already full.";
                battleMessageTimer = 1.0f;
                return;
            }

            inventory.Consume(ItemId::HealingPotion);
            playerCombat.hp = std::min(playerCombat.maxHp, playerCombat.hp + 18);
            assets.PlaySound("heal");
            battleMessage = "You use a Healing Potion. " + EnemyTurn();
        }
        else
        {
            if (battleEnemyIsBoss)
            {
                battleMessage = "There is no escape from the final guardian!";
                battleMessage += " " + EnemyTurn();
            }
            else if (GetRandomValue(0, 99) < 60)
            {
                player.x = battleEnemyCenterX > CenterOf(player).x
                               ? std::max(34.0f, player.x - 84.0f)
                               : std::min(static_cast<float>(ScreenWidth - 34) - player.width,
                                          player.x + 84.0f);
                state = GameState::Exploration;
                SetStatus("You escaped from the encounter.");
                return;
            }
            else
            {
                battleMessage = "You could not get away. " + EnemyTurn();
            }
        }

        if (state == GameState::Battle)
        {
            battleMessageTimer = 1.25f;
        }
    }

    std::string EnemyTurn()
    {
        const int damage = std::max(1, battleEnemy.attack - playerCombat.defense +
                                       GetRandomValue(-1, 1));
        playerCombat.hp = std::max(0, playerCombat.hp - damage);
        if (playerCombat.hp == 0)
        {
            state = GameState::GameOver;
            return battleEnemyName + " deals " + std::to_string(damage) +
                   " damage. You have been defeated.";
        }
        return battleEnemyName + " hits you for " + std::to_string(damage) + " damage.";
    }

    void FinishBattle()
    {
        for (Entity &entity : CurrentArea().entities)
        {
            if (entity.id == battleEnemyId)
            {
                entity.active = false;
                break;
            }
        }

        if (battleEnemyIsBoss)
        {
            state = GameState::Victory;
            return;
        }

        state = GameState::Exploration;
        SetStatus("You defeated " + battleEnemyName + ".");
    }

    const char *ItemTextureKey(ItemId item) const
    {
        switch (item)
        {
        case ItemId::HealingPotion:
            return "potion";
        case ItemId::RustyKey:
            return "key";
        case ItemId::SunSigil:
            return "sigil";
        default:
            return "";
        }
    }

    void DrawSpriteFrame(const std::string &key, int columns, int rows, int frame, int row,
                         Rectangle destination) const
    {
        const Texture2D *texture = assets.GetTexture(key);
        if (texture == nullptr || columns <= 0 || rows <= 0)
        {
            return;
        }

        const int frameWidth = texture->width / columns;
        const int frameHeight = texture->height / rows;
        const Rectangle source = {
            static_cast<float>((frame % columns) * frameWidth),
            static_cast<float>((row % rows) * frameHeight),
            static_cast<float>(frameWidth),
            static_cast<float>(frameHeight)};
        assets.DrawRegion(key, source, destination);
    }

    bool DrawCharacterFrame(const std::string &key, int frame, int row,
                            Rectangle destination) const
    {
        if (assets.GetTexture(key) == nullptr)
        {
            return false;
        }
        DrawSpriteFrame(key, 4, 7, frame, row, destination);
        return true;
    }

    void DrawUiPanel(const std::string &key, Rectangle bounds, Color fallback) const
    {
        if (assets.GetTexture(key) != nullptr)
        {
            assets.DrawPanel(key, bounds);
        }
        else
        {
            DrawRectangleRec(bounds, fallback);
            DrawRectangleLinesEx(bounds, 2.0f, {194, 170, 112, 255});
        }
    }

    void DrawWorld() const
    {
        const Area &area = CurrentArea();
        DrawRectangle(0, 0, ScreenWidth, ScreenHeight, area.background);
        const bool underground = area.id == "ruins" || area.id == "sanctum";
        const std::string tileSet = underground ? "dungeon-tiles" : "field-tiles";
        const Rectangle floorTile = underground ? Rectangle{16, 16, 16, 16}
                                                : Rectangle{0, 16, 16, 16};
        for (int x = 0; x < ScreenWidth; x += 48)
        {
            for (int y = FieldTop; y < FieldBottom; y += 48)
            {
                assets.DrawRegion(tileSet, floorTile,
                                  {static_cast<float>(x), static_cast<float>(y), 48, 48});
            }
        }
        DrawRectangle(0, 0, ScreenWidth, FieldTop, {35, 42, 49, 255});
        DrawRectangle(0, FieldBottom, ScreenWidth, ScreenHeight - FieldBottom,
                      {35, 42, 49, 255});

        DrawRectangle(0, 325, ScreenWidth, 74, Fade(area.accent, 0.13f));
        for (const Rectangle obstacle : area.obstacles)
        {
            DrawRectangleRec(obstacle, {75, 69, 62, 255});
            DrawRectangleLinesEx(obstacle, 3.0f, Fade(BLACK, 0.3f));
        }

        for (const Entity &entity : area.entities)
        {
            if (entity.active)
            {
                DrawEntity(entity);
            }
        }
        DrawPlayer();
    }

    void DrawEntity(const Entity &entity) const
    {
        const Rectangle bounds = entity.bounds;
        switch (entity.type)
        {
        case EntityType::Npc:
            if (!DrawCharacterFrame(entity.id == "ranger" ? "ranger" : "villager", 1, 0,
                                    {bounds.x - 7, bounds.y + 3, 48, 48}))
            {
                DrawRectangleRec(bounds, {73, 105, 164, 255});
                DrawCircle(static_cast<int>(bounds.x + bounds.width / 2),
                           static_cast<int>(bounds.y + 11), 12, {224, 185, 143, 255});
            }
            break;
        case EntityType::Pickup:
        {
            const char *itemTexture = ItemTextureKey(entity.item);
            if (assets.GetTexture(itemTexture) != nullptr)
            {
                assets.DrawRegion(itemTexture,
                                  {0, 0, static_cast<float>(assets.GetTexture(itemTexture)->width),
                                   static_cast<float>(assets.GetTexture(itemTexture)->height)},
                                  {bounds.x - 4, bounds.y - 7, 32, 36});
            }
            else
            {
                DrawPoly(CenterOf(bounds), 4, 16, 45.0f,
                         entity.item == ItemId::HealingPotion ? RED : GOLD);
            }
            break;
        }
        case EntityType::Chest:
            if (assets.GetTexture("chest") != nullptr)
            {
                assets.DrawRegion("chest", {0, 0, 32, 16},
                                  {bounds.x - 6, bounds.y + 4, 64, 32});
            }
            else
            {
                DrawRectangleRec(bounds, {132, 79, 39, 255});
                DrawRectangleLinesEx(bounds, 3.0f, {58, 43, 36, 255});
            }
            break;
        case EntityType::Enemy:
            if (entity.boss)
            {
                const int frame = static_cast<int>(animationTimer * 5.0f) % 5;
                DrawSpriteFrame("guardian", 5, 1, frame, 0,
                                {bounds.x - 27, bounds.y - 38, 112, 112});
                if (assets.GetTexture("guardian") == nullptr)
                {
                    DrawCircleV(CenterOf(bounds), bounds.width / 2.0f,
                                {159, 78, 91, 255});
                }
            }
            else
            {
                const int frame = static_cast<int>(animationTimer * 6.0f) % 4;
                DrawSpriteFrame("slime", 4, 4, frame, 0,
                                {bounds.x - 6, bounds.y - 6, 50, 50});
                if (assets.GetTexture("slime") == nullptr)
                {
                    DrawCircleV(CenterOf(bounds), bounds.width / 2.0f,
                                {121, 186, 91, 255});
                }
            }
            break;
        case EntityType::Portal:
        {
            const bool locked = !CanUsePortal(entity);
            const Color portalColor = locked ? Color{131, 92, 73, 255} : CurrentArea().accent;
            DrawRectangleRec(bounds, Fade(portalColor, 0.42f));
            DrawRectangleLinesEx(bounds, 3.0f, portalColor);
            DrawCircle(static_cast<int>(bounds.x + bounds.width / 2),
                       static_cast<int>(bounds.y + bounds.height / 2), 5, RAYWHITE);
            break;
        }
        }

        if (entity.type == EntityType::Npc || entity.type == EntityType::Pickup ||
            entity.type == EntityType::Chest || entity.type == EntityType::Portal)
        {
            assets.DrawText(entity.name.c_str(), static_cast<int>(bounds.x) - 25,
                            static_cast<int>(bounds.y) + static_cast<int>(bounds.height) + 5,
                            14, RAYWHITE);
        }
    }

    void DrawPlayer() const
    {
        DrawEllipse(static_cast<int>(player.x + player.width / 2),
                    static_cast<int>(player.y + player.height - 2), 13, 5, Fade(BLACK, 0.28f));
        const int frame = playerMoving ? 1 + (static_cast<int>(animationTimer * 8.0f) % 2) : 0;
        if (!DrawCharacterFrame("player", frame, playerFacingRow,
                                {player.x - 10, player.y - 10, 48, 48}))
        {
            DrawRectangle(static_cast<int>(player.x + 4), static_cast<int>(player.y + 13),
                          static_cast<int>(player.width - 8),
                          static_cast<int>(player.height - 13), {68, 96, 153, 255});
            DrawCircle(static_cast<int>(player.x + player.width / 2),
                       static_cast<int>(player.y + 9), 10, {235, 199, 157, 255});
        }
    }

    void DrawHud() const
    {
        assets.DrawText(CurrentArea().name.c_str(), 22, 12, 24, RAYWHITE);
        assets.DrawText(TextFormat("HP %d / %d", playerCombat.hp, playerCombat.maxHp),
                        22, 43, 16, {225, 228, 231, 255});
        DrawRectangle(136, 47, 150, 12, {74, 77, 81, 255});
        const float healthRatio = playerCombat.maxHp > 0 ?
                                  static_cast<float>(playerCombat.hp) / playerCombat.maxHp : 0.0f;
        DrawRectangle(136, 47, static_cast<int>(150.0f * healthRatio), 12, {184, 69, 70, 255});

        const Texture2D *potion = assets.GetTexture("potion");
        if (potion != nullptr)
        {
            assets.DrawRegion("potion",
                              {0, 0, static_cast<float>(potion->width),
                               static_cast<float>(potion->height)},
                              {static_cast<float>(ScreenWidth - 178), 13, 24, 30});
        }
        assets.DrawText(TextFormat("Potions: %d", inventory.Count(ItemId::HealingPotion)),
                        ScreenWidth - 151, 18, 18, RAYWHITE);
        assets.DrawText("WASD / Arrows: Move    E: Interact    I: Inventory",
                        22, FieldBottom + 13, 16, RAYWHITE);

        if (FindNearbyEntity() >= 0 && state == GameState::Exploration)
        {
            assets.DrawText("E  Interact", ScreenWidth - 142, FieldBottom + 13, 16, GOLD);
        }
        if (statusTimer > 0.0f)
        {
            assets.DrawText(statusMessage.c_str(), 22, ScreenHeight - 27, 15,
                            {217, 220, 223, 255});
        }
    }

    void DrawDialogue() const
    {
        DrawUiPanel("dialogue-panel", {18, 486, ScreenWidth - 36, 154},
                    {28, 34, 43, 248});
        assets.DrawText(dialogueSpeaker.c_str(), 42, 503, 21, GOLD);
        if (dialogueLine < static_cast<int>(activeDialogue.size()))
        {
            DrawWrappedText(assets, activeDialogue[static_cast<std::size_t>(dialogueLine)],
                            42, 540, ScreenWidth - 84, 18, RAYWHITE);
        }
        assets.DrawText("Enter / Space: Continue      Esc: Close",
                        ScreenWidth - 356, 610, 15, {190, 196, 203, 255});
    }

    void DrawInventory() const
    {
        DrawUiPanel("wood-panel", {170, 120, 620, 480}, {27, 33, 42, 250});
        assets.DrawText("Inventory", 204, 148, 29, GOLD);
        assets.DrawText("Up / Down: Select     Enter: Use     I / Esc: Close",
                        204, 188, 16, {195, 201, 208, 255});

        const std::vector<InventoryEntry> &items = inventory.Entries();
        if (items.empty())
        {
            assets.DrawText("Your inventory is empty.", 208, 250, 20, RAYWHITE);
            return;
        }

        for (std::size_t index = 0; index < items.size(); ++index)
        {
            const int y = 238 + static_cast<int>(index) * 50;
            if (static_cast<int>(index) == selectedInventoryItem)
            {
                DrawRectangle(198, y - 5, 550, 40, Fade({194, 170, 112, 255}, 0.22f));
            }
            const ItemDefinition &item = GetItemDefinition(items[index].item);
            const char *textureKey = ItemTextureKey(items[index].item);
            const Texture2D *icon = assets.GetTexture(textureKey);
            if (icon != nullptr)
            {
                assets.DrawRegion(textureKey,
                                  {0, 0, static_cast<float>(icon->width),
                                   static_cast<float>(icon->height)},
                                  {212, static_cast<float>(y - 2), 28, 28});
            }
            assets.DrawText(TextFormat("%s  x%d", item.name, items[index].quantity),
                            250, y, 20,
                            static_cast<int>(index) == selectedInventoryItem ? GOLD : RAYWHITE);
        }

        const int selected = std::max(0, std::min(selectedInventoryItem,
                                                  static_cast<int>(items.size()) - 1));
        const ItemDefinition &item = GetItemDefinition(items[static_cast<std::size_t>(selected)].item);
        DrawLine(204, 425, 756, 425, Fade(RAYWHITE, 0.25f));
        assets.DrawText(item.name, 208, 448, 20, GOLD);
        DrawWrappedText(assets, item.description, 208, 482, 535, 17, RAYWHITE);
    }

    void DrawBattle() const
    {
        DrawUiPanel("wood-panel", {65, 102, ScreenWidth - 130, 520},
                    {31, 31, 43, 248});
        assets.DrawText(battleEnemyName.c_str(), 118, 130, 28, RAYWHITE);
        const std::string enemyTexture = battleEnemyIsBoss ? "guardian" : "slime";
        const int enemyFrame = battleEnemyIsBoss
                                   ? static_cast<int>(animationTimer * 5.0f) % 5
                                   : static_cast<int>(animationTimer * 6.0f) % 4;
        DrawSpriteFrame(enemyTexture, battleEnemyIsBoss ? 5 : 4,
                        battleEnemyIsBoss ? 1 : 4, enemyFrame, 0,
                        {static_cast<float>(ScreenWidth - 300), 165,
                         battleEnemyIsBoss ? 140.0f : 100.0f,
                         battleEnemyIsBoss ? 140.0f : 100.0f});
        DrawCharacterFrame("player", 0, playerFacingRow, {450, 185, 96, 96});
        if (assets.GetTexture(enemyTexture) == nullptr)
        {
            DrawCircle(ScreenWidth - 214, 233, battleEnemyIsBoss ? 68 : 48,
                       battleEnemyIsBoss ? Color{159, 78, 91, 255}
                                         : Color{121, 186, 91, 255});
        }
        assets.DrawText(TextFormat("HP %d / %d", battleEnemy.hp, battleEnemy.maxHp),
                        118, 176, 18, RAYWHITE);
        DrawRectangle(118, 204, 256, 16, {73, 76, 80, 255});
        DrawRectangle(118, 204,
                      static_cast<int>(256.0f * battleEnemy.hp / battleEnemy.maxHp),
                      16, {176, 72, 76, 255});
        assets.DrawText(TextFormat("Your HP: %d / %d", playerCombat.hp, playerCombat.maxHp),
                        118, 254, 19, RAYWHITE);
        DrawRectangle(118, 282, 256, 16, {73, 76, 80, 255});
        DrawRectangle(118, 282,
                      static_cast<int>(256.0f * playerCombat.hp / playerCombat.maxHp),
                      16, {176, 72, 76, 255});
        assets.DrawText(TextFormat("Healing Potions: %d", inventory.Count(ItemId::HealingPotion)),
                        118, 322, 17, {205, 209, 214, 255});

        DrawLine(112, 359, ScreenWidth - 112, 359, Fade(RAYWHITE, 0.22f));
        if (battleMessageTimer > 0.0f)
        {
            DrawWrappedText(assets, battleMessage, 118, 378,
                            ScreenWidth - 236, 18, RAYWHITE);
        }
        else
        {
            static const char *actions[] = {"Attack", "Use Healing Potion", "Flee"};
            for (int index = 0; index < 3; ++index)
            {
                const int y = 385 + index * 42;
                if (index == selectedBattleAction)
                {
                    DrawRectangle(110, y - 5, ScreenWidth - 220, 35,
                                  Fade({194, 170, 112, 255}, 0.22f));
                }
                assets.DrawText(TextFormat("%d. %s", index + 1, actions[index]), 124, y, 19,
                                index == selectedBattleAction ? GOLD : RAYWHITE);
            }
            assets.DrawText("Up / Down + Enter, or press 1 / 2 / 3",
                            118, 535, 16, {194, 200, 206, 255});
        }
    }

    void DrawEndScreen(bool victory) const
    {
        DrawRectangle(0, 0, ScreenWidth, ScreenHeight, Fade(BLACK, 0.78f));
        const char *title = victory ? "VICTORY" : "GAME OVER";
        const char *message = victory ? "The Sun Guardian has fallen. Willowmere is safe."
                                      : "Your journey ends here, but you can try again.";
        const int titleWidth = assets.MeasureText(title, 48);
        assets.DrawText(title, (ScreenWidth - titleWidth) / 2, 245, 48,
                        victory ? GOLD : Color{224, 108, 108, 255});
        assets.DrawText(message, 166, 325, 20, RAYWHITE);
        assets.DrawText("Press R to start a new adventure", 318, 390, 18,
                        {205, 211, 217, 255});
    }
};
} // namespace

int main()
{
    InitWindow(ScreenWidth, ScreenHeight, "Willowmere - A Tiny Adventure RPG");
    SetTargetFPS(60);
    InitAudioDevice();

    {
        AdventureGame game;
        while (!WindowShouldClose())
        {
            game.Update(GetFrameTime());
            game.Draw();
        }
    }

    if (IsAudioDeviceReady())
    {
        CloseAudioDevice();
    }
    CloseWindow();
    return 0;
}
