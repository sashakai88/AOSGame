# Unreal Engine 5 Implementation Guide
## Project Gorgon-Inspired MMORPG

This guide provides step-by-step instructions for implementing the game design in Unreal Engine 5.

---

## Table of Contents

1. [Initial Setup](#1-initial-setup)
2. [Project Structure](#2-project-structure)
3. [Phase 1: Core Combat Systems](#3-phase-1-core-combat-systems)
4. [Character System](#4-character-system)
5. [Combat Implementation](#5-combat-implementation)
6. [Skill System](#6-skill-system)
7. [Blueprint Integration](#7-blueprint-integration)
8. [Testing & Debugging](#8-testing--debugging)

---

## 1. Initial Setup

### 1.1 Install Unreal Engine 5

1. Download **Epic Games Launcher**
2. Install **Unreal Engine 5.3+** (latest stable)
3. Install **Visual Studio 2022** with:
   - Desktop development with C++
   - Game development with C++
   - .NET desktop development

### 1.2 Create New Project

**Project Settings:**
- Template: **Third Person** (C++)
- Target Platform: **Desktop**
- Quality Preset: **Scalable**
- Starter Content: **Yes**
- Raytracing: **Disabled** (for broader compatibility)

**Project Name:** `GorgonChronicles`

**Project Location:** `C:/Projects/GorgonChronicles`

### 1.3 Initial Configuration

**Edit → Project Settings:**

**Engine - Input:**
- Enhanced Input: **Enabled**
- Action Mappings: Will create later

**Engine - Collision:**
- Create custom collision channels:
  - `PlayerProjectile`
  - `EnemyProjectile`
  - `Interactable`

**Project - Description:**
- Project Name: Gorgon Chronicles
- Project Version: 0.1.0
- Description: "Project Gorgon-inspired MMORPG"

---

## 2. Project Structure

### 2.1 Folder Organization

```
GorgonChronicles/
├── Config/              (Project settings)
├── Content/
│   ├── Art/
│   │   ├── Characters/
│   │   ├── Creatures/
│   │   ├── Environment/
│   │   ├── UI/
│   │   └── VFX/
│   ├── Blueprints/
│   │   ├── Characters/
│   │   ├── Combat/
│   │   ├── Skills/
│   │   ├── UI/
│   │   └── World/
│   ├── Data/
│   │   ├── Skills/
│   │   ├── Abilities/
│   │   ├── Items/
│   │   └── NPCs/
│   ├── Maps/
│   │   ├── Anagoge/
│   │   ├── Serbule/
│   │   └── TestMaps/
│   └── UI/
│       ├── HUD/
│       ├── CharacterCreation/
│       └── Menus/
├── Plugins/             (Third-party plugins)
└── Source/
    └── GorgonChronicles/
        ├── Characters/
        ├── Combat/
        ├── Skills/
        ├── Equipment/
        ├── World/
        └── GorgonChronicles.h
```

### 2.2 Source Code Structure

**Key C++ Classes to Create:**

**Characters:**
- `AGCCharacterBase` - Base character class
- `AGCPlayerCharacter` - Player character
- `AGCEnemy` - Enemy base class
- `AGCRace` - Race data (Human, Elf, Rakshasa, Fairy)

**Combat:**
- `UGCCombatComponent` - Handles combat logic
- `UGCAttributeSet` - Armor, Health, Power stats
- `UGCDamageCalculator` - Hit/Wound/Save calculations
- `AGCProjectile` - Ranged attacks

**Skills:**
- `UGCSkill` - Base skill class
- `UGCAbility` - Base ability class
- `UGCSkillManager` - Manages active skills
- `UGCAbilityData` - Data asset for abilities

**Equipment:**
- `UGCEquipmentComponent` - Manages equipped items
- `UGCItem` - Base item class
- `UGCTreasureEffect` - Equipment modifiers

---

## 3. Phase 1: Core Combat Systems

**Goal:** Build a playable combat prototype with Armor/Health/Power

### 3.1 Milestone Checklist

- [ ] Character movement and camera
- [ ] Armor/Health/Power attribute system
- [ ] Basic melee attack (Sword skill)
- [ ] Basic ranged attack (Fire Magic skill)
- [ ] Enemy AI with basic combat
- [ ] Hit/Wound/Save/Ward system
- [ ] Damage numbers and combat feedback
- [ ] Simple UI showing stats

### 3.2 Timeline

**Week 1-2:** Project setup, character movement, camera
**Week 3-4:** Attribute system (Armor/Health/Power)
**Week 5-6:** Combat system implementation
**Week 7-8:** Enemy AI and testing

---

## 4. Character System

### 4.1 Base Character Class

**File:** `Source/GorgonChronicles/Characters/GCCharacterBase.h`

See detailed implementation in code files section below.

**Key Features:**
- Armor, Health, Power attributes
- Damage calculation
- Combat state management
- Animation integration

### 4.2 Race System

**Create Data Table:** `Content/Data/DT_Races`

**Race Data Structure:**
```cpp
USTRUCT(BlueprintType)
struct FGCRaceData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RaceName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BasePower = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseArmor = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> StartingSkills;

    // Human: Bonus when socialized
    // Elf: Bonus when clean
    // Rakshasa: Bonus when peaceful
    // Fairy: Special abilities
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ERaceBonus RaceBonus;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 InventorySlots = 64; // Fairies = 48
};
```

### 4.3 Player Character

**Enhanced Input System:**

Create Input Actions:
- `IA_Move` - Movement (2D Vector)
- `IA_Look` - Camera (2D Vector)
- `IA_Jump` - Jump
- `IA_Ability1` - Primary ability
- `IA_Ability2` - Secondary ability
- `IA_Ability3-8` - Additional abilities
- `IA_Interact` - Interact with NPCs/objects

---

## 5. Combat Implementation

### 5.1 Attribute System

**Using Gameplay Ability System (GAS)?**

**Option A: Use GAS** (Recommended for complex RPG)
- Powerful, industry-standard
- Built for multiplayer
- Steeper learning curve
- Best for scaling

**Option B: Custom System** (Simpler initially)
- Easier to understand
- Full control
- May need refactoring later
- Good for prototype

**Recommendation:** Start with Custom, migrate to GAS later if needed

### 5.2 Combat Component

**File:** `Source/GorgonChronicles/Combat/GCCombatComponent.h`

**Responsibilities:**
- Process attacks
- Calculate damage
- Apply effects
- Manage combat state
- Handle regeneration

### 5.3 Damage Calculation

**The Project Gorgon Formula:**

```
1. Roll Hit (1d6)
   - 1 = Auto Miss
   - < Hit Target = Miss
   - 6 = Critical

2. Roll Wound (1d6) [If Crit(Auto-wound), skip this]
   - 1 = Auto Fail
   - < Wound Target = Fail

3. Roll Save (1d6)
   - Modified by Rend
   - 1 = Auto Fail
   - >= Modified Save = Saved

4. Roll Ward (1d6) [If defender has ward]
   - >= Ward Value = Saved

5. Apply Damage
```

**Implementation Notes:**
- Use `FRandomStream` for deterministic randomness (important for networking)
- Log all rolls for debugging
- Visual feedback for each step
- Sound effects for crits, misses, etc.

### 5.4 Weapon Data

**Create Data Asset:** `UGCWeaponData`

```cpp
UCLASS(BlueprintType)
class UGCWeaponData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName WeaponName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Attacks = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 HitTarget = 4; // 4+ to hit

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 WoundTarget = 4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Rend = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Damage = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Range = 150.0f; // cm (Melee = 150cm)

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> Abilities; // "Crit (2 Hits)", "Crit (Auto-wound)", etc.
};
```

---

## 6. Skill System

### 6.1 Skill Architecture

**Dual-Skill System:**
- Player has 2 active combat skills
- Each skill provides 6-8 abilities
- Abilities have levels, cooldowns, power costs

**Skill Progression:**
- XP gained by using abilities
- Level up unlocks new abilities
- Abilities become more powerful

### 6.2 Skill Data Asset

**File:** `Content/Data/Skills/DA_Sword.uasset`

```cpp
UCLASS(BlueprintType)
class UGCSkillData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkillName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxLevel = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FGCAbilityUnlock> Abilities;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon;
};

USTRUCT(BlueprintType)
struct FGCAbilityUnlock
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 UnlockLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UGCAbility> AbilityClass;
};
```

### 6.3 Ability Base Class

**File:** `Source/GorgonChronicles/Skills/GCAbility.h`

```cpp
UCLASS(Blueprintable)
class UGCAbility : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AbilityName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PowerCost = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Cooldown = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Range = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<EAbilityTag> Tags; // BasicAttack, NiceAttack, etc.

    UFUNCTION(BlueprintCallable)
    virtual bool CanActivate(AGCCharacterBase* Caster);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void Activate(AGCCharacterBase* Caster, AActor* Target);

    UFUNCTION(BlueprintCallable)
    void StartCooldown();

protected:
    float RemainingCooldown = 0.0f;
};
```

### 6.4 Example Ability: Sword Slash

**Blueprint:** `Content/Blueprints/Skills/Sword/BP_Ability_SwordSlash`

**C++ Parent:** `UGCSwordSlashAbility`

**Activation:**
1. Check: Enough Power? Not on cooldown? Target in range?
2. Consume Power
3. Play animation (Montage)
4. Spawn damage event at sword tip
5. Apply damage if hit
6. Start cooldown
7. Grant Skill XP

---

## 7. Blueprint Integration

### 7.1 Using C++ with Blueprints

**Philosophy:**
- **C++ for logic**: Combat calculations, data structures, networking
- **Blueprints for content**: Abilities, UI, animation events, VFX

**Exposing to Blueprint:**
```cpp
UFUNCTION(BlueprintCallable, Category = "Combat")
void DealDamage(AActor* Target, float DamageAmount);

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
float CurrentHealth;

UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
void OnHealthChanged(float NewHealth, float MaxHealth);
```

### 7.2 Character Blueprint

**Create:** `Content/Blueprints/Characters/BP_PlayerCharacter`

**Parent Class:** `AGCPlayerCharacter` (C++)

**Components:**
- Spring Arm
- Camera
- Combat Component
- Skill Manager Component
- Equipment Component

**Event Graph:**
- Input handling
- Animation state
- UI updates
- VFX spawning

### 7.3 UI System

**Main HUD:** `Content/UI/HUD/WBP_MainHUD`

**Widgets:**
- Health/Armor/Power bars
- Ability cooldowns
- Target info
- Combat log

**Use Common UI Plugin** (Epic's standard for UI)

---

## 8. Testing & Debugging

### 8.1 Test Map

**Create:** `Content/Maps/TestMaps/TM_CombatPrototype`

**Contents:**
- Player start
- 3-5 test enemies (different difficulties)
- Damage target dummies
- UI debug displays

### 8.2 Debug Commands

**Console Commands to Implement:**

```cpp
UFUNCTION(Exec)
void SetHealth(float Amount);

UFUNCTION(Exec)
void SetPower(float Amount);

UFUNCTION(Exec)
void SetArmor(float Amount);

UFUNCTION(Exec)
void GodMode();

UFUNCTION(Exec)
void KillAllEnemies();

UFUNCTION(Exec)
void SpawnEnemy(FString EnemyName);

UFUNCTION(Exec)
void AddSkillXP(FString SkillName, int32 Amount);
```

**Usage:**
- Press ` (tilde) to open console
- Type command: `SetHealth 100`

### 8.3 Debug Displays

**On-Screen Debug:**
```cpp
// In Tick or combat functions
if (GEngine)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
        FString::Printf(TEXT("Health: %.0f/%.0f"), CurrentHealth, MaxHealth));
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue,
        FString::Printf(TEXT("Armor: %.0f"), CurrentArmor));
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan,
        FString::Printf(TEXT("Power: %.0f/%.0f"), CurrentPower, MaxPower));
}
```

### 8.4 Testing Checklist

**Combat System Tests:**
- [ ] Player can deal damage to enemy
- [ ] Enemy can deal damage to player
- [ ] Armor reduces damage correctly (1 dmg per 25 armor)
- [ ] Health reaches 0 = death
- [ ] Power is consumed by abilities
- [ ] Cooldowns work correctly
- [ ] Critical hits trigger (6 on d6)
- [ ] Critical effects work (2 Hits, Auto-wound)
- [ ] Ward saves work
- [ ] Regeneration works (in and out of combat)

---

## Next Steps

1. **Set up Unreal Engine 5 project**
2. **Create base character class** (following code examples)
3. **Implement combat component**
4. **Create test enemy**
5. **Build simple UI**
6. **Test combat loop**

Once core combat is working, we'll move to:
- Skill system
- Multiple abilities
- Enemy AI
- Beast Forms
- Equipment system

---

## Additional Resources

### UE5 Documentation
- [Character Movement Component](https://docs.unrealengine.com/5.3/en-US/character-movement-component-in-unreal-engine/)
- [Enhanced Input](https://docs.unrealengine.com/5.3/en-US/enhanced-input-in-unreal-engine/)
- [Gameplay Ability System](https://docs.unrealengine.com/5.3/en-US/gameplay-ability-system-for-unreal-engine/)
- [Animation Blueprints](https://docs.unrealengine.com/5.3/en-US/animation-blueprints-in-unreal-engine/)
- [UMG UI Designer](https://docs.unrealengine.com/5.3/en-US/umg-ui-designer-for-unreal-engine/)

### Community Resources
- Unreal Slackers Discord
- UE5 Subreddit (r/unrealengine)
- Tom Looman's Blog (gameplay programming)
- Multiplayer Network Compendium

---

**Next Document:** See `UE5_CODE_TEMPLATES.md` for complete C++ class implementations
