#include "../include/skills/Ability.h"
#include "../include/characters/Character.h"
#include <iostream>

namespace Gorgon {

// Ability implementation
Ability::Ability(const std::string& name, int powerCost, float cooldown)
    : name(name), powerCost(powerCost), cooldown(cooldown) {
}

bool Ability::HasTag(AbilityTag tag) const {
    for (const auto& t : tags) {
        if (t == tag) return true;
    }
    return false;
}

bool Ability::CanActivate(Character* caster, Character* target) const {
    if (!caster || !caster->IsAlive()) return false;
    if (IsOnCooldown()) return false;
    if (caster->GetAttributes().currentPower < powerCost) return false;
    if (target && !target->IsAlive()) return false;
    return true;
}

void Ability::Update(float deltaTime) {
    if (remainingCooldown > 0.0f) {
        remainingCooldown -= deltaTime;
        if (remainingCooldown < 0.0f) {
            remainingCooldown = 0.0f;
        }
    }
}

// Sword abilities
SwordSlash::SwordSlash() : Ability("Sword Slash", 10, 3.0f) {
    description = "A basic sword attack";
    range = 150.0f;
    AddTag(AbilityTag::BasicAttack);
}

CombatResult SwordSlash::Activate(Character* caster, Character* target) {
    if (!CanActivate(caster, target)) {
        CombatResult result;
        result.AddMessage("Cannot activate " + name);
        return result;
    }

    // Consume power
    caster->GetAttributes().ConsumePower(static_cast<float>(powerCost));
    StartCooldown();

    // Create weapon data
    WeaponData weapon = Weapons::CreateSword();

    // Execute attack
    std::cout << caster->GetName() << " uses " << name << "!\n";
    return caster->Attack(*target, weapon);
}

Parry::Parry() : Ability("Parry", 15, 8.0f) {
    description = "Block the next attack and restore armor";
    AddTag(AbilityTag::Buff);
}

CombatResult Parry::Activate(Character* caster, Character* target) {
    CombatResult result;
    if (!CanActivate(caster, nullptr)) {
        result.AddMessage("Cannot activate " + name);
        return result;
    }

    caster->GetAttributes().ConsumePower(static_cast<float>(powerCost));
    StartCooldown();

    // Restore armor
    caster->GetAttributes().RestoreArmor(50.0f);

    std::cout << caster->GetName() << " uses " << name << "! +50 Armor\n";
    result.AddMessage("Restored 50 Armor");
    return result;
}

Riposte::Riposte() : Ability("Riposte", 20, 12.0f) {
    description = "Counter-attack with increased power";
    range = 150.0f;
    AddTag(AbilityTag::NiceAttack);
}

CombatResult Riposte::Activate(Character* caster, Character* target) {
    if (!CanActivate(caster, target)) {
        CombatResult result;
        result.AddMessage("Cannot activate " + name);
        return result;
    }

    caster->GetAttributes().ConsumePower(static_cast<float>(powerCost));
    StartCooldown();

    // Powerful sword attack
    WeaponData weapon = Weapons::CreatePowerfulSword();

    std::cout << caster->GetName() << " uses " << name << "!\n";
    return caster->Attack(*target, weapon);
}

// Fire Magic abilities
Fireball::Fireball() : Ability("Fireball", 15, 4.0f) {
    description = "Launch a ball of fire at your enemy";
    range = 1800.0f;
    AddTag(AbilityTag::BasicAttack);
}

CombatResult Fireball::Activate(Character* caster, Character* target) {
    if (!CanActivate(caster, target)) {
        CombatResult result;
        result.AddMessage("Cannot activate " + name);
        return result;
    }

    caster->GetAttributes().ConsumePower(static_cast<float>(powerCost));
    StartCooldown();

    WeaponData weapon = Weapons::CreateFireball();

    std::cout << caster->GetName() << " casts " << name << "!\n";
    return caster->Attack(*target, weapon);
}

FireWall::FireWall() : Ability("Fire Wall", 30, 15.0f) {
    description = "Create a wall of flames that damages enemies";
    range = 1000.0f;
    AddTag(AbilityTag::AoE);
    AddTag(AbilityTag::EpicAttack);
}

CombatResult FireWall::Activate(Character* caster, Character* target) {
    if (!CanActivate(caster, target)) {
        CombatResult result;
        result.AddMessage("Cannot activate " + name);
        return result;
    }

    caster->GetAttributes().ConsumePower(static_cast<float>(powerCost));
    StartCooldown();

    // Create a powerful fire attack
    WeaponData weapon;
    weapon.name = "Fire Wall";
    weapon.attacks = 3;
    weapon.hitTarget = 3;
    weapon.woundTarget = 3;
    weapon.rend = 2;
    weapon.damage = 2;
    weapon.damageType = DamageType::Fire;

    std::cout << caster->GetName() << " creates a " << name << "!\n";
    return caster->Attack(*target, weapon);
}

Scintillate::Scintillate() : Ability("Scintillate", 12, 3.0f) {
    description = "Quick fire burst";
    range = 1200.0f;
    AddTag(AbilityTag::BasicAttack);
    AddTag(AbilityTag::Burst);
}

CombatResult Scintillate::Activate(Character* caster, Character* target) {
    if (!CanActivate(caster, target)) {
        CombatResult result;
        result.AddMessage("Cannot activate " + name);
        return result;
    }

    caster->GetAttributes().ConsumePower(static_cast<float>(powerCost));
    StartCooldown();

    WeaponData weapon;
    weapon.name = "Scintillate";
    weapon.attacks = 2;
    weapon.hitTarget = 4;
    weapon.woundTarget = 4;
    weapon.damage = 1;
    weapon.damageType = DamageType::Fire;

    std::cout << caster->GetName() << " casts " << name << "!\n";
    return caster->Attack(*target, weapon);
}

// Staff abilities
Suppress::Suppress() : Ability("Suppress", 8, 2.0f) {
    description = "Reduce enemy power";
    range = 1000.0f;
    AddTag(AbilityTag::Debuff);
}

CombatResult Suppress::Activate(Character* caster, Character* target) {
    CombatResult result;
    if (!CanActivate(caster, target)) {
        result.AddMessage("Cannot activate " + name);
        return result;
    }

    caster->GetAttributes().ConsumePower(static_cast<float>(powerCost));
    StartCooldown();

    // Reduce target's power
    target->GetAttributes().currentPower = std::max(0.0f, target->GetAttributes().currentPower - 20.0f);

    std::cout << caster->GetName() << " uses " << name << " on " << target->GetName() << "!\n";
    result.AddMessage("Drained 20 Power from target");
    return result;
}

HealingMist::HealingMist() : Ability("Healing Mist", 25, 10.0f) {
    description = "Restore health over time";
    range = 500.0f;
    AddTag(AbilityTag::Heal);
}

CombatResult HealingMist::Activate(Character* caster, Character* target) {
    CombatResult result;
    if (!CanActivate(caster, target)) {
        result.AddMessage("Cannot activate " + name);
        return result;
    }

    caster->GetAttributes().ConsumePower(static_cast<float>(powerCost));
    StartCooldown();

    // Restore health
    float healAmount = 30.0f;
    target->GetAttributes().RestoreHealth(healAmount);

    std::cout << caster->GetName() << " casts " << name << " on " << target->GetName() << "!\n";
    result.AddMessage("Restored " + std::to_string(static_cast<int>(healAmount)) + " Health");
    return result;
}

} // namespace Gorgon
