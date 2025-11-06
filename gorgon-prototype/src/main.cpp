#include <raylib.h>
#include <iostream>
#include <memory>
#include <vector>

#include "../include/core/Types.h"
#include "../include/combat/Attributes.h"
#include "../include/combat/DamageCalculator.h"
#include "../include/characters/Character.h"
#include "../include/skills/Skill.h"
#include "../include/skills/Ability.h"

using namespace Gorgon;

// Game state
struct GameState {
    std::unique_ptr<PlayerCharacter> player;
    std::unique_ptr<Enemy> enemy;
    float gameTime = 0.0f;
    bool showHelp = true;
};

// Draw UI elements
void DrawUI(const GameState& state) {
    const int screenWidth = GetScreenWidth();
    const int margin = 20;
    const int barWidth = 200;
    const int barHeight = 20;

    // Player stats (top left)
    int yPos = margin;
    DrawText("PLAYER", margin, yPos, 20, WHITE);
    yPos += 30;

    // Health bar
    float healthPercent = state.player->GetAttributes().GetHealthPercent();
    DrawText("Health:", margin, yPos, 16, WHITE);
    DrawRectangle(margin + 80, yPos, barWidth, barHeight, DARKGRAY);
    DrawRectangle(margin + 80, yPos, static_cast<int>(barWidth * healthPercent), barHeight, RED);
    DrawText(TextFormat("%.0f/%.0f",
        state.player->GetAttributes().currentHealth,
        state.player->GetAttributes().maxHealth),
        margin + 80 + barWidth + 10, yPos, 16, WHITE);
    yPos += 30;

    // Power bar
    float powerPercent = state.player->GetAttributes().GetPowerPercent();
    DrawText("Power:", margin, yPos, 16, WHITE);
    DrawRectangle(margin + 80, yPos, barWidth, barHeight, DARKGRAY);
    DrawRectangle(margin + 80, yPos, static_cast<int>(barWidth * powerPercent), barHeight, BLUE);
    DrawText(TextFormat("%.0f/%.0f",
        state.player->GetAttributes().currentPower,
        state.player->GetAttributes().maxPower),
        margin + 80 + barWidth + 10, yPos, 16, WHITE);
    yPos += 30;

    // Armor bar
    float armorPercent = state.player->GetAttributes().GetArmorPercent();
    DrawText("Armor:", margin, yPos, 16, WHITE);
    DrawRectangle(margin + 80, yPos, barWidth, barHeight, DARKGRAY);
    DrawRectangle(margin + 80, yPos, static_cast<int>(barWidth * armorPercent), barHeight, GOLD);
    DrawText(TextFormat("%.0f/%.0f",
        state.player->GetAttributes().currentArmor,
        state.player->GetAttributes().maxArmor),
        margin + 80 + barWidth + 10, yPos, 16, WHITE);
    yPos += 30;

    // Combat state
    if (state.player->IsInCombat()) {
        DrawText("IN COMBAT", margin, yPos, 16, RED);
    } else {
        DrawText("Out of Combat", margin, yPos, 16, GREEN);
    }

    // Enemy stats (top right)
    if (state.enemy && state.enemy->IsAlive()) {
        yPos = margin;
        DrawText("ENEMY", screenWidth - margin - 300, yPos, 20, WHITE);
        yPos += 30;

        // Enemy health
        float enemyHealthPercent = state.enemy->GetAttributes().GetHealthPercent();
        DrawText("Health:", screenWidth - margin - 300, yPos, 16, WHITE);
        DrawRectangle(screenWidth - margin - 220, yPos, barWidth, barHeight, DARKGRAY);
        DrawRectangle(screenWidth - margin - 220, yPos, static_cast<int>(barWidth * enemyHealthPercent), barHeight, RED);
        DrawText(TextFormat("%.0f/%.0f",
            state.enemy->GetAttributes().currentHealth,
            state.enemy->GetAttributes().maxHealth),
            screenWidth - margin - 220 + barWidth + 10, yPos, 16, WHITE);
        yPos += 30;

        // Enemy armor
        float enemyArmorPercent = state.enemy->GetAttributes().GetArmorPercent();
        DrawText("Armor:", screenWidth - margin - 300, yPos, 16, WHITE);
        DrawRectangle(screenWidth - margin - 220, yPos, barWidth, barHeight, DARKGRAY);
        DrawRectangle(screenWidth - margin - 220, yPos, static_cast<int>(barWidth * enemyArmorPercent), barHeight, GOLD);
        DrawText(TextFormat("%.0f/%.0f",
            state.enemy->GetAttributes().currentArmor,
            state.enemy->GetAttributes().maxArmor),
            screenWidth - margin - 220 + barWidth + 10, yPos, 16, WHITE);
    } else {
        DrawText("ENEMY DEFEATED", screenWidth - margin - 300, margin, 20, GREEN);
        DrawText("Press R to spawn new enemy", screenWidth - margin - 400, margin + 30, 16, WHITE);
    }

    // Controls (bottom)
    if (state.showHelp) {
        yPos = GetScreenHeight() - 200;
        DrawText("CONTROLS:", margin, yPos, 20, YELLOW);
        yPos += 30;
        DrawText("1 - Sword Slash", margin, yPos, 16, WHITE);
        yPos += 20;
        DrawText("2 - Fireball", margin, yPos, 16, WHITE);
        yPos += 20;
        DrawText("3 - Parry (restore armor)", margin, yPos, 16, WHITE);
        yPos += 20;
        DrawText("4 - Healing Mist", margin, yPos, 16, WHITE);
        yPos += 20;
        DrawText("R - Respawn enemy", margin, yPos, 16, WHITE);
        yPos += 20;
        DrawText("H - Toggle help", margin, yPos, 16, WHITE);
        yPos += 20;
        DrawText("ESC - Quit", margin, yPos, 16, WHITE);
    } else {
        DrawText("Press H for help", margin, GetScreenHeight() - 40, 16, LIGHTGRAY);
    }
}

