#pragma once

#include <random>
#include <string>
#include <vector>
#include <memory>

namespace Gorgon {

// Singleton Random Number Generator
class Random {
public:
    static Random& Instance() {
        static Random instance;
        return instance;
    }

    // Prevent copying
    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

    // Roll a single d6 (1-6)
    int RollD6() {
        return distribution(generator);
    }

    // Roll multiple d6
    int RollMultipleD6(int count) {
        int total = 0;
        for (int i = 0; i < count; i++) {
            total += RollD6();
        }
        return total;
    }

    // Roll 2d6 (common in Project Gorgon)
    int Roll2D6() {
        return RollD6() + RollD6();
    }

    // Seed the random generator (useful for testing)
    void Seed(unsigned int seed) {
        generator.seed(seed);
    }

    // Random float between 0.0 and 1.0
    float RollFloat() {
        return floatDistribution(generator);
    }

private:
    Random()
        : generator(std::random_device{}()),
          distribution(1, 6),
          floatDistribution(0.0f, 1.0f) {}

    std::mt19937 generator;
    std::uniform_int_distribution<int> distribution;
    std::uniform_real_distribution<float> floatDistribution;
};

// Damage types in Project Gorgon
enum class DamageType {
    // Physical
    Slashing,
    Piercing,
    Crushing,

    // Elemental
    Fire,
    Cold,
    Electricity,
    Acid,

    // Special
    Nature,
    Psychic,
    Darkness,
    Poison,
    Trauma
};

// Combat result for detailed feedback
struct CombatResult {
    int damage = 0;
    int hitsMade = 0;
    int hitsMissed = 0;
    int wounds = 0;
    int saves = 0;
    int wardSaves = 0;
    bool wasCritical = false;
    std::vector<std::string> messages;

    void AddMessage(const std::string& msg) {
        messages.push_back(msg);
    }
};

// Race types
enum class Race {
    Human,
    Elf,
    Rakshasa,
    Fairy
};

// Beast forms
enum class BeastForm {
    None,
    Cow,
    Deer,
    Spider,
    Pig,
    Rabbit,
    GiantBat,
    Lycanthrope,
    SpiritFox
};

// Skill categories
enum class SkillCategory {
    Combat,
    Beast,
    Trade,
    Other
};

} // namespace Gorgon
