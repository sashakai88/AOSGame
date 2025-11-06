#include "../include/characters/Character.h"
#include <iostream>

namespace Gorgon {

// Character implementation
Character::Character(const std::string& name, Race race)
    : name(name), race(race) {
    InitializeRaceAttributes();
}

void Character::InitializeRaceAttributes() {
    // Base attributes for each race
    switch (race) {
        case Race::Human:
            attributes.Initialize(100.0f, 100.0f, 0.0f);
            // Humans: Bonus when socialized (not implemented in prototype)
            break;

        case Race::Elf:
            attributes.Initialize(90.0f, 110.0f, 0.0f);
            // Elves: More power, less health, bonus when clean
            break;

        case Race::Rakshasa:
            attributes.Initialize(110.0f, 90.0f, 10.0f);
            // Rakshasa: More health, less power, starts with armor
            break;

        case Race::Fairy:
            attributes.Initialize(80.0f, 120.0f, 0.0f);
            // Fairies: Low health, high power, special abilities
            break;
    }
}

void Character::SetPosition(float x, float y, float z) {
    posX = x;
    posY = y;
    posZ = z;
}

void Character::GetPosition(float& x, float& y, float& z) const {
    x = posX;
    y = posY;
    z = posZ;
}

void Character::Move(float dx, float dy, float dz) {
    posX += dx;
    posY += dy;
    posZ += dz;
}

CombatResult Character::Attack(Character& target, const WeaponData& weapon) {
    CombatResult result = DamageCalculator::CalculateAttack(
        attributes,
        target.GetAttributes(),
        weapon
    );

    // Print combat log
    std::cout << name << " attacks " << target.GetName() << " with " << weapon.name << ":\n";
    for (const auto& msg : result.messages) {
        std::cout << "  " << msg << "\n";
    }

    if (!target.IsAlive()) {
        std::cout << target.GetName() << " has been defeated!\n";
        target.OnDeath();
    }

    return result;
}

void Character::OnDamageTaken(float damage) {
    // Hook for effects that trigger on damage
}

void Character::OnDeath() {
    std::cout << name << " has died.\n";
}

void Character::AddSkill(std::shared_ptr<Skill> skill) {
    skills.push_back(skill);
}

void Character::SetActiveSkills(int skill1Index, int skill2Index) {
    activeSkill1Index = skill1Index;
    activeSkill2Index = skill2Index;
}

std::shared_ptr<Skill> Character::GetActiveSkill(int slot) const {
    if (slot == 0 && activeSkill1Index >= 0 && activeSkill1Index < skills.size()) {
        return skills[activeSkill1Index];
    }
    if (slot == 1 && activeSkill2Index >= 0 && activeSkill2Index < skills.size()) {
        return skills[activeSkill2Index];
    }
    return nullptr;
}

void Character::TransformTo(BeastForm form) {
    beastForm = form;
    std::cout << name << " transforms into " << static_cast<int>(form) << "!\n";
    // Apply beast form modifiers (not fully implemented in prototype)
}

void Character::RevertForm() {
    if (beastForm != BeastForm::None) {
        std::cout << name << " reverts to normal form.\n";
        beastForm = BeastForm::None;
    }
}

void Character::Update(float deltaTime) {
    attributes.Update(deltaTime);
}

// PlayerCharacter implementation
PlayerCharacter::PlayerCharacter(const std::string& name, Race race)
    : Character(name, race) {
}

void PlayerCharacter::GainExperience(const std::string& skillName, int amount) {
    experience += amount;
    std::cout << name << " gains " << amount << " XP in " << skillName << "\n";

    // Simple level up at 100 XP
    if (experience >= 100 * level) {
        level++;
        std::cout << name << " reached level " << level << "!\n";
    }
}

void PlayerCharacter::OnDeath() {
    Character::OnDeath();
    std::cout << "Game Over!\n";
}

// Enemy implementation
Enemy::Enemy(const std::string& name, int difficultyLevel)
    : Character(name, Race::Human), difficultyLevel(difficultyLevel) {
    // Scale attributes by difficulty
    attributes.maxHealth *= difficultyLevel;
    attributes.currentHealth = attributes.maxHealth;
    attributes.maxArmor *= difficultyLevel;
}

void Enemy::UpdateAI(float deltaTime, Character* target) {
    aiUpdateTimer += deltaTime;

    // Simple AI: Attack every 2 seconds
    if (isAggressive && target && target->IsAlive() && aiUpdateTimer >= 2.0f) {
        aiUpdateTimer = 0.0f;

        // Use basic attack
        WeaponData weapon = Weapons::CreateUnarmed();
        Attack(*target, weapon);
    }
}

void Enemy::OnDeath() {
    Character::OnDeath();
    isAggressive = false;
}

} // namespace Gorgon
