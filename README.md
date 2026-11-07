# RPG Game Sample Project

A small top-down C++ game built with raylib. The test scene demonstrates a player, a pursuing boss, health, collision, and throwable projectiles.

## Build and run

Open `main.code-workspace` in VS Code and press **F5**. The Windows build task expects raylib and w64devkit under `C:/raylib`, as configured in `.vscode/tasks.json`. Alternatively, run:

```powershell
mingw32-make.exe RAYLIB_PATH=C:/raylib/raylib PROJECT_NAME=game "OBJS=src/*.cpp" BUILD_MODE=DEBUG
```

## Controls

- Move with **WASD** or the **arrow keys**.
- Throw in the last movement direction with **Z** or **Enter**.

## Entity architecture

Entities are assembled from plain components in `src/systems/components.hpp`. The scene creates the player, boss, and projectiles through `entity_factory.hpp`, then delegates input, movement, collision, projectile cleanup, and rendering to small systems. Components are stored directly on each entity, keeping the design suitable for this small game without adding an ECS dependency.
