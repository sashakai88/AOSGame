# Gorgon Prototype

A C++ prototype implementation of Project Gorgon-inspired combat mechanics using Raylib for 3D visualization.

## Features

### Core Combat System
- **Armor/Health/Power Trinity**: Three-resource system with regeneration
- **Project Gorgon Combat Formula**: Hit → Wound → Save → Ward → Damage
- **Dice-Based Mechanics**: d6 rolls with critical hits, auto-fails, and modifiers
- **Armor Mitigation**: 1 damage reduced per 25 armor
- **Combat State**: Regeneration speeds up when out of combat

### Character System
- **4 Playable Races**: Human, Elf, Rakshasa, Fairy (with different base stats)
- **Beast Forms**: Framework for 8 beast transformations
- **Dual-Skill System**: Two active combat skills at a time

### Skills & Abilities
- **Sword Skill**: Sword Slash, Parry, Riposte
- **Fire Magic**: Fireball, Scintillate, Fire Wall
- **Staff Skill**: Suppress, Healing Mist
- **Cooldowns**: Each ability has its own cooldown and power cost
- **Skill Progression**: Gain XP by using abilities

### Weapons
- **Varied Attacks**: Different numbers of attacks per weapon
- **Hit/Wound Targets**: Difficulty to land and wound with each weapon
- **Rend**: Modifier making saves harder
- **Critical Abilities**: Crit (2 Hits), Crit (Auto-wound)
- **Damage Types**: Physical (Slashing/Piercing/Crushing), Elemental (Fire/Cold/etc.)

## Project Structure

```
gorgon-prototype/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── include/
│   ├── core/
│   │   └── Types.h         # Core types, enums, Random class
│   ├── combat/
│   │   ├── Attributes.h    # Armor/Health/Power system
│   │   └── DamageCalculator.h  # Combat formula implementation
│   ├── characters/
│   │   └── Character.h     # Character, Player, Enemy classes
│   └── skills/
│       ├── Skill.h         # Skill system
│       └── Ability.h       # Ability system
├── src/
│   ├── main.cpp            # Main game loop with Raylib
│   ├── Character.cpp       # Character implementations
│   ├── Skill.cpp           # Skill implementations
│   └── Ability.cpp         # Ability implementations
├── data/                   # Game data files (future use)
└── build/                  # Build directory (generated)
```

## Building

### Prerequisites

- **CMake** 3.15 or higher
- **C++17** compatible compiler (GCC, Clang, MSVC)
- **Raylib** 4.0 or higher (will be auto-downloaded if not found)

### Linux/macOS

```bash
cd gorgon-prototype
mkdir build && cd build
cmake ..
make
./bin/GorgonPrototype
```

### Windows (Visual Studio)

```bash
cd gorgon-prototype
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
.\bin\Release\GorgonPrototype.exe
```

## Controls

- **1** - Sword Slash (Basic sword attack)
- **2** - Fireball (Ranged fire attack)
- **3** - Parry (Restore armor)
- **4** - Healing Mist (Restore health)
- **R** - Respawn enemy
- **H** - Toggle help
- **ESC** - Quit

## Gameplay

1. Start the game - You control a Hero fighting a Goblin
2. Use abilities (1-4) to attack or heal
3. Watch your Health, Power, and Armor bars
4. Power regenerates automatically (faster out of combat)
5. Armor regenerates over time
6. Health regenerates slowly
7. Defeat the enemy and press R to spawn a new one
8. Gain skill XP by using abilities

## Combat Formula

The combat system uses Project Gorgon's dice-based mechanics:

### Step 1: Hit Roll (1d6)
- Roll 1d6 + modifiers
- **1** = Auto miss (before modifiers)
- **6** = Critical hit (before modifiers)
- Must equal or exceed Hit Target (usually 4+)

### Step 2: Wound Roll (1d6)
- Roll 1d6
- **1** = Auto fail
- Must equal or exceed Wound Target (usually 4+)
- **Skipped if Crit (Auto-wound)**

### Step 3: Save Roll (1d6)
- Defender rolls 1d6
- **1** = Auto fail
- Modified by attacker's Rend
- Must equal or exceed modified Save Target

