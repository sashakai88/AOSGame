#pragma once

#include <algorithm>
#include <cmath>

namespace Gorgon {

// The core attribute system: Armor, Health, Power
struct Attributes {
    // Current values
    float currentHealth = 100.0f;
    float currentPower = 100.0f;
    float currentArmor = 0.0f;

    // Maximum values
    float maxHealth = 100.0f;
    float maxPower = 100.0f;
    float maxArmor = 200.0f;

    // Regeneration rates (per second)
    float healthRegen = 5.0f;    // 5 HP/sec
    float powerRegen = 10.0f;    // 10 Power/sec
    float armorRegen = 20.0f;    // 20 Armor/sec

    // Combat state
    bool isInCombat = false;
    float combatTimer = 0.0f;

    // Save value (for defense)
    int saveValue = 4; // Default 4+

    // Ward value (optional additional save)
    int wardValue = 0; // 0 = no ward

    // Vulnerability (takes extra damage)
    bool isVulnerable = false;

    // Initialize with race-specific values
    void Initialize(float health, float power, float armor) {
        maxHealth = health;
        maxPower = power;
        maxArmor = armor;

        currentHealth = maxHealth;
        currentPower = maxPower;
        currentArmor = maxArmor;
    }

    // Take damage (applies armor mitigation)
    void TakeDamage(float amount) {
        // Armor mitigation: 1 damage reduced per 25 armor
        float mitigation = currentArmor / 25.0f;
        float actualDamage = std::max(0.0f, amount - mitigation);

        // Apply vulnerability
        if (isVulnerable) {
            actualDamage *= 1.5f; // 50% more damage when vulnerable
        }

        currentHealth = std::max(0.0f, currentHealth - actualDamage);
        EnterCombat();
    }

    // Restore health
    void RestoreHealth(float amount) {
        currentHealth = std::min(maxHealth, currentHealth + amount);
    }

    // Restore power
    void RestorePower(float amount) {
        currentPower = std::min(maxPower, currentPower + amount);
    }

    // Restore armor
    void RestoreArmor(float amount) {
        currentArmor = std::min(maxArmor, currentArmor + amount);
    }

    // Consume power for abilities
    bool ConsumePower(float amount) {
        if (currentPower >= amount) {
            currentPower -= amount;
            return true;
        }
        return false;
    }

    // Reduce armor (can go negative)
    void ReduceArmor(float amount) {
        currentArmor -= amount;
        EnterCombat();
    }

    // Enter combat state (affects regeneration)
    void EnterCombat() {
        isInCombat = true;
        combatTimer = 0.0f;
    }

    // Check if character is alive
    bool IsAlive() const {
        return currentHealth > 0.0f;
    }

    // Check if character is at full health
    bool IsFullHealth() const {
        return currentHealth >= maxHealth;
    }

    // Update regeneration and combat state
    void Update(float deltaTime) {
        // Update combat timer
        combatTimer += deltaTime;

        // Exit combat after 5 seconds of no damage
        if (combatTimer > 5.0f) {
            isInCombat = false;
        }

        // Regeneration (3x faster out of combat)
        float regenMultiplier = isInCombat ? 1.0f : 3.0f;

        if (currentHealth < maxHealth) {
            RestoreHealth(healthRegen * regenMultiplier * deltaTime);
        }

        if (currentPower < maxPower) {
            RestorePower(powerRegen * regenMultiplier * deltaTime);
        }

        if (currentArmor < maxArmor) {
            RestoreArmor(armorRegen * regenMultiplier * deltaTime);
        }
    }

    // Get health percentage (0.0 to 1.0)
    float GetHealthPercent() const {
        return maxHealth > 0.0f ? currentHealth / maxHealth : 0.0f;
    }

    // Get power percentage (0.0 to 1.0)
    float GetPowerPercent() const {
        return maxPower > 0.0f ? currentPower / maxPower : 0.0f;
    }

    // Get armor percentage (0.0 to 1.0)
    float GetArmorPercent() const {
        return maxArmor > 0.0f ? currentArmor / maxArmor : 0.0f;
    }
};

} // namespace Gorgon
