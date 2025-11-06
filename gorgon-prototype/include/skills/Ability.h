#pragma once

#include "../core/Types.h"
#include "../combat/DamageCalculator.h"
#include <string>
#include <vector>

namespace Gorgon {

// Forward declaration
class Character;

// Ability tags (for Treasure Effect modifiers)
enum class AbilityTag {
    BasicAttack,
    NiceAttack,
    EpicAttack,
    Burst,
    Signature,
    AoE,
    Heal,
    Buff,
    Debuff
};

// Base Ability class
class Ability {
public:
    Ability(const std::string& name, int powerCost, float cooldown);
    virtual ~Ability() = default;

    // Basic info
    std::string GetName() const { return name; }
    std::string GetDescription() const { return description; }
    int GetPowerCost() const { return powerCost; }
    float GetCooldown() const { return cooldown; }
    float GetRange() const { return range; }

    // Tags
    void AddTag(AbilityTag tag) { tags.push_back(tag); }
    bool HasTag(AbilityTag tag) const;
    const std::vector<AbilityTag>& GetTags() const { return tags; }

    // Activation
    virtual bool CanActivate(Character* caster, Character* target) const;
    virtual CombatResult Activate(Character* caster, Character* target) = 0;

    // Cooldown management
    void StartCooldown() { remainingCooldown = cooldown; }
    void Update(float deltaTime);
    bool IsOnCooldown() const { return remainingCooldown > 0.0f; }
    float GetRemainingCooldown() const { return remainingCooldown; }

protected:
    std::string name;
    std::string description;
    int powerCost;
    float cooldown;
    float remainingCooldown = 0.0f;
    float range = 150.0f; // Default melee range
    std::vector<AbilityTag> tags;
};

// Sword abilities
class SwordSlash : public Ability {
public:
    SwordSlash();
    CombatResult Activate(Character* caster, Character* target) override;
};

class Parry : public Ability {
public:
    Parry();
    CombatResult Activate(Character* caster, Character* target) override;
};

class Riposte : public Ability {
public:
    Riposte();
    CombatResult Activate(Character* caster, Character* target) override;
};

// Fire Magic abilities
class Fireball : public Ability {
public:
    Fireball();
    CombatResult Activate(Character* caster, Character* target) override;
};

class FireWall : public Ability {
public:
    FireWall();
    CombatResult Activate(Character* caster, Character* target) override;
};

class Scintillate : public Ability {
public:
    Scintillate();
    CombatResult Activate(Character* caster, Character* target) override;
};

// Staff abilities
class Suppress : public Ability {
public:
    Suppress();
    CombatResult Activate(Character* caster, Character* target) override;
};

class HealingMist : public Ability {
public:
    HealingMist();
    CombatResult Activate(Character* caster, Character* target) override;
};

} // namespace Gorgon