// Draw 3D scene
void Draw3DScene(const GameState& state) {
    BeginMode3D(Camera3D{
        {0.0f, 10.0f, 10.0f},  // Camera position
        {0.0f, 0.0f, 0.0f},     // Camera target
        {0.0f, 1.0f, 0.0f},     // Camera up
        45.0f,                   // FOV
        CAMERA_PERSPECTIVE       // Projection type
    });

    // Draw ground
    DrawPlane({0.0f, 0.0f, 0.0f}, {20.0f, 20.0f}, DARKGREEN);
    DrawGrid(20, 1.0f);

    // Draw player
    if (state.player->IsAlive()) {
        DrawCube({-3.0f, 1.0f, 0.0f}, 2.0f, 2.0f, 2.0f, BLUE);
        DrawCubeWires({-3.0f, 1.0f, 0.0f}, 2.0f, 2.0f, 2.0f, DARKBLUE);
    }

    // Draw enemy
    if (state.enemy && state.enemy->IsAlive()) {
        DrawCube({3.0f, 1.0f, 0.0f}, 2.0f, 2.0f, 2.0f, RED);
        DrawCubeWires({3.0f, 1.0f, 0.0f}, 2.0f, 2.0f, 2.0f, DARKBROWN);
    }

    EndMode3D();
}

// Initialize game state
void InitGame(GameState& state) {
    // Create player
    state.player = std::make_unique<PlayerCharacter>("Hero", Race::Human);

    // Add skills to player
    auto swordSkill = std::make_shared<SwordSkill>();
    auto fireMagicSkill = std::make_shared<FireMagicSkill>();
    auto staffSkill = std::make_shared<StaffSkill>();

    state.player->AddSkill(swordSkill);
    state.player->AddSkill(fireMagicSkill);
    state.player->AddSkill(staffSkill);

    // Set active skills (Sword and Fire Magic)
    state.player->SetActiveSkills(0, 1);

    // Give player some starting armor
    state.player->GetAttributes().RestoreArmor(100.0f);

    // Create enemy
    state.enemy = std::make_unique<Enemy>("Goblin", 1);
    state.enemy->GetAttributes().RestoreArmor(50.0f);

    std::cout << "\n=== GORGON PROTOTYPE ===\n";
    std::cout << "Player created: " << state.player->GetName() << "\n";
    std::cout << "Enemy spawned: " << state.enemy->GetName() << "\n\n";
}

