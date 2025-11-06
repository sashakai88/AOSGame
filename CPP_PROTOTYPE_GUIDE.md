# C++ Prototype Implementation Guide
## Project Gorgon-Inspired Game - Pure C++ Prototype

Let's build a working prototype in pure C++ with a simple 3D framework before committing to a full engine.

---

## Why Start Simple?

**Benefits:**
- ✅ Focus on game logic, not engine complexity
- ✅ Understand systems deeply before scaling
- ✅ Easy to iterate and test ideas
- ✅ Lightweight and fast to compile
- ✅ Can port to any engine later

**What We'll Use:**
- **Raylib** - Simple, powerful, cross-platform 3D framework
- **C++17** - Modern C++ features
- **CMake** - Cross-platform build system
- **nlohmann/json** - JSON parsing for data files

---

## Table of Contents

1. [Setup & Dependencies](#1-setup--dependencies)
2. [Project Structure](#2-project-structure)
3. [Core Systems](#3-core-systems)
4. [Combat Implementation](#4-combat-implementation)
5. [Skill System](#5-skill-system)
6. [First Playable](#6-first-playable)

---

## 1. Setup & Dependencies

### 1.1 Install Dependencies

**Windows:**
```bash
# Install vcpkg (package manager)
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat

# Install libraries
./vcpkg install raylib nlohmann-json
```

**Linux:**
```bash
sudo apt-get install build-essential cmake
sudo apt-get install libraylib-dev
sudo apt-get install nlohmann-json3-dev
```

**macOS:**
```bash
brew install cmake raylib nlohmann-json
```

### 1.2 Project Setup

```bash
mkdir gorgon-prototype
cd gorgon-prototype
mkdir -p src/{core,combat,skills,characters,world}
mkdir -p include/{core,combat,skills,characters,world}
mkdir -p data/{skills,abilities,equipment,npcs}
mkdir assets/{models,textures,sounds}
mkdir build
```

---

## 2. Project Structure

```
gorgon-prototype/
├── CMakeLists.txt
├── README.md
├── data/
│   ├── skills.json
│   ├── abilities.json
│   └── races.json
├── include/
│   ├── core/
│   │   ├── Game.h
│   │   └── Types.h
│   ├── combat/
│   │   ├── CombatSystem.h
│   │   ├── Attributes.h
│   │   └── DamageCalculator.h
│   ├── skills/
│   │   ├── Skill.h
│   │   ├── Ability.h
│   │   └── SkillManager.h
│   └── characters/
│       ├── Character.h
│       ├── Player.h
│       └── Enemy.h
├── src/
│   ├── main.cpp
│   ├── core/
│   │   └── Game.cpp
│   ├── combat/
│   │   ├── CombatSystem.cpp
│   │   └── DamageCalculator.cpp
│   ├── skills/
│   │   ├── Skill.cpp
│   │   └── Ability.cpp
│   └── characters/
│       ├── Character.cpp
│       ├── Player.cpp
│       └── Enemy.cpp
└── build/
```

---

## 3. Core Systems

### 3.1 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.15)
project(GorgonPrototype)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(raylib 4.0 REQUIRED)
find_package(nlohmann_json 3.11 REQUIRED)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    raylib
    nlohmann_json::nlohmann_json
)

# Copy data directory to build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
)
```

### 3.2 Core Types (include/core/Types.h)

```cpp
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <random>

namespace Gorgon {

// Random number generator
class Random {
public:
    static Random& Instance() {
        static Random instance;
        return instance;
    }

    int RollD6() {
        return distribution(generator);
    }

    int RollD3() {
        return (RollD6() + 1) / 2;
    }

    int Roll2D6() {
        return RollD6() + RollD6();
    }

private:
    Random() : generator(std::random_device{}()), distribution(1, 6) {}
    std::mt19937 generator;
    std::uniform_int_distribution<int> distribution;
};

// Damage types
enum class DamageType {
    Slashing,
    Piercing,
    Crushing,
    Fire,
    Cold,
    Electricity,
    Acid,
    Nature,
    Psychic,
    Darkness,
    Poison,
    Trauma
};

// Ability tags for proc effects
enum class AbilityTag {
    BasicAttack,
    NiceAttack,
    CoreAttack,
    EpicAttack,
    SignatureDebuff
};

// Combat result
struct CombatResult {
    int damage = 0;
    bool isCritical = false;
    bool isMiss = false;
    std::vector<std::string> messages;
};

} // namespace Gorgon
```

### 3.3 Game Loop (include/core/Game.h)

```cpp
#pragma once
#include "raylib.h"
#include <memory>
#include <vector>

namespace Gorgon {

class Character;
class Player;

class Game {
public:
    Game();
    ~Game();

    void Run();

private:
    void Initialize();
    void Update(float deltaTime);
    void Render();
    void Shutdown();

    void UpdateCamera();
    void RenderWorld();
    void RenderUI();

    Camera3D camera;
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Character>> enemies;

    bool isRunning = true;
    const int screenWidth = 1280;
    const int screenHeight = 720;
};

} // namespace Gorgon
```

---

## 4. Combat Implementation

### 4.1 Attributes System (include/combat/Attributes.h)

```cpp
#pragma once

namespace Gorgon {

struct Attributes {
    // Primary resources
    float currentHealth = 100.0f;
    float maxHealth = 100.0f;

    float currentPower = 100.0f;
    float maxPower = 100.0f;

    float currentArmor = 0.0f;
    float maxArmor = 200.0f;

    // Regeneration (per second)
    float healthRegen = 2.0f;
    float powerRegen = 5.0f;
    float armorRegen = 10.0f;

    // Combat stats
    int hitBonus = 0;
    int woundBonus = 0;
    int save = 4;           // 4+ save
    int ward = 0;           // 0 = no ward, 6+ etc

    // State
    bool isInCombat = false;
    float combatTimer = 0.0f;  // Time since last combat action

    // Methods
    void TakeDamage(float amount) {
        // Armor mitigation: 1 damage per 25 armor
        float mitigated = currentArmor / 25.0f;
        float actualDamage = std::max(0.0f, amount - mitigated);

        currentHealth = std::max(0.0f, currentHealth - actualDamage);

        EnterCombat();
    }

    void ConsumePower(float amount) {
        currentPower = std::max(0.0f, currentPower - amount);
    }

    void RestoreHealth(float amount) {
        currentHealth = std::min(maxHealth, currentHealth + amount);
    }

    void RestoreArmor(float amount) {
        currentArmor = std::min(maxArmor, currentArmor + amount);
    }

    void RestorePower(float amount) {
        currentPower = std::min(maxPower, currentPower + amount);
    }

    void EnterCombat() {
        isInCombat = true;
        combatTimer = 0.0f;
    }

    void Update(float deltaTime) {
        combatTimer += deltaTime;

        // Exit combat after 5 seconds of no activity
        if (combatTimer > 5.0f) {
            isInCombat = false;
        }

        // Regeneration (out of combat is faster)
        float regenMultiplier = isInCombat ? 1.0f : 3.0f;

        RestoreHealth(healthRegen * regenMultiplier * deltaTime);
        RestorePower(powerRegen * regenMultiplier * deltaTime);
        RestoreArmor(armorRegen * regenMultiplier * deltaTime);
    }

    bool IsAlive() const {
        return currentHealth > 0.0f;
    }

    float GetHealthPercent() const {
        return maxHealth > 0 ? (currentHealth / maxHealth) * 100.0f : 0.0f;
    }

    float GetPowerPercent() const {
        return maxPower > 0 ? (currentPower / maxPower) * 100.0f : 0.0f;
    }
};

} // namespace Gorgon
```

### 4.2 Damage Calculator (include/combat/DamageCalculator.h)

```cpp
#pragma once
#include "core/Types.h"
#include "Attributes.h"

namespace Gorgon {

struct WeaponData {
    std::string name;
    int attacks = 1;
    int hitTarget = 4;      // 4+ to hit
    int woundTarget = 4;    // 4+ to wound
    int rend = 0;           // Negative modifier to save
    int damage = 1;
    float range = 1.5f;     // meters

    // Special abilities
    bool hasCrit2Hits = false;
    bool hasCritAutoWound = false;
    bool hasChargeBonus = false;  // +1 damage if charged this turn
};

class DamageCalculator {
public:
    static CombatResult CalculateAttack(
        const Attributes& attacker,
        Attributes& defender,
        const WeaponData& weapon,
        bool hasCharged = false
    ) {
        CombatResult result;

        for (int i = 0; i < weapon.attacks; i++) {
            int hitRoll = Random::Instance().RollD6();

            // Auto miss on 1
            if (hitRoll == 1) {
                result.messages.push_back("Attack " + std::to_string(i+1) + ": Auto miss!");
                continue;
            }

            // Check if hit
            int hitTarget = weapon.hitTarget - attacker.hitBonus;
            if (hitRoll < hitTarget) {
                result.messages.push_back("Attack " + std::to_string(i+1) + ": Miss!");
                continue;
            }

            // Critical hit?
            bool isCrit = (hitRoll == 6);

            // Crit (2 Hits) doubles this attack
            int hits = (isCrit && weapon.hasCrit2Hits) ? 2 : 1;
            if (hits == 2) {
                result.isCritical = true;
                result.messages.push_back("CRITICAL! Crit (2 Hits)!");
            }

            for (int h = 0; h < hits; h++) {
                // Wound roll (skip if Crit Auto-wound)
                if (!(isCrit && weapon.hasCritAutoWound)) {
                    int woundRoll = Random::Instance().RollD6();

                    if (woundRoll == 1) {
                        result.messages.push_back("Wound: Auto fail!");
                        continue;
                    }

                    int woundTarget = weapon.woundTarget - attacker.woundBonus;
                    if (woundRoll < woundTarget) {
                        result.messages.push_back("Failed to wound!");
                        continue;
                    }
                } else {
                    result.isCritical = true;
                    result.messages.push_back("CRITICAL! Auto-wound!");
                }

                // Save roll
                int saveRoll = Random::Instance().RollD6();
                int modifiedSave = defender.save + weapon.rend;

                if (saveRoll != 1 && saveRoll >= modifiedSave) {
                    result.messages.push_back("Saved!");
                    continue;
                }

                // Ward save (if defender has one)
                if (defender.ward > 0) {
                    int wardRoll = Random::Instance().RollD6();
                    if (wardRoll >= defender.ward) {
                        result.messages.push_back("Ward save!");
                        continue;
                    }
                }

                // Calculate damage
                int damageAmount = weapon.damage;

                // Charge bonus
                if (hasCharged && weapon.hasChargeBonus) {
                    damageAmount += 1;
                    result.messages.push_back("Charge bonus: +1 damage!");
                }

                result.damage += damageAmount;
            }
        }

        if (result.damage > 0) {
            defender.TakeDamage(static_cast<float>(result.damage));
            result.messages.push_back("Total damage: " + std::to_string(result.damage));
        }

        return result;
    }
};

} // namespace Gorgon
```

---

## 5. Skill System

### 5.1 Skill Base (include/skills/Skill.h)

```cpp
#pragma once
#include <string>
#include <vector>
#include <memory>

namespace Gorgon {

class Ability;

enum class SkillType {
    Sword,
    FireMagic,
    IceMagic,
    Archery,
    Shield,
    Hammer,
    Staff,
    Unarmed,
    Psychology,
    Mentalism,
    Necromancy,
    // Add more as needed
};

class Skill {
public:
    Skill(SkillType type, const std::string& name)
        : type(type), name(name) {}

    virtual ~Skill() = default;

    // Level up the skill
    void GainXP(int amount) {
        xp += amount;
        while (xp >= XPForNextLevel() && level < maxLevel) {
            level++;
            xp -= XPForNextLevel();
            OnLevelUp();
        }
    }

    // Get abilities available at current level
    std::vector<Ability*> GetAvailableAbilities() const;

    // Getters
    int GetLevel() const { return level; }
    int GetXP() const { return xp; }
    int GetMaxLevel() const { return maxLevel; }
    SkillType GetType() const { return type; }
    const std::string& GetName() const { return name; }

protected:
    virtual void OnLevelUp() {}

    int XPForNextLevel() const {
        // XP curve: level * 100
        return level * 100;
    }

    SkillType type;
    std::string name;
    int level = 1;
    int xp = 0;
    int maxLevel = 50;

    std::vector<std::unique_ptr<Ability>> abilities;
};

} // namespace Gorgon
```

### 5.2 Ability System (include/skills/Ability.h)

```cpp
#pragma once
#include "core/Types.h"
#include <string>
#include <vector>

namespace Gorgon {

class Character;

class Ability {
public:
    Ability(const std::string& name, int powerCost, float cooldown)
        : name(name), powerCost(powerCost), maxCooldown(cooldown) {}

    virtual ~Ability() = default;

    // Check if can be activated
    virtual bool CanActivate(const Character* caster, const Character* target) const;

    // Activate the ability
    virtual void Activate(Character* caster, Character* target) = 0;

    // Update cooldown
    void Update(float deltaTime) {
        if (currentCooldown > 0) {
            currentCooldown -= deltaTime;
            if (currentCooldown < 0) currentCooldown = 0;
        }
    }

    // Start cooldown
    void StartCooldown() {
        currentCooldown = maxCooldown;
    }

    bool IsOnCooldown() const { return currentCooldown > 0; }
    float GetCooldownPercent() const {
        return maxCooldown > 0 ? (currentCooldown / maxCooldown) * 100.0f : 0.0f;
    }

    // Getters
    const std::string& GetName() const { return name; }
    int GetPowerCost() const { return powerCost; }
    float GetMaxCooldown() const { return maxCooldown; }
    float GetRange() const { return range; }
    const std::vector<AbilityTag>& GetTags() const { return tags; }

protected:
    std::string name;
    int powerCost;
    float maxCooldown;
    float currentCooldown = 0.0f;
    float range = 5.0f;  // meters
    std::vector<AbilityTag> tags;
};

// Example: Sword Slash ability
class SwordSlashAbility : public Ability {
public:
    SwordSlashAbility()
        : Ability("Sword Slash", 10, 3.0f) {
        range = 2.0f;
        tags.push_back(AbilityTag::NiceAttack);
    }

    void Activate(Character* caster, Character* target) override;
};

} // namespace Gorgon
```

---

## 6. First Playable

### 6.1 Main Game Loop (src/main.cpp)

```cpp
#include "core/Game.h"
#include <iostream>

int main() {
    try {
        Gorgon::Game game;
        game.Run();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
```

### 6.2 Game Implementation (src/core/Game.cpp)

```cpp
#include "core/Game.h"
#include "characters/Player.h"
#include "characters/Enemy.h"
#include <iostream>

namespace Gorgon {

Game::Game() {
    Initialize();
}

Game::~Game() {
    Shutdown();
}

void Game::Initialize() {
    // Initialize Raylib
    InitWindow(screenWidth, screenHeight, "Gorgon Chronicles - Prototype");
    SetTargetFPS(60);

    // Setup camera
    camera.position = Vector3{ 0.0f, 10.0f, 10.0f };
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Create player
    player = std::make_unique<Player>();
    player->position = Vector3{ 0.0f, 0.0f, 0.0f };

    // Create test enemy
    auto enemy = std::make_unique<Enemy>();
    enemy->position = Vector3{ 5.0f, 0.0f, 0.0f };
    enemies.push_back(std::move(enemy));

    std::cout << "Game initialized!" << std::endl;
}

void Game::Run() {
    while (!WindowShouldClose() && isRunning) {
        float deltaTime = GetFrameTime();

        Update(deltaTime);
        Render();
    }
}

void Game::Update(float deltaTime) {
    // Update player
    player->Update(deltaTime);

    // Update enemies
    for (auto& enemy : enemies) {
        enemy->Update(deltaTime);
    }

    // Update camera to follow player
    UpdateCamera();

    // Combat input (spacebar to attack nearest enemy)
    if (IsKeyPressed(KEY_SPACE) && !enemies.empty()) {
        player->AttackTarget(enemies[0].get());
    }
}

void Game::UpdateCamera() {
    // Follow player
    camera.target = player->position;
    camera.position = Vector3{
        player->position.x,
        player->position.y + 10.0f,
        player->position.z + 10.0f
    };
}

void Game::Render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
    RenderWorld();
    EndMode3D();

    RenderUI();

    EndDrawing();
}

void Game::RenderWorld() {
    // Draw ground plane
    DrawPlane(Vector3{ 0.0f, 0.0f, 0.0f }, Vector2{ 50.0f, 50.0f }, LIGHTGRAY);
    DrawGrid(50, 1.0f);

    // Draw player
    DrawCube(player->position, 1.0f, 2.0f, 1.0f, BLUE);
    DrawCubeWires(player->position, 1.0f, 2.0f, 1.0f, DARKBLUE);

    // Draw enemies
    for (const auto& enemy : enemies) {
        Color enemyColor = enemy->attributes.IsAlive() ? RED : GRAY;
        DrawCube(enemy->position, 1.0f, 2.0f, 1.0f, enemyColor);
        DrawCubeWires(enemy->position, 1.0f, 2.0f, 1.0f, DARKGRAY);
    }
}

void Game::RenderUI() {
    // Player stats
    int yPos = 10;
    DrawText("=== PLAYER ===", 10, yPos, 20, BLACK);
    yPos += 25;

    DrawText(TextFormat("Health: %.0f / %.0f",
        player->attributes.currentHealth,
        player->attributes.maxHealth), 10, yPos, 20, GREEN);
    yPos += 25;

    DrawText(TextFormat("Armor: %.0f", player->attributes.currentArmor), 10, yPos, 20, BLUE);
    yPos += 25;

    DrawText(TextFormat("Power: %.0f / %.0f",
        player->attributes.currentPower,
        player->attributes.maxPower), 10, yPos, 20, PURPLE);
    yPos += 25;

    // Combat status
    if (player->attributes.isInCombat) {
        DrawText("IN COMBAT", 10, yPos, 20, RED);
    } else {
        DrawText("Out of Combat", 10, yPos, 20, GREEN);
    }
    yPos += 35;

    // Enemy stats
    if (!enemies.empty()) {
        DrawText("=== ENEMY ===", 10, yPos, 20, BLACK);
        yPos += 25;

        auto& enemy = enemies[0];
        DrawText(TextFormat("Health: %.0f / %.0f",
            enemy->attributes.currentHealth,
            enemy->attributes.maxHealth), 10, yPos, 20, RED);
        yPos += 25;

        DrawText(TextFormat("Armor: %.0f", enemy->attributes.currentArmor), 10, yPos, 20, BLUE);
    }

    // Controls
    DrawText("Controls:", 10, screenHeight - 80, 20, BLACK);
    DrawText("WASD - Move | SPACE - Attack | ESC - Quit", 10, screenHeight - 55, 16, DARKGRAY);

    // FPS
    DrawFPS(screenWidth - 100, 10);
}

void Game::Shutdown() {
    CloseWindow();
}

} // namespace Gorgon
```

---

## Building and Running

### Build Steps

```bash
cd gorgon-prototype
mkdir build
cd build
cmake ..
cmake --build .

# Run
./GorgonPrototype  # Linux/Mac
GorgonPrototype.exe  # Windows
```

### Quick Start Script

**build.sh** (Linux/Mac):
```bash
#!/bin/bash
mkdir -p build
cd build
cmake ..
cmake --build .
./GorgonPrototype
```

**build.bat** (Windows):
```batch
@echo off
mkdir build
cd build
cmake ..
cmake --build .
GorgonPrototype.exe
```

---

## Next Steps

**Week 1:**
1. ✅ Set up project structure
2. ✅ Implement core types and random system
3. ✅ Build attributes system (Armor/Health/Power)
4. ✅ Implement damage calculator with Project Gorgon rules
5. ✅ Create basic game loop with Raylib

**Week 2:**
- Character classes (Player, Enemy)
- Movement system (WASD)
- Basic AI for enemies
- Skill system framework
- 2-3 abilities (Sword Slash, Fireball, etc.)

**Week 3:**
- Dual-skill system
- Cooldown management
- Power consumption
- Combat feedback (damage numbers, effects)
- Simple UI improvements

**Week 4:**
- Equipment system basics
- Treasure Effects framework
- Multiple enemies
- Targeting system
- Save/Load progress

---

## Advantages of This Approach

1. **Fast Iteration** - Compile and test in seconds
2. **Deep Understanding** - Build every system yourself
3. **Portable** - Can move to any engine later
4. **Lightweight** - No engine overhead
5. **Full Control** - Understand exactly how everything works

Once core systems are solid, we can port to:
- Unreal Engine (best graphics)
- Unity (easiest)
- Godot (open source)
- Custom engine (ultimate control)

---

**Ready to start coding?** Let's begin with the full implementation!
