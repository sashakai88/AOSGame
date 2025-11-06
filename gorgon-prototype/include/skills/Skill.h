#pragma once

#include "../core/Types.h"
#include <string>
#include <vector>
#include <memory>

namespace Gorgon {

// Forward declaration
class Ability;
class Character;

// Skill progression data
struct SkillLevel {
    int level = 0;
    int currentXP = 0;
    int xpToNextLevel = 100;

    bool CanLevelUp() const {
        return currentXP >= xpToNextLevel;
    }

    void AddXP(int amount) {
        currentXP += amount;
        while (CanLevelUp()) {
            LevelUp();
        }
    }

    void LevelUp() {
        currentXP -= xpToNextLevel;
        level++;
        xpToNextLevel = static_cast<int>(xpToNextLevel * 1.1f); // 10% increase per level
    }
};

// Base Skill class
class Skill {
public:
    Skill(const std::string& name, SkillCategory category);
    virtual ~Skill() = default;

    // Basic info
    std::string GetName() const { return name; }
    SkillCategory GetCategory() const { return category; }
    int GetLevel() const { return progression.level; }

    // Progression
    void GainXP(int amount);
    SkillLevel& GetProgression() { return progression; }

    // Abilities
    void AddAbility(std::shared_ptr<Ability> ability, int unlockLevel);
    std::vector<std::shared_ptr<Ability>> GetAvailableAbilities() const;
    std::shared_ptr<Ability> GetAbility(int index) const;
    size_t GetAbilityCount() const { return abilities.size(); }

    // Update cooldowns
    void Update(float deltaTime);

protected:
    std::string name;
    SkillCategory category;
    SkillLevel progression;

    struct AbilityUnlock {
        std::shared_ptr<Ability> ability;
        int unlockLevel;
    };

    std::vector<AbilityUnlock> abilities;
};

// Specific skill types

class SwordSkill : public Skill {
public:
    SwordSkill();
};

class FireMagicSkill : public Skill {
public:
    FireMagicSkill();
};

class StaffSkill : public Skill {
public:
    StaffSkill();
};

class BattleChemistrySkill : public Skill {
public:
    BattleChemistrySkill();
};

} // namespace Gorgon