// Handle input
void HandleInput(GameState& state) {
    if (!state.player->IsAlive()) return;
    if (!state.enemy || !state.enemy->IsAlive()) return;

    auto skill1 = state.player->GetActiveSkill(0); // Sword
    auto skill2 = state.player->GetActiveSkill(1); // Fire Magic
    auto skill3 = state.player->GetActiveSkill(2); // Staff

    // Ability 1 - Sword Slash
    if (IsKeyPressed(KEY_ONE) && skill1) {
        auto ability = skill1->GetAbility(0);
        if (ability && ability->CanActivate(state.player.get(), state.enemy.get())) {
            ability->Activate(state.player.get(), state.enemy.get());
            skill1->GainXP(10);
        } else {
            std::cout << "Cannot use Sword Slash (on cooldown or not enough power)\n";
        }
    }

    // Ability 2 - Fireball
    if (IsKeyPressed(KEY_TWO) && skill2) {
        auto ability = skill2->GetAbility(0);
        if (ability && ability->CanActivate(state.player.get(), state.enemy.get())) {
            ability->Activate(state.player.get(), state.enemy.get());
            skill2->GainXP(15);
        } else {
            std::cout << "Cannot use Fireball (on cooldown or not enough power)\n";
        }
    }

    // Ability 3 - Parry
    if (IsKeyPressed(KEY_THREE) && skill1) {
        auto ability = skill1->GetAbility(1); // Parry (level 3 skill)
        if (ability && ability->CanActivate(state.player.get(), nullptr)) {
            ability->Activate(state.player.get(), nullptr);
            skill1->GainXP(12);
        } else {
            std::cout << "Cannot use Parry (not unlocked, on cooldown, or not enough power)\n";
        }
    }

    // Ability 4 - Healing Mist
    if (IsKeyPressed(KEY_FOUR)) {
        if (skill3) {
            auto ability = skill3->GetAbility(1); // Healing Mist (level 4 skill)
            if (ability && ability->CanActivate(state.player.get(), state.player.get())) {
                ability->Activate(state.player.get(), state.player.get());
            } else {
                std::cout << "Cannot use Healing Mist (not unlocked, on cooldown, or not enough power)\n";
            }
        }
    }

    // Respawn enemy
    if (IsKeyPressed(KEY_R)) {
        state.enemy = std::make_unique<Enemy>("Goblin", 1);
        state.enemy->GetAttributes().RestoreArmor(50.0f);
        std::cout << "\nNew enemy spawned!\n\n";
    }

    // Toggle help
    if (IsKeyPressed(KEY_H)) {
        state.showHelp = !state.showHelp;
    }
}

// Update game logic
void UpdateGame(GameState& state, float deltaTime) {
    state.gameTime += deltaTime;

    // Update player
    state.player->Update(deltaTime);

    // Update player skills
    for (auto& skill : state.player->GetSkills()) {
        skill->Update(deltaTime);
    }

    // Update enemy
    if (state.enemy && state.enemy->IsAlive()) {
        state.enemy->Update(deltaTime);
        state.enemy->UpdateAI(deltaTime, state.player.get());
    }
}

// Main entry point
int main() {
    // Initialize window
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Gorgon Prototype - Project Gorgon Combat System");
    SetTargetFPS(60);

    // Initialize game
    GameState state;
    InitGame(state);

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // Handle input
        HandleInput(state);

        // Update game
        UpdateGame(state, deltaTime);

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw 3D scene
        Draw3DScene(state);

        // Draw UI overlay
        DrawUI(state);

        // Draw FPS
        DrawFPS(10, 10);

        EndDrawing();
    }

    // Cleanup
    CloseWindow();

    std::cout << "\n=== GAME ENDED ===\n";
    std::cout << "Total game time: " << state.gameTime << " seconds\n";

    return 0;
}
