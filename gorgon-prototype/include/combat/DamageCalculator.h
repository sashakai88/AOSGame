#pragma once

#include "../core/Types.h"
#include "Attributes.h"
#include <string>

namespace Gorgon {

// Weapon data structure
struct WeaponData {
    std::string name = "Unarmed";
    int attacks = 1;         // Number of attacks
    int hitTarget = 4;       // Target number to hit (4+ means roll 4 or higher)
    int woundTarget = 4;     // Target number to wound
    int rend = 0;            // Rend modifier (makes saves harder)
    int damage = 1;          // Damage per successful attack
    float range = 150.0f;    // Range in cm (150 = melee)

    // Critical hit abilities
    bool hasCrit2Hits = false;      // Crit (2 Hits): One attack becomes two
    bool hasCritAutoWound = false;  // Crit (Auto-wound): Skip wound roll
    bool hasCritMortalWound = false; // Crit (Mortal Wound): Extra damage on failed save

    DamageType damageType = DamageType::Crushing;
};

// Combat calculator implementing Project Gorgon mechanics
class DamageCalculator {
public:
    // Calculate a full attack sequence
    static CombatResult CalculateAttack(
        const Attributes& attacker,
        Attributes& defender,
        const WeaponData& weapon,
        bool hasCharged = false,
        bool hasInspired = false
    ) {
        CombatResult result;

        // Process each attack
        for (int i = 0; i < weapon.attacks; i++) {
            ProcessSingleAttack(result, attacker, defender, weapon, hasCharged, hasInspired);
        }

        // Apply total damage to defender
        if (result.damage > 0) {
            defender.TakeDamage(static_cast<float>(result.damage));
        }

        return result;
    }

private:
    // Process a single attack in the sequence
    static void ProcessSingleAttack(
        CombatResult& result,
        const Attributes& attacker,
        Attributes& defender,
        const WeaponData& weapon,
        bool hasCharged,
        bool hasInspired
    ) {
        // Step 1: Hit Roll (1d6)
        int hitRoll = Random::Instance().RollD6();
        int modifiedHitRoll = hitRoll;

        // Modifiers
        if (hasCharged) modifiedHitRoll++; // +1 to hit if charged
        if (hasInspired) modifiedHitRoll++; // +1 to hit if inspired

        // Auto-miss on unmodified 1
        if (hitRoll == 1) {
            result.hitsMissed++;
            result.AddMessage("Miss (rolled 1)");
            return;
        }

        // Check if hit succeeds
        if (modifiedHitRoll < weapon.hitTarget) {
            result.hitsMissed++;
            result.AddMessage("Miss (needed " + std::to_string(weapon.hitTarget) + "+, rolled " + std::to_string(modifiedHitRoll) + ")");
            return;
        }

        // Hit succeeded
        result.hitsMade++;
        bool isCritical = (hitRoll == 6); // Unmodified 6 is critical

        // Critical Hit: 2 Hits ability
        int hitsToProcess = 1;
        if (isCritical && weapon.hasCrit2Hits) {
            hitsToProcess = 2;
            result.wasCritical = true;
            result.AddMessage("Critical Hit! (2 Hits)");
        }

        // Process each hit
        for (int hit = 0; hit < hitsToProcess; hit++) {
            ProcessWoundSequence(result, defender, weapon, isCritical && weapon.hasCritAutoWound);
        }
    }

    // Process wound, save, and ward sequence
    static void ProcessWoundSequence(
        CombatResult& result,
        Attributes& defender,
        const WeaponData& weapon,
        bool autoWound
    ) {
        // Step 2: Wound Roll (1d6) - skip if Auto-wound
        bool wounded = false;

        if (autoWound) {
            wounded = true;
            result.wounds++;
            result.AddMessage("Auto-wound (Critical)");
        } else {
            int woundRoll = Random::Instance().RollD6();

            // Auto-fail on unmodified 1
            if (woundRoll == 1) {
                result.AddMessage("Failed to wound (rolled 1)");
                return;
            }

            // Check if wound succeeds
            if (woundRoll >= weapon.woundTarget) {
                wounded = true;
                result.wounds++;
                result.AddMessage("Wounded (needed " + std::to_string(weapon.woundTarget) + "+, rolled " + std::to_string(woundRoll) + ")");
            } else {
                result.AddMessage("Failed to wound (needed " + std::to_string(weapon.woundTarget) + "+, rolled " + std::to_string(woundRoll) + ")");
                return;
            }
        }

        // Step 3: Save Roll (1d6)
        if (wounded) {
            int saveRoll = Random::Instance().RollD6();
            int modifiedSaveTarget = defender.saveValue + weapon.rend;

            // Auto-fail on unmodified 1
            if (saveRoll == 1) {
                result.AddMessage("Save failed (rolled 1)");
                // Continue to ward save if available
            } else if (saveRoll >= modifiedSaveTarget) {
                result.saves++;
                result.AddMessage("Saved! (needed " + std::to_string(modifiedSaveTarget) + "+, rolled " + std::to_string(saveRoll) + ")");
                return; // Save successful, no damage
            } else {
                result.AddMessage("Save failed (needed " + std::to_string(modifiedSaveTarget) + "+, rolled " + std::to_string(saveRoll) + ")");
            }

            // Step 4: Ward Save (1d6) - only if defender has ward
            if (defender.wardValue > 0) {
                int wardRoll = Random::Instance().RollD6();

                if (wardRoll >= defender.wardValue) {
                    result.wardSaves++;
                    result.AddMessage("Ward Save! (needed " + std::to_string(defender.wardValue) + "+, rolled " + std::to_string(wardRoll) + ")");
                    return; // Ward successful, no damage
                } else {
                    result.AddMessage("Ward failed (needed " + std::to_string(defender.wardValue) + "+, rolled " + std::to_string(wardRoll) + ")");
                }
            }

            // Step 5: Apply Damage
            result.damage += weapon.damage;
            result.AddMessage("Dealt " + std::to_string(weapon.damage) + " damage!");
        }
    }
};

// Predefined weapons for testing
namespace Weapons {
    inline WeaponData CreateUnarmed() {
        WeaponData weapon;
        weapon.name = "Unarmed";
        weapon.attacks = 1;
        weapon.hitTarget = 4;
        weapon.woundTarget = 5;
        weapon.damage = 1;
        weapon.damageType = DamageType::Crushing;
        return weapon;
    }

    inline WeaponData CreateSword() {
        WeaponData weapon;
        weapon.name = "Basic Sword";
        weapon.attacks = 2;
        weapon.hitTarget = 4;
        weapon.woundTarget = 4;
        weapon.damage = 1;
        weapon.damageType = DamageType::Slashing;
        return weapon;
    }

    inline WeaponData CreateFireball() {
        WeaponData weapon;
        weapon.name = "Fireball";
        weapon.attacks = 1;
        weapon.hitTarget = 4;
        weapon.woundTarget = 3;
        weapon.rend = 1;
        weapon.damage = 2;
        weapon.range = 1800.0f; // 18 meters
        weapon.damageType = DamageType::Fire;
        weapon.hasCritAutoWound = true;
        return weapon;
    }

    inline WeaponData CreatePowerfulSword() {
        WeaponData weapon;
        weapon.name = "Power Sword";
        weapon.attacks = 3;
        weapon.hitTarget = 3;
        weapon.woundTarget = 3;
        weapon.rend = 1;
        weapon.damage = 2;
        weapon.damageType = DamageType::Slashing;
        weapon.hasCrit2Hits = true;
        return weapon;
    }
}

} // namespace Gorgon
