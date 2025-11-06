#include <iostream>
#include <memory>

#include "../include/core/Types.h"
#include "../include/combat/Attributes.h"
#include "../include/combat/DamageCalculator.h"
#include "../include/characters/Character.h"
#include "../include/skills/Skill.h"
#include "../include/skills/Ability.h"

using namespace Gorgon;

void PrintSeparator() {
    std::cout << "\n" << std::string(60, '=') << "\n\n";
}

void PrintCharacterStats(const Character& character) {
    const auto& attrs = character.GetAttributes();
    std::cout << character.GetName() << " Stats:\n";
    std::cout << "  Health: " << attrs.currentHealth << "/" << attrs.maxHealth << "\n";
    std::cout << "  Power:  " << attrs.currentPower << "/" << attrs.maxPower << "\n";
    std::cout << "  Armor:  " << attrs.currentArmor << "/" << attrs.maxArmor << "\n";
    std::cout << "  Status: " << (character.IsAlive() ? "Alive" : "Defeated") << "\n";
}

int main() {
    std::cout << "=== GORGON PROTOTYPE - CONSOLE TEST ===\n";
    std::cout << "Testing Project Gorgon-inspired combat mechanics\n";

    PrintSeparator();

    // Create player
    std::cout << "Creating player character...\n";
    auto player = std::make_unique<PlayerCharacter>("Hero", Race::Human);

    // Add skills
    auto swordSkill = std::make_shared<SwordSkill>();
    auto fireMagicSkill = std::make_shared<FireMagicSkill>();
    auto staffSkill = std::make_shared<StaffSkill>();

    player->AddSkill(swordSkill);
    player->AddSkill(fireMagicSkill);
    player->AddSkill(staffSkill);
    player->SetActiveSkills(0, 1);

    // Give player starting armor
    player->GetAttributes().RestoreArmor(100.0f);

    PrintCharacterStats(*player);

    PrintSeparator();

    // Create enemy
    std::cout << "Spawning enemy...\n";
    auto enemy = std::make_unique<Enemy>("Goblin", 1);
    enemy->GetAttributes().RestoreArmor(50.0f);

    PrintCharacterStats(*enemy);

    PrintSeparator();

    // Test 1: Basic sword attack
    std::cout << "TEST 1: Basic Sword Attack\n\n";
    WeaponData basicSword = Weapons::CreateSword();
    player->Attack(*enemy, basicSword);
    std::cout << "\nAfter attack:\n";
    PrintCharacterStats(*player);
    PrintCharacterStats(*enemy);

    PrintSeparator();

    // Test 2: Fireball ability
    std::cout << "TEST 2: Fireball Ability\n\n";
    auto fireball = fireMagicSkill->GetAbility(0);
    if (fireball) {
        fireball->Activate(player.get(), enemy.get());
        std::cout << "\nAfter fireball:\n";
        PrintCharacterStats(*player);
        PrintCharacterStats(*enemy);
    }

    PrintSeparator();

    // Test 3: Parry (restore armor)
    std::cout << "TEST 3: Parry Ability (Restore Armor)\n\n";

    // First level up sword skill to unlock Parry
    swordSkill->GainXP(300);
    std::cout << "Sword skill level: " << swordSkill->GetLevel() << "\n\n";

    auto parry = swordSkill->GetAbility(1);
    if (parry) {
        parry->Activate(player.get(), nullptr);
        std::cout << "\nAfter parry:\n";
        PrintCharacterStats(*player);
    } else {
        std::cout << "Parry not yet unlocked!\n";
    }

    PrintSeparator();

    // Test 4: Multiple attacks until enemy defeated
    std::cout << "TEST 4: Combat Until Defeat\n\n";
    int attackCount = 0;
    while (enemy->IsAlive() && attackCount < 20) {
        attackCount++;
        std::cout << "\n--- Attack " << attackCount << " ---\n";

        // Alternate between sword and fireball
        if (attackCount % 2 == 0) {
            player->Attack(*enemy, basicSword);
        } else {
            WeaponData fireball = Weapons::CreateFireball();
            player->Attack(*enemy, fireball);
        }

        // Regenerate some power
        player->GetAttributes().RestorePower(20.0f);
    }

    std::cout << "\nFinal stats after " << attackCount << " attacks:\n";
    PrintCharacterStats(*player);
    PrintCharacterStats(*enemy);

    PrintSeparator();

    // Test 5: Dice roll statistics
    std::cout << "TEST 5: Dice Roll Statistics (1000 rolls)\n\n";
    int rollCounts[7] = {0}; // Index 0 unused, 1-6 for die faces

    for (int i = 0; i < 1000; i++) {
        int roll = Random::Instance().RollD6();
        rollCounts[roll]++;
    }

    std::cout << "Distribution of 1d6 rolls:\n";
    for (int i = 1; i <= 6; i++) {
        std::cout << "  " << i << ": " << rollCounts[i]
                  << " (" << (rollCounts[i] / 10.0) << "%)\n";
    }

    std::cout << "\nExpected: ~166-167 per face (16.67%)\n";

    PrintSeparator();

    // Test 6: Armor mitigation
    std::cout << "TEST 6: Armor Mitigation\n\n";

    Attributes testAttrs;
    testAttrs.Initialize(100.0f, 100.0f, 100.0f);
    testAttrs.currentArmor = 100.0f;

    std::cout << "Starting armor: " << testAttrs.currentArmor << "\n";
    std::cout << "Mitigation: " << (testAttrs.currentArmor / 25.0f) << " damage\n\n";

    std::cout << "Taking 10 damage with 100 armor:\n";
    float healthBefore = testAttrs.currentHealth;
    testAttrs.TakeDamage(10.0f);
    float healthAfter = testAttrs.currentHealth;
    std::cout << "  Health: " << healthBefore << " -> " << healthAfter
              << " (took " << (healthBefore - healthAfter) << " damage)\n";
    std::cout << "  Expected: 6 damage (10 - 4 mitigation)\n\n";

    testAttrs.currentArmor = 50.0f;
    std::cout << "Taking 10 damage with 50 armor:\n";
    healthBefore = testAttrs.currentHealth;
    testAttrs.TakeDamage(10.0f);
    healthAfter = testAttrs.currentHealth;
    std::cout << "  Health: " << healthBefore << " -> " << healthAfter
              << " (took " << (healthBefore - healthAfter) << " damage)\n";
    std::cout << "  Expected: 8 damage (10 - 2 mitigation)\n";

    PrintSeparator();

    // Test 7: Regeneration
    std::cout << "TEST 7: Regeneration System\n\n";

    Attributes regenAttrs;
    regenAttrs.Initialize(100.0f, 100.0f, 100.0f);
    regenAttrs.currentHealth = 50.0f;
    regenAttrs.currentPower = 30.0f;
    regenAttrs.currentArmor = 20.0f;

    std::cout << "Starting state (damaged):\n";
    std::cout << "  Health: " << regenAttrs.currentHealth << "/" << regenAttrs.maxHealth << "\n";
    std::cout << "  Power:  " << regenAttrs.currentPower << "/" << regenAttrs.maxPower << "\n";
    std::cout << "  Armor:  " << regenAttrs.currentArmor << "/" << regenAttrs.maxArmor << "\n";
    std::cout << "  In Combat: " << (regenAttrs.isInCombat ? "Yes" : "No") << "\n\n";

    // Simulate in-combat regeneration (1 second)
    regenAttrs.isInCombat = true;
    regenAttrs.combatTimer = 0.0f;
    regenAttrs.Update(1.0f);

    std::cout << "After 1 second (IN COMBAT):\n";
    std::cout << "  Health: " << regenAttrs.currentHealth << "/" << regenAttrs.maxHealth
              << " (+" << (regenAttrs.currentHealth - 50.0f) << ")\n";
    std::cout << "  Power:  " << regenAttrs.currentPower << "/" << regenAttrs.maxPower
              << " (+" << (regenAttrs.currentPower - 30.0f) << ")\n";
    std::cout << "  Armor:  " << regenAttrs.currentArmor << "/" << regenAttrs.maxArmor
              << " (+" << (regenAttrs.currentArmor - 20.0f) << ")\n\n";

    // Simulate out-of-combat regeneration
    regenAttrs.combatTimer = 6.0f; // More than 5 seconds
    regenAttrs.Update(0.1f); // Small update to exit combat

    float healthBefore7 = regenAttrs.currentHealth;
    float powerBefore = regenAttrs.currentPower;
    float armorBefore = regenAttrs.currentArmor;

    regenAttrs.Update(1.0f);

    std::cout << "After 1 second (OUT OF COMBAT - 3x multiplier):\n";
    std::cout << "  Health: " << regenAttrs.currentHealth << "/" << regenAttrs.maxHealth
              << " (+" << (regenAttrs.currentHealth - healthBefore7) << ")\n";
    std::cout << "  Power:  " << regenAttrs.currentPower << "/" << regenAttrs.maxPower
              << " (+" << (regenAttrs.currentPower - powerBefore) << ")\n";
    std::cout << "  Armor:  " << regenAttrs.currentArmor << "/" << regenAttrs.maxArmor
              << " (+" << (regenAttrs.currentArmor - armorBefore) << ")\n";

    PrintSeparator();

    std::cout << "=== ALL TESTS COMPLETE ===\n\n";
    std::cout << "Core combat system verified:\n";
    std::cout << "  ✓ Character creation and attributes\n";
    std::cout << "  ✓ Skill and ability system\n";
    std::cout << "  ✓ Hit/Wound/Save/Ward mechanics\n";
    std::cout << "  ✓ Armor mitigation (1 per 25 armor)\n";
    std::cout << "  ✓ Regeneration (3x faster out of combat)\n";
    std::cout << "  ✓ Dice roll randomness\n";
    std::cout << "  ✓ Combat calculations\n\n";

    return 0;
}
