# Age of Sigmar 4th Edition - Battle Simulator

A tactical combat game implementing Age of Sigmar 4th Edition core rules.

## Versions

### HTML Version (`index.html`)
- Open directly in web browser
- No installation required
- Browser-based gameplay

### Python Version (`aos_game.py`)
- Desktop application using Pygame
- Better performance
- Native desktop experience

## Installation (Python Version)

1. Install Python 3.8 or higher
2. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```
3. Run the game:
   ```bash
   python aos_game.py
   ```

## Controls (Python Version)

### Mouse
- **Left Click**: Select units, move, attack
- **Right Click**: Cancel selection

### Keyboard
- **M**: Normal Move (during Movement Phase)
- **R**: Run (Move + D6, cannot shoot)
- **T**: Retreat (escape combat, take D3 mortal damage)
- **Space**: Next Phase
- **P**: Pause/Resume Timer
- **ESC**: Quit

## Game Features

### Battle Structure
- **Battle Rounds**: Multiple rounds, each with 2 turns
- **6 Phases per Turn**: Hero, Movement, Shooting, Charge, Combat, Turn End
- **Command Points**: 4 per player, reset each battle round
- **Victory Points**: Earned by controlling objectives

### Core Mechanics

#### Weapons & Abilities
- **Crit (2 Hits)**: Critical hit (unmodified 6) scores 2 hits
- **Crit (Auto-wound)**: Critical hit automatically wounds
- **Anti-X (+1 Rend)**: +1 Rend vs specific keywords (e.g., INFANTRY)
- **Charge (+1 Damage)**: +1 Damage if unit charged this turn
- **Ward Saves**: Save after normal save (e.g., 6+ ward for Stormcast)

#### Movement Options
- **Normal Move**: Up to MOVE characteristic
- **Run**: MOVE + D6", cannot shoot this turn
- **Retreat**: Escape combat, take D3 mortal damage, cannot shoot/charge
- **Charge**: Roll 2D6, move that distance toward enemy

#### Combat Rules
- Units in combat (within 3") cannot move or shoot
- Can only retreat to escape combat
- Attack sequence: Hit → Wound → Save → Ward → Damage
- Unmodified 1s always fail hit/wound/save rolls
- Unmodified 6s on hit rolls = critical hits

#### Objectives System
- 4 objectives on battlefield (numbered 1-4)
- Contest objectives within 3" using CONTROL characteristic
- Scored at Turn End Phase
- Each controlled objective = 1 VP

### Units

#### Order (Stormcast Eternals)
- **Liberator Squad**: Infantry with melee/ranged weapons, 6+ ward
- **Prosecutor Squad**: Flying infantry with 18" range javelins, 6+ ward
- **Knight-Arcanum**: Hero with Crit (2 Hits) ability, 6+ ward

#### Chaos
- **Chaos Warriors**: Infantry with Anti-Infantry (+1 Rend)
- **Bloodletters**: Daemons with Charge (+1 Damage), 5+ ward
- **Chaos Lord**: Hero with Crit (Auto-wound)

### Auto-Skip System
Phases automatically skip when no valid actions are available:
- Hero Phase: Always skips (no abilities yet)
- Movement Phase: Skips if all units moved or in combat
- Shooting Phase: Skips if no ranged units or enemies out of range
- Charge Phase: Skips if no enemies in charge range
- Combat Phase: Skips if no units in melee range (3")
- Turn End Phase: Always auto-scores objectives and skips

## Gameplay Tips

1. **Control Objectives**: Position units within 3" of objectives to contest them
2. **Use Cover**: Keep ranged units safe while softening up enemies
3. **Charge Bonuses**: Bloodletters gain +1 damage when charging
4. **Ward Saves**: Stormcast and Daemons have additional saves
5. **Combat Engagement**: Units in combat can't shoot or move normally
6. **Retreat Carefully**: Retreating costs D3 mortal damage
7. **Run When Needed**: Running gives +D6" movement but prevents shooting

## Development

### Structure (Python Version)
- `Weapon` class: Weapon stats and abilities
- `Unit` class: Unit characteristics and state
- `Objective` class: Objective markers
- `CombatLog` class: Scrolling combat log
- `Game` class: Main game logic and rendering

### Adding New Features
1. Add new weapon abilities in `perform_weapon_attack()`
2. Create new units in `create_units()`
3. Modify phases in the `Phase` enum
4. Add UI elements in `render()` methods

## Credits

Based on Age of Sigmar 4th Edition core rules by Games Workshop.

Implemented by Claude Code.

## License

For personal use and educational purposes only.