### Step 4: Ward Save (1d6)
- Optional additional save if defender has Ward
- Roll 1d6, must equal or exceed Ward value

### Step 5: Apply Damage
- If all rolls succeed (or fail for defender), apply damage
- Damage is reduced by armor mitigation (1 per 25 armor)

## Example Weapons

### Basic Sword
- Attacks: 2
- Hit: 4+
- Wound: 4+
- Rend: 0
- Damage: 1

### Fireball
- Attacks: 1
- Hit: 4+
- Wound: 3+
- Rend: 1
- Damage: 2
- Range: 18 meters
- Special: Crit (Auto-wound)

### Power Sword
- Attacks: 3
- Hit: 3+
- Wound: 3+
- Rend: 1
- Damage: 2
- Special: Crit (2 Hits)

## Race Stats

### Human
- Health: 100
- Power: 100
- Armor: 0
- Special: Bonus when socialized (not implemented)

### Elf
- Health: 90
- Power: 110
- Armor: 0
- Special: Bonus when clean (not implemented)

### Rakshasa
- Health: 110
- Power: 90
- Armor: 10
- Special: Bonus when peaceful (not implemented)

### Fairy
- Health: 80
- Power: 120
- Armor: 0
- Special: Unique abilities (not implemented)

## Development Roadmap

### Phase 1: Core Combat (COMPLETE)
- [x] Attribute system (Armor/Health/Power)
- [x] Damage calculator
- [x] Character system
- [x] Basic abilities
- [x] Skill progression
- [x] 3D visualization

### Phase 2: Expanded Content (TODO)
- [ ] All 100+ skills from Project Gorgon
- [ ] Beast Forms (Cow, Deer, Spider, etc.)
- [ ] Equipment system
- [ ] Treasure Effects (modify specific abilities)
- [ ] NPC Favor system
- [ ] More abilities per skill (6-8 per skill)

### Phase 3: World Systems (TODO)
- [ ] Multiple zones (Anagoge, Serbule, etc.)
- [ ] Dungeons
- [ ] Enemy AI improvements
- [ ] Loot system
- [ ] Crafting system

### Phase 4: Multiplayer (TODO)
- [ ] Networking
- [ ] Server/client architecture
- [ ] Synchronization
- [ ] Chat system

## Technical Details

### Random Number Generation
Uses `std::mt19937` (Mersenne Twister) for deterministic randomness, important for networking later.

### Attributes
All attributes update with delta time for smooth regeneration. Combat state affects regeneration rate (3x faster out of combat).

### Memory Management
Uses `std::shared_ptr` for skills/abilities and `std::unique_ptr` for characters to ensure proper cleanup.

### Extensibility
- Data-driven design for skills and abilities
- Virtual functions for custom character behavior
- Modular combat component system

## License

This is a prototype for educational and research purposes, inspired by Project Gorgon by Elder Game, LLC.

## Credits

- **Game Design**: Based on Project Gorgon wiki documentation
- **Graphics**: Raylib (https://www.raylib.com/)
- **Combat System**: Inspired by Warhammer Age of Sigmar and Project Gorgon

## Next Steps

To expand this prototype:

1. **Add More Skills**: Implement all 26 combat skills from Project Gorgon
2. **Beast Forms**: Complete transformation system with stat changes
3. **Equipment**: Add armor sets with Treasure Effects
4. **UI Improvements**: Better ability bars, combat log, skill trees
5. **Save System**: Save/load character progression
6. **Data Files**: Move skills/abilities to JSON data files
7. **Testing**: Unit tests for combat calculations
8. **Migration**: Eventually migrate to Unreal Engine 5 for full 3D MMO

## Build Notes

If Raylib is not found, CMake will automatically download and build it via FetchContent. This may take a few minutes on first build.

For faster development iteration, install Raylib system-wide:

**Ubuntu/Debian:**
```bash
sudo apt install libraylib-dev
```

**macOS:**
```bash
brew install raylib
```

**Windows:**
Download from https://github.com/raysan5/raylib/releases
