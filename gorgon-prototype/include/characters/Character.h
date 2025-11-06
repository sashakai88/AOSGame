#pragma once

#include "../core/Types.h"
#include "../combat/Attributes.h"
#include "../combat/DamageCalculator.h"
#include <string>
#include <vector>
#include <memory>

namespace Gorgon {

// Forward declarations
class Skill;
class Ability;

// Base character class
class Character {
public:
    Character(const std::string& name, Race race);
    virtual ~Character() = default;

    // Basic info
    std::string GetName() const { return name; }
    Race GetRace() const { return race; }
    BeastForm GetBeastForm() const { return beastForm; }

    // Attributes
    Attributes& GetAttributes() { return attributes; }
    const Attributes& GetAttributes() const { return attributes; }

    // Position and movement
    void SetPosition(float x, float y, float z);
    void GetPosition(float& x, float& y, float& z) const;
    void Move(float dx, float dy, float dz);

    // Combat
    virtual CombatResult Attack(Character& target, const WeaponData& weapon);
    virtual void OnDamageTaken(float damage);
    virtual void OnDeath();

    // Skills
    void AddSkill(std::shared_ptr<Skill> skill);
    void SetActiveSkills(int skill1Index, int skill2Index);
    std::shared_ptr<Skill> GetActiveSkill(int slot) const;
    std::vector<std::shared_ptr<Skill>>& GetSkills() { return skills; }

    // Beast Forms
    void TransformTo(BeastForm form);
    void RevertForm();

    // Update
    virtual void Update(float deltaTime);

    // Status
    bool IsAlive() const { return attributes.IsAlive(); }
    bool IsInCombat() const { return attributes.isInCombat; }

protected:
    std::string name;
    Race race;
    BeastForm beastForm = BeastForm::None;

    Attributes attributes;

    // Position
    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = 0.0f;

    // Skills
    std::vector<std::shared_ptr<Skill>> skills;
    int activeSkill1Index = -1;
    int activeSkill2Index = -1;

    // Initialize attributes based on race
    void InitializeRaceAttributes();
};

// Player character
class PlayerCharacter : public Character {
public:
    PlayerCharacter(const std::string& name, Race race);

    // Player-specific functionality
    void GainExperience(const std::string& skillName, int amount);
    int GetLevel() const { return level; }

    void OnDeath() override;

private:
    int level = 1;
    int experience = 0;
};

// Enemy character
class Enemy : public Character {
public:
    Enemy(const std::string& name, int difficultyLevel);

    // Enemy-specific functionality
    void SetAggressive(bool aggressive) { isAggressive = aggressive; }
    bool IsAggressive() const { return isAggressive; }

    // AI behavior
    virtual void UpdateAI(float deltaTime, Character* target);

    void OnDeath() override;

private:
    bool isAggressive = true;
    float aiUpdateTimer = 0.0f;
    int difficultyLevel = 1;
};

} // namespace Gorgon
