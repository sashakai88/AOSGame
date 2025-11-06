# Project Gorgon-Inspired Game: Complete Design Document

## Executive Summary

**Game Title:** Gorgon Chronicles (Working Title)

**Genre:** Fantasy MMORPG / Action-RPG

**Core Concept:** A skill-based RPG inspired by Project Gorgon featuring:
- Classless progression with 100+ learnable skills
- Unique beast transformation system
- Deep crafting and social systems
- Equipment that modifies specific abilities (not just stat boosts)
- Meaningful player choices with permanent consequences

---

## Table of Contents

1. [Core Gameplay Systems](#core-gameplay-systems)
2. [Combat Mechanics](#combat-mechanics)
3. [Character Progression](#character-progression)
4. [Beast Forms System](#beast-forms-system)
5. [Equipment & Items](#equipment--items)
6. [Skills System](#skills-system)
7. [World & Zones](#world--zones)
8. [NPC & Social Systems](#npc--social-systems)
9. [Economy & Trading](#economy--trading)
10. [Technical Implementation Plan](#technical-implementation-plan)

---

## 1. Core Gameplay Systems

### 1.1 Game Philosophy

**"Learn by Doing"**
- Skills level up through use, not experience points
- Experiment and discover combinations
- No rigid class system - players define their own build

**"Actions Have Consequences"**
- Permanent transformations (Beast Forms, especially Lycanthropy)
- NPC relationships that matter
- Reputation affects available services

**"Depth Over Breadth"**
- Each skill has 50-100 levels of progression
- Equipment has ability-specific modifiers
- Complex interactions between systems

### 1.2 Core Gameplay Loop

```
Explore → Fight/Gather → Craft → Improve Equipment →
Trade/Socialize → Unlock New Skills → Explore Deeper
```

---

## 2. Combat Mechanics

### 2.1 Core Combat Resources

#### **Armor**
- Primary defensive stat
- 1 damage mitigated per 25 points of armor
- Restored by "Combat Refresh" abilities
- **Formula:** `Damage Reduction = Armor / 25`

#### **Health**
- Hit points determining survival
- Restored by potions, abilities, and regeneration
- Can be topped up during combat with consumables
- **Starting Values:** 100-200 HP depending on race/build

#### **Power**
- Mana/energy for using abilities
- Most abilities cost power
- Emergency abilities have no power cost but longer cooldowns
- Regenerates slowly in combat, faster out of combat
- **Starting Values:** 100-150 Power

### 2.2 Combat Flow

#### **Attack Sequence**
```
1. Hit Roll (vs Target Number)
   ↓ (if successful)
2. Wound Roll (vs Target Number)
   ↓ (if successful)
3. Save Roll (modified by Rend)
   ↓ (if failed)
4. Ward Save (if available)
   ↓ (if failed)
5. Damage Applied
```

#### **Critical Hits**
- Unmodified 6 on hit roll = Critical
- Critical effects vary by weapon:
  - **Crit (2 Hits):** One attack becomes two hits
  - **Crit (Auto-wound):** Automatically wounds, skip wound roll
  - **Crit (Mortal):** Deals damage as mortal wounds (bypasses armor)

#### **Special Mechanics**

**Rage System (Enemies)**
- Monsters build rage as they take/deal damage
- Full rage bar triggers special attack
- Players can:
  - Deplete rage (Sword Parry)
  - Slow rage gain (Psychology abilities)
  - Manage rage in groups

**Vulnerability Windows**
- Enemies periodically become vulnerable (visual + audio cue)
- Certain abilities deal bonus damage to vulnerable targets
- Whack-a-mole timing mechanic

**Combat Refresh**
- Special "Basic Attack" category abilities
- Restore small amounts of Health, Armor, or Power
- Encourages active rotation rather than spam

### 2.3 Damage Types

- **Physical:** Slashing, Piercing, Crushing
- **Elemental:** Fire, Cold, Electricity, Acid
- **Special:** Nature, Psychic, Darkness, Demonic, Trauma, Poison

### 2.4 Ability Tags

Abilities are tagged for proc effects:
- **Basic Attack:** Triggers Combat Refresh
- **Nice Attack:** Common attacks
- **Core Attack:** Bread-and-butter abilities
- **Epic Attack:** High damage, long cooldown
- **Signature Debuff:** Defining debuff abilities

**Example:**
> "After using Precision Pierce, your Nice Attacks deal +64 damage for 6 seconds"

---

## 3. Character Progression

### 3.1 Races

#### **Human**
- **Philosophy:** Traders, hard workers, loyal
- **Bonus:** +XP when recently socialized
- **Penalty:** -XP when lonely
- **Starting Skills:** Sword 5, Endurance 5

#### **Elf**
- **Philosophy:** Long-lived, clean, friendly
- **Bonus:** +XP when clean
- **Penalty:** -XP when filthy
- **Starting Skills:** Archery 5, Nature Appreciation 5

#### **Rakshasa**
- **Philosophy:** Tiger-warriors with strong justice sense
- **Bonus:** +XP when feeling peaceable (haven't killed sentients)
- **Penalty:** -XP when feeling guilty
- **Starting Skills:** Sword 10, Psychology 5

#### **Fairy** (Advanced Race - Unlock Required)
- **Philosophy:** Immortal fae from another realm
- **Special Abilities:**
  - Can fly
  - Learn Fairy Magic (Mentalism subskill)
  - Break items into Fairy Dust and Crystal Ice
- **Limitations:**
  - -16 inventory slots (smaller/weaker)
  - -10% direct damage when wearing 3+ metal armor pieces
  - Respawn in Fae Realm on death
- **Starting Skills:** Ice Magic 30, Mentalism 30, Fairy Magic 30, Knife Fighting 30, Animal Handling 30
- **Starting Pets:** Bee and Wasp (Bond Level 100)

### 3.2 Starting Experience

**Tutorial Island: Anagoge**
- Teach basic movement and combat
- Introduction to the dual-skill system
- First quest chains
- Portal to main world at end

**First Zone: Serbule**
- Friendly starter town
- Access to basic trainers
- Introduction to crafting
- Side quests teaching mechanics

---

## 4. Beast Forms System

### 4.1 Overview

**Concept:** Players can transform into animals, gaining unique abilities and playstyles

**Key Features:**
- Each form has its own combat skill
- Lose access to weapons/handheld items
- Different NPCs react differently to animals
- Some NPCs only speak to animals
- Beast Speech impairment (improves with skill)

### 4.2 Beast Form Types

#### **Cow** (Tank Archetype)
- **Playstyle:** Durable tank with self-healing
- **Key Abilities:**
  - Tough Hoof (survivability)
  - Chew Cud (regeneration)
  - Moo of Determination (sprint)
  - Collect Milk (produce milk for crafting)
- **Special:** Can be milked by other players (with consent)
- **Transformation:** Defeat boss Maronesa's rage attack
- **Removal:** Defeat Maronesa, drink Minor Panacea, Word of Power

#### **Deer** (Off-Tank)
- **Playstyle:** Mobile off-tank with jumping
- **Key Abilities:**
  - Bounding Escape (jump higher/faster)
  - AoE Taunts
  - Summon Deer (pet ally)
- **Special:** Druid skill unlocks Deer Form toggle at level 25
- **Transformation:** Boss Ciervos rage attack, Deer Juice, Druid L25
- **Removal:** Defeat Ciervos, Un-Deer Juice, Word of Power

#### **Spider** (DPS/Assassin)
- **Playstyle:** Crushing and Poison damage
- **Key Abilities:**
  - Incubate (inject eggs, spawn pet spider)
  - Insidious Illusion (temporary humanoid form, 20min CD, costs Moonstone)
  - Webspin (produce Spiderweb for crafting)
- **Special:** Can craft Spider Casing armor (changes carapace color)
- **Transformation:** Drink Spider Juice
- **Removal:** Un-Spider Juice, Word of Power, Minor Panacea

#### **Pig** (Support/Healer)
- **Playstyle:** Group support and healing
- **Key Abilities:**
  - Mudbath (healing)
  - Frenzy (damage buff)
  - Porcine Alertness (awareness buff)
  - Terror Dash (speed boost escape)
  - Truffle Sniff (find mushrooms)
- **Transformation:** Drink Pig Juice
- **Removal:** Un-Pig Juice, Word of Power

#### **Rabbit** (Stealth)
- **Playstyle:** Stealth and mobility
- **Key Abilities:**
  - Play Dead (stealth)
  - Hare Dash (speed increase)
  - Long Ear (reduce detection range, slows movement)
- **Special:** Only Beast Form that can use Ice Magic (requires special headgear)
- **Transformation:** Drink Bunny Juice (seasonal event)
- **Removal:** Un-Bunny Juice, Minor Panacea

#### **Giant Bat** (Versatile)
- **Playstyle:** Knock-back, recovery, high damage
- **Key Abilities:**
  - Echolocation (see in dark)
  - Flight
  - Virulent Bite (infect others in PVP)
- **Special:** Can fly, infect others with bat disease in PVP
- **Transformation:** Fevered Trismus disease (untreated for 2 hours)
- **Removal:** Un-Bat Juice, Minor Panacea

#### **Lycanthropy** (PERMANENT)
- **Playstyle:** Powerful slashing/crushing/trauma attacks
- **Key Abilities:**
  - Switch between human and wolf form
  - Howl Mode (pack buffs)
  - Forced into wolf during full moon
- **WARNING:** Permanent transformation, cannot be removed
- **Special:** Can wear wolf-form armor (Blacksmithing)
- **Transformation:** Gift Spirit Stone to Altar of Norala
- **Removal:** NONE - Permanent!

#### **Spirit Fox** (Trickster)
- **Playstyle:** Mobility and crowd control
- **Key Abilities:**
  - Dimensional Snare (immobilize)
  - Blur Step (melee evasion boost)
  - Illusive Guise (humanoid disguise)
- **Damage Types:** Crushing and Darkness
- **Special:** No Beast Speech impairment
- **Transformation:** Gift Spirit Stone to Completely Normal Fox
- **Removal:** Trade Spirit Stone, Minor Panacea

### 4.3 Beast Form Mechanics

#### **Beast Speech**
- Low skill: Random words replaced with animal sounds
- "I need help" → "I moooo moooo"
- Improves with Beast Speech skill level
- Spirit Fox immune to this

#### **NPC Relationships**
- Many shopkeepers refuse to deal with animals
- Some NPCs only speak to animals
- Animal-friendly locations: Animal Town (Sun Vale), various outposts
- Spider/Bat/Lycanthrope considered "scary" - more restrictions

#### **Equipment Restrictions**
- Cannot use weapons (Sword, Fire Magic, Shield, etc.)
- Can use necklaces with Necromancy Gems or Holy Symbols
- Special animal-only equipment available
- Requires rune-infused gems for crafting (Sigil Scripting 25)

#### **Animal Town** (Safe Haven for Beast Forms)
- Hidden in Sun Vale
- Only accessible to Beast Forms (3+ hour transformation time)
- Humanoids forcibly removed by Summoned Guardians
- Exclusive trainers, storage, and services

### 4.4 Beast Skills

**Beast Metabolism** (Lycanthropes only)
- Levels by consuming enemy corpses

**Beast Speech**
- Improves communication
- Levels by speaking (private channels recommended)

**Survival Instincts** (All Beast Forms)
- Sidebar utility abilities
- Toughen Up (alternative to Armor Patching)
- Toxic Flesh (poison melee attackers)
- Lend Grace (evasion + power regen)

**Warden** (Beast Form Exclusive Combat Skill)
- Electricity, Fire, and Acid damage
- Must have Friends favor with Suspicious Cow, Red, or Norbert
- Complete quest series to unlock

---

## 5. Equipment & Items

### 5.1 Equipment Philosophy

**Equipment modifies abilities, not just stats**

Traditional MMO:
> Sword: +50 Attack, +10 Strength

Project Gorgon Style:
> Sword: "When you use Precision Pierce, restore 15 Health and gain +64 Nice Attack damage for 6 seconds"

### 5.2 Armor System

#### **Armor Types**
- **Metal:** High armor, requires Endurance, restricts magic for Fairies
- **Leather:** Balanced, moderate armor
- **Cloth:** Low armor, high Power bonuses, good for casters
- **Organic:** Seashell/Kelp armor, underwater bonuses

#### **Armor Set Bonuses** (Examples)

**Death Trooper Armor** (Metal, Endurance 50)
- Max Armor: +665
- Armor from Combat Refresh: +280
- Melee Attack Damage: +20
- Shield Skill Damage: +5
- Kick Damage: +10

**Mutterer Cabalist Armor** (Cloth, Endurance 35)
- Max Armor: +325
- Health from Combat Refresh: +280
- Max Power: +40
- Direct Fire Damage: +10%
- Fire Magic Damage: +10
- Ice Magic Damage: +10

**Fae Navy Armor** (Organic)
- Max Armor: +695
- Chance to Ignore Stuns: +20%
- Vulnerability to Electricity/Darkness/Demonic: -10%
- Projectile Accuracy: +10%
- +5 Inventory Slots
- Sprint/Flying/Swimming Speed: +1

### 5.3 Treasure Effects

**Concept:** Equipment has 2-4 "Treasure Effects" that modify specific abilities

**Examples:**
- "For 6 seconds after using Sword Parry, regain 10 Health per update"
- "Precision Pierce deals +64 Nice Attack damage"
- "Your Fireball deals +20% damage and generates 20% less Rage"
- "After using Tell Me About Your Mother, restore 50 Armor"

**Categories:**
- Damage boosts for specific abilities
- Power cost reduction
- Cooldown reduction
- Secondary effects (healing, buffs, debuffs)
- Rage management

### 5.4 Crafting Equipment

**Gemstone System**
- Two gemstones required when crafting magical equipment
- Gemstones determine which skills get Treasure Effects
- Example: Sapphire (Fire Magic) + Topaz (Sword) = Equipment with Fire Magic and Sword bonuses

**Beast Form Crafting**
- Requires rune-infused gems (Sigil Scripting 25)
- Bat Malachite, Cow Malachite, Deer Malachite, Pig Malachite, etc.
- Spirit Fox Lapis for Spirit Fox

### 5.5 Augmentation System

**Three Types:**
1. **Armor Augmentation:** Chest/Leg augments
2. **Ancillary-Armor Augmentation:** Helm/Shoes augments
3. **Weapon Augmentation:** Weapon/Off-hand/Glove augments
4. **Jewelry Augmentation:** Necklace/Ring augments

**Purpose:** Add additional Treasure Effects to gear

### 5.6 Transmutation

**Concept:** Transform items into other items
- Break down equipment
- Recombine into new forms
- Preserve some properties

---

## 6. Skills System

### 6.1 Skill Categories

#### **Combat Skills** (26 skills)

**Weapon Skills:**
- Sword, Hammer, Staff, Shield, Knife Fighting, Archery, Crossbow, Unarmed

**Magic Skills:**
- Fire Magic, Ice Magic, Mentalism, Necromancy, Psychology, Druid, Priest, Bard, Fairy Magic, Weather Witching

**Special Combat:**
- Animal Handling (pets), Battle Chemistry (potions), Vampirism

#### **Beast Skills** (13 skills)
- Cow, Deer, Spider, Pig, Rabbit, Giant Bat, Lycanthropy, Spirit Fox
- Beast Metabolism, Beast Speech, Survival Instincts, Warden, Howling

#### **Trade Skills** (45+ skills)

**Crafting:**
- Blacksmithing, Armorsmithing, Bladesmithing, Carpentry, Leatherworking, Tailoring, Toolcrafting, Jewelry Crafting

**Cooking/Alchemy:**
- Cooking, Alchemy, Brewing, Cheesemaking, Sushi Preparation

**Gathering:**
- Mining, Foraging, Fishing, Angling, Butchering, Skinning, Mycology, Gardening, Mushroom Farming

**Enhancement:**
- Armor Augmentation, Weapon Augmentation, Jewelry Augmentation, Transmutation, Shamanic Infusion

**Art/Culture:**
- Calligraphy, Artistry, Art History, Flower Arrangement, Non-Fiction Writing, Whittling

#### **Other Skills** (30+ skills)

**Utility:**
- First Aid, Armor Patching, Endurance, Meditation, Teleportation, Sigil Scripting

**Knowledge:**
- Anatomy, Lore, Pathology, Phrenology, Assessment, Genetics, Paleontology

**Social:**
- Oratory, Performance, Interpretive Dance, Retail Management, Industry

**Quirky:**
- Golf, Gaming, Gourmand, Dying, Corpse Talking, Safecracking

### 6.2 Dual-Skill System

**Core Concept:** Players can have TWO active combat skills

**Examples of Synergies:**
- **Sword + Psychology:** Melee with rage control and debuffs
- **Fire Magic + Ice Magic:** Full elemental coverage
- **Archery + Animal Handling:** Ranged DPS with pet tank
- **Necromancy + Priest:** Death and life magic (unusual but possible)
- **Battle Chemistry + Fire Magic:** Area damage specialist
- **Shield + Hammer:** Classic tank

**Skill Compatibility:**
- Most skills can work together
- Some skills require specific equipment (can't use Fire Magic + Sword simultaneously since both need main hand)
- Consult Skill Compatibility Chart for synergies

### 6.3 Skill Trainers

**Concept:** NPCs teach skills and abilities

**Progression:**
1. Find NPC who teaches skill
2. Build Favor (relationship) with NPC
3. Unlock training as Favor increases
4. Pay for ability training
5. Use ability to level skill

**Favor Tiers:**
- Neutral / Comfortable / Friends / Close Friends / Best Friends / Like Family / Soul Mates

**Gifting System:**
- Give items NPCs like to raise Favor
- Each NPC has preferences
- Some items give more Favor than others

### 6.4 Leveling Mechanics

**Learn By Doing:**
- Use ability → Gain Skill XP
- Kill appropriate-level enemies → Bonus XP
- Complete skill-specific tasks → XP

**Level Caps:**
- Most skills: 50-70 level cap
- New content raises caps
- Unlock new abilities every 5-10 levels

**Ability Restrictions:**
- Cannot use abilities >25 levels apart
- Example: Level 50 Sword + Level 20 Fire Magic = Fire Magic 46+ abilities grayed out

---

## 7. World & Zones

### 7.1 Zone Progression

**Starting Zones:**
1. **Anagoge Island** (Tutorial) - Level 1-5
2. **Serbule** (Starter Town) - Level 5-20
3. **Serbule Hills** - Level 15-30

**Mid-Level Zones:**
4. **Eltibule** - Level 25-40
5. **Sun Vale** - Level 30-45
6. **Ilmari** (Desert) - Level 40-55
7. **Kur Mountains** - Level 40-60

**High-Level Zones:**
8. **Gazluk** (Frozen Wasteland) - Level 50-70
9. **Rahu** (Rakshasa City) - Level 50-70
10. **Fae Realm** - Level 50-70

### 7.2 Dungeons

**Types:**
- **Solo Dungeons:** Serbule Crypt, Carpal Tunnels
- **Group Dungeons:** Goblin Dungeon, Wolf Cave, Sacred Grotto
- **High-Level Dungeons:** Kur Tower, Gazluk Keep, Winter Nexus
- **Raids:** Red Wing Casino, Labyrinth

**Dungeon Features:**
- Boss encounters with Rage attacks
- Loot chests
- Skill challenges (lockpicking, puzzle-solving)
- Respawning enemies

### 7.3 Points of Interest

**Animal Town** (Sun Vale)
- Beast Form exclusive area
- Trainers, storage, socializing
- 3-hour transformation requirement

**Outposts:**
- Kur Mountains Fox Outpost
- Gazluk Animal Camp
- Warden Caves

---

## 8. NPC & Social Systems

### 8.1 Favor System

**Building Relationships:**
- Complete quests
- Gift items
- "Hang Outs" (timed activities with NPCs)

**Benefits:**
- Unlock training
- Better shop prices
- Storage access
- Quest chains
- Lore and backstory

### 8.2 Guilds

**Player Guilds:**
- Create/join guilds
- Guild halls
- Shared storage
- Guild quests

**NPC Factions:**
- Council (Humans)
- Wardens (Beast Form protectors)
- Rakshasa Houses

### 8.3 Quests

**Types:**
1. **Story Quests:** Main narrative
2. **Favor Quests:** Build NPC relationships
3. **Hunting Quests:** Kill X enemies
4. **Guild Quests:** Special guild objectives
5. **Seasonal Events:** Timed special events

---

## 9. Economy & Trading

### 9.1 Currency

**Councils** (Gold equivalent)
- Standard currency
- Earned from selling, quests, looting
- Used for purchases, training

### 9.2 Trading Systems

**Merchants:**
- Buy/sell with NPC shopkeepers
- Prices vary by Favor level
- Some merchants Animal-only or humanoid-only

**Bartering:**
- Direct trade system with NPCs
- "I'll give you X if you give me Y"
- Some NPCs prefer bartering to cash

**Consignment:**
- Player-to-player market
- List items for sale
- Players browse and buy

**Used Gear:**
- NPCs buy used equipment
- Different NPCs specialize in different gear types

### 9.3 Crafting Economy

**Material Flow:**
```
Gather Raw Materials (Mining, Foraging, Butchering)
    ↓
Process Materials (Tanning, Milling, Smelting)
    ↓
Craft Equipment/Consumables
    ↓
Augment/Enhance (Augmentation Skills)
    ↓
Sell or Use
```

**Key Crafting Chains:**
- Leather: Skin → Tan → Leatherwork
- Metal: Mine → Smelt → Blacksmith → Bladesmithing
- Food: Forage/Farm → Cook/Brew/Cheesemaking
- Potions: Forage → Alchemy/Battle Chemistry

---

## 10. Technical Implementation Plan

### 10.1 Development Phases

#### **Phase 1: Core Systems (Months 1-3)**
**Goal:** Playable combat prototype

**Milestones:**
1. Character creation (4 races)
2. Basic movement and camera
3. Combat system (Armor/Health/Power)
4. Dual-skill system framework
5. 2-3 combat skills implemented (e.g., Sword, Fire Magic, Archery)
6. Basic enemy AI
7. Hit/Wound/Save/Ward system
8. Critical hits and vulnerability

**Technical Stack:**
- **Engine:** Unity or Unreal Engine 5
- **Language:** C# (Unity) or C++ (Unreal)
- **Networking:** Photon, Mirror, or built-in multiplayer

#### **Phase 2: World & Content (Months 4-6)**
**Goal:** First zone playable end-to-end

**Milestones:**
1. Anagoge Island (tutorial)
2. Serbule (first town)
3. 5-10 NPCs with dialog
4. Basic favor system
5. Quest system (10-15 starter quests)
6. First dungeon (Serbule Crypt)
7. Loot system
8. Inventory management

#### **Phase 3: Equipment & Progression (Months 7-9)**
**Goal:** Deep character progression

**Milestones:**
1. Equipment with Treasure Effects
2. 10+ skills implemented
3. Skill trainers and ability unlocks
4. Crafting system (3-5 crafting skills)
5. Augmentation system
6. Armor sets (10+ sets)
7. Gems and crafting modifiers

#### **Phase 4: Beast Forms (Months 10-12)**
**Goal:** Unique transformation system

**Milestones:**
1. Beast Form framework
2. 3-4 Beast Forms (Cow, Spider, Deer, Lycanthropy)
3. Beast Speech system
4. Animal-specific equipment
5. Animal Town
6. NPC relationship changes
7. Beast Form skills

#### **Phase 5: Social & Economy (Months 13-15)**
**Goal:** Living world economy

**Milestones:**
1. Player trading
2. Consignment system
3. Guilds
4. Hang Outs
5. More zones (Eltibule, Kur Mountains)
6. More dungeons
7. Seasonal events

#### **Phase 6: Polish & Expansion (Months 16-18)**
**Goal:** Release-ready

**Milestones:**
1. Balance pass on all skills
2. More Beast Forms (Rabbit, Bat, Spirit Fox, Pig)
3. High-level zones
4. Raid content
5. PvP systems
6. Weather/day-night cycles
7. Advanced NPC AI

### 10.2 Technology Stack

#### **Game Engine Options**

**Option 1: Unity**
- **Pros:** Large community, C#, good 2D/3D support, asset store
- **Cons:** Licensing changes, some performance concerns
- **Recommended for:** Smaller team, faster prototyping

**Option 2: Unreal Engine 5**
- **Pros:** Stunning graphics, powerful tools, Blueprints + C++, free until revenue
- **Cons:** Steeper learning curve, larger builds
- **Recommended for:** High-fidelity graphics, experienced team

**Option 3: Godot 4**
- **Pros:** Open source, GDScript + C#, lightweight
- **Cons:** Smaller community, less mature for large MMO
- **Recommended for:** Indie team, full control

#### **Networking Architecture**

**Client-Server Model:**
```
[Client] ←→ [Game Server] ←→ [Database]
                ↓
         [Auth Server]
                ↓
          [Web API]
```

**Key Components:**
- **Game Server:** Handles combat, movement, NPCs
- **Database:** PostgreSQL or MongoDB for persistence
- **Auth Server:** Player accounts, login
- **Web API:** Leaderboards, guild info, marketplace

#### **Database Schema (High-Level)**

```sql
-- Players
Players (id, username, race, created_at, last_login)
Characters (id, player_id, name, race, level, x, y, zone)

-- Skills
CharacterSkills (character_id, skill_id, level, xp)
SkillAbilities (character_id, ability_id, unlocked)

-- Equipment
EquipmentSlots (character_id, slot, item_id)
Items (id, name, type, treasure_effects[])

-- Social
Favor (character_id, npc_id, level, xp)
Guilds (id, name, created_at)
GuildMembers (guild_id, character_id, rank)

-- Economy
Transactions (id, seller_id, buyer_id, item_id, price, timestamp)
ConsignmentListings (id, seller_id, item_id, price, listed_at)
```

### 10.3 Art Style

**Options:**

**Stylized/Cartoony**
- Lower poly count
- Faster iteration
- Ages better
- Examples: WoW, Albion Online

**Realistic**
- Higher fidelity
- More immersive
- More expensive
- Examples: ESO, New World

**Pixel Art / 2D**
- Nostalgic
- Much faster development
- Smaller team
- Examples: Stardew Valley, Terraria

**Recommendation:** Stylized 3D (best balance for MMO)

### 10.4 Core Systems Architecture

#### **Skill System**
```csharp
public abstract class Skill {
    public string Name;
    public int Level;
    public int XP;
    public List<Ability> Abilities;

    public abstract void Use(Ability ability, Target target);
    public virtual void GainXP(int amount) { /* ... */ }
}

public class Ability {
    public string Name;
    public int LevelRequired;
    public int PowerCost;
    public float Cooldown;
    public List<AbilityTag> Tags; // BasicAttack, NiceAttack, etc.

    public abstract void Execute(Character caster, Target target);
}
```

#### **Combat System**
```csharp
public class CombatSystem {
    public DamageResult PerformAttack(Character attacker, Character defender, Weapon weapon) {
        // 1. Hit Roll
        int hitRoll = RollD6();
        if (hitRoll == 1) return Miss;
        if (hitRoll < weapon.HitTarget) return Miss;

        bool isCrit = (hitRoll == 6);
        int hits = isCrit && weapon.HasAbility("Crit (2 Hits)") ? 2 : 1;

        int totalDamage = 0;
        for (int i = 0; i < hits; i++) {
            // 2. Wound Roll
            bool autoWound = isCrit && weapon.HasAbility("Crit (Auto-wound)");
            if (!autoWound) {
                int woundRoll = RollD6();
                if (woundRoll == 1 || woundRoll < weapon.WoundTarget) continue;
            }

            // 3. Save Roll
            int saveRoll = RollD6();
            int modifiedSave = defender.Save + weapon.Rend;
            if (saveRoll != 1 && saveRoll >= modifiedSave) continue;

            // 4. Ward Save
            if (defender.Ward != null) {
                int wardRoll = RollD6();
                if (wardRoll >= defender.Ward) continue;
            }

            // 5. Apply Damage
            totalDamage += weapon.Damage;
        }

        defender.TakeDamage(totalDamage);
        return new DamageResult(totalDamage, isCrit);
    }
}
```

#### **Equipment System**
```csharp
public class Equipment {
    public string Name;
    public EquipmentSlot Slot;
    public int Armor;
    public List<TreasureEffect> TreasureEffects;
}

public class TreasureEffect {
    public string Description; // "After using Sword Parry, restore 10 Health"
    public TriggerType Trigger; // AfterAbility, OnHit, OnCrit, etc.
    public string TriggerAbility; // "Sword Parry"
    public EffectType Effect; // RestoreHealth, IncreaseDamage, etc.
    public int Value;
    public float Duration;
}

public class TreasureEffectSystem {
    public void ProcessEffects(Character character, GameEvent gameEvent) {
        foreach (var equipment in character.EquippedItems) {
            foreach (var effect in equipment.TreasureEffects) {
                if (effect.Trigger.Matches(gameEvent)) {
                    ApplyEffect(character, effect);
                }
            }
        }
    }
}
```

#### **Beast Form System**
```csharp
public class BeastForm {
    public string Name;
    public BeastFormType Type; // Cow, Deer, Spider, etc.
    public bool IsPermanent; // Lycanthropy = true
    public List<Ability> Abilities;
    public List<string> RestrictedNPCs;

    public virtual void Transform(Character character) {
        character.CurrentForm = this;
        character.DisableWeaponSkills();
        character.EnableBeastSkills();
        UpdateNPCRelationships(character);
    }
}

public class BeastSpeech {
    private int skillLevel;

    public string TransformMessage(string message) {
        if (skillLevel >= 70) return message; // No transformation

        float replacePercent = 1.0f - (skillLevel / 100.0f);
        string[] words = message.Split(' ');

        for (int i = 0; i < words.Length; i++) {
            if (Random.value < replacePercent) {
                words[i] = GetAnimalSound(words[i].Length);
            }
        }

        return string.Join(" ", words);
    }
}
```

### 10.5 Performance Considerations

**Target Specs:**
- 60 FPS on mid-range hardware
- 30-50 players visible simultaneously
- 100+ NPCs in a town
- Seamless zone transitions (or quick load screens)

**Optimization Strategies:**
- Level of Detail (LOD) system
- Occlusion culling
- Object pooling for projectiles/effects
- Async loading for assets
- Server-side authoritative combat (prevent cheating)
- Client-side prediction for movement
- Delta compression for network traffic

### 10.6 Monetization (Optional)

**Ethical F2P Options:**
- **Cosmetics:** Skins, dyes, pets (visual only)
- **Convenience:** Extra storage, fast travel
- **Expansions:** New zones, skills as DLC

**Avoid:**
- Pay-to-win gear
- Loot boxes with gameplay advantages
- Required grinding that can be skipped with money

**Recommended Model:**
- **B2P (Buy-to-Play)** with optional cosmetic shop
- Or **Subscription** for all content access
- Or **F2P** with ethical cosmetics only

---

## Conclusion

This design document provides a comprehensive blueprint for creating a Project Gorgon-inspired game. The key to success will be:

1. **Start Small:** Build core combat and skill systems first
2. **Iterate:** Test with players early and often
3. **Prioritize Depth:** A few deep systems beat many shallow ones
4. **Community:** Build with player feedback
5. **Polish:** Make the core loop feel great before expanding

**Estimated Timeline:** 18-24 months to MVP with a small team (3-5 developers)

**Next Steps:**
1. Choose engine and tech stack
2. Build combat prototype
3. Implement 2-3 skills
4. Playtest and iterate
5. Expand content gradually

---

## Appendices

### Appendix A: Full Skill List

*See [Skills System](#6-skills-system) for complete breakdown*

### Appendix B: NPC Relationship Matrix

*Complex matrix of which NPCs speak to which Beast Forms - see scraped data*

### Appendix C: Complete Equipment Tables

*Full armor set statistics from scraped wiki data*

### Appendix D: Zone Maps

*To be designed based on final world layout*

---

**Document Version:** 1.0
**Last Updated:** 2025-11-06
**Author:** Claude (AI Assistant)
**Based On:** Project Gorgon Wiki Data (103 pages scraped)
