#include "../include/skills/Skill.h"
#include "../include/skills/Ability.h"
#include <iostream>

namespace Gorgon {

// Skill implementation
Skill::Skill(const std::string& name, SkillCategory category)
    : name(name), category(category) {
}

void Skill::GainXP(int amount) {
    int oldLevel = progression.level;
    progression.AddXP(amount);

    if (progression.level > oldLevel) {
        std::cout << "Skill " << name << " leveled up to " << progression.level << "!\n";
    }
}

void Skill::AddAbility(std::shared_ptr<Ability> ability, int unlockLevel) {
    abilities.push_back({ability, unlockLevel});
}

std::vector<std::shared_ptr<Ability>> Skill::GetAvailableAbilities() const {
    std::vector<std::shared_ptr<Ability>> available;
    for (const auto& unlock : abilities) {
        if (progression.level >= unlock.unlockLevel) {
            available.push_back(unlock.ability);
        }
    }
    return available;
}

std::shared_ptr<Ability> Skill::GetAbility(int index) const {
    auto available = GetAvailableAbilities();
    if (index >= 0 && index < available.size()) {
        return available[index];
    }
    return nullptr;
}

void Skill::Update(float deltaTime) {
    // Update ability cooldowns
    for (auto& unlock : abilities) {
        unlock.ability->Update(deltaTime);
    }
}

// SwordSkill implementation
SwordSkill::SwordSkill() : Skill("Sword", SkillCategory::Combat) {
    // Add abilities
    AddAbility(std::make_shared<SwordSlash>(), 0);    // Unlocked at level 0
    AddAbility(std::make_shared<Parry>(), 3);         // Unlocked at level 3
    AddAbility(std::make_shared<Riposte>(), 5);       // Unlocked at level 5
}

// FireMagicSkill implementation
FireMagicSkill::FireMagicSkill() : Skill("Fire Magic", SkillCategory::Combat) {
    AddAbility(std::make_shared<Fireball>(), 0);      // Unlocked at level 0
    AddAbility(std::make_shared<Scintillate>(), 3);   // Unlocked at level 3
    AddAbility(std::make_shared<FireWall>(), 7);      // Unlocked at level 7
}

// StaffSkill implementation
StaffSkill::StaffSkill() : Skill("Staff", SkillCategory::Combat) {
    AddAbility(std::make_shared<Suppress>(), 0);      // Unlocked at level 0
    AddAbility(std::make_shared<HealingMist>(), 4);   // Unlocked at level 4
}

// BattleChemistrySkill implementation
BattleChemistrySkill::BattleChemistrySkill() : Skill("Battle Chemistry", SkillCategory::Combat) {
    // Placeholder - abilities would be added here
}

} // namespace Gorgon
