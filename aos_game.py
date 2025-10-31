#!/usr/bin/env python3
"""
Age of Sigmar 4th Edition - Battle Simulator
A tactical combat game implementing AOS 4e core rules

Controls:
- Left Click: Select units, move, attack
- Right Click: Cancel selection
- M: Normal Move
- R: Run
- T: Retreat
- Space: Next Phase
- P: Pause Timer
- ESC: Quit
"""

import pygame
import random
import math
from enum import Enum
from typing import List, Optional, Tuple
from dataclasses import dataclass, field

# Initialize Pygame
pygame.init()

# Constants
SCREEN_WIDTH = 1400
SCREEN_HEIGHT = 800
BATTLEFIELD_WIDTH = 800
BATTLEFIELD_HEIGHT = 600
BATTLEFIELD_X = 250
BATTLEFIELD_Y = 20

# Colors
COLOR_BG = (26, 26, 46)
COLOR_BG_LIGHT = (30, 30, 60)
COLOR_GOLD = (199, 156, 63)
COLOR_GOLD_LIGHT = (255, 215, 0)
COLOR_ORDER = (74, 144, 226)
COLOR_CHAOS = (226, 74, 74)
COLOR_GREEN = (74, 226, 74)
COLOR_RED = (226, 74, 74)
COLOR_WHITE = (224, 224, 224)
COLOR_GRAY = (128, 128, 128)

# Pixels per inch conversion
PIXELS_PER_INCH = 50

# Game settings
PHASE_TIMER_SECONDS = 30
AUTO_SKIP_DELAY_MS = 800


class WeaponType(Enum):
    MELEE = "MELEE"
    RANGED = "RANGED"


class UnitType(Enum):
    INFANTRY = "INFANTRY"
    CAVALRY = "CAVALRY"
    MONSTER = "MONSTER"
    WAR_MACHINE = "WAR_MACHINE"


class Phase(Enum):
    HERO = 0
    MOVEMENT = 1
    SHOOTING = 2
    CHARGE = 3
    COMBAT = 4
    TURN_END = 5


@dataclass
class Weapon:
    """Weapon with stats and abilities"""
    weapon_type: WeaponType
    attacks: int
    hit: int  # Target number (3+ = 3)
    wound: int
    rend: int
    damage: int
    range_inches: int = 3  # 3" for melee
    abilities: List[str] = field(default_factory=list)

    def get_range_pixels(self) -> int:
        return self.range_inches * PIXELS_PER_INCH


class Unit:
    """Unit with stats, weapons, and state"""

    def __init__(self, name: str, player: int, move: int, health: int, save: int,
                 control: int, unit_type: UnitType, keywords: List[str],
                 weapons: List[Weapon], x: int, y: int, ward: Optional[int] = None):
        self.name = name
        self.player = player
        self.move = move  # in pixels
        self.health = health
        self.max_health = health
        self.save = save
        self.control = control
        self.unit_type = unit_type
        self.keywords = keywords
        self.weapons = weapons
        self.x = x
        self.y = y
        self.ward = ward

        # State flags
        self.has_moved = False
        self.has_charged = False
        self.has_attacked = False
        self.has_run = False
        self.has_retreated = False
        self.strike_first = False
        self.strike_last = False

    def is_alive(self) -> bool:
        return self.health > 0

    def get_health_percent(self) -> float:
        return (self.health / self.max_health) * 100 if self.max_health > 0 else 0

    def distance_to(self, other: 'Unit') -> float:
        dx = self.x - other.x
        dy = self.y - other.y
        return math.sqrt(dx * dx + dy * dy)

    def in_combat_range(self, other: 'Unit') -> bool:
        return self.distance_to(other) <= 3 * PIXELS_PER_INCH

    def is_in_combat(self, all_units: List['Unit']) -> bool:
        """Check if this unit is within 3" of any enemy unit"""
        return any(
            other.player != self.player and other.is_alive() and self.in_combat_range(other)
            for other in all_units
        )

    def has_ranged_weapon(self) -> bool:
        return any(w.weapon_type == WeaponType.RANGED for w in self.weapons)

    def is_hero(self) -> bool:
        return "HERO" in self.keywords

    def is_monster(self) -> bool:
        return "MONSTER" in self.keywords

    def reset(self):
        """Reset unit state at start of turn"""
        self.has_moved = False
        self.has_charged = False
        self.has_attacked = False
        self.has_run = False
        self.has_retreated = False
        self.strike_first = False
        self.strike_last = False

    def take_damage(self, amount: int):
        self.health = max(0, self.health - amount)


@dataclass
class Objective:
    """Objective marker for control"""
    x: int
    y: int
    controlled_by: int = 0  # 0 = neutral, 1 = player 1, 2 = player 2

    def distance_to_unit(self, unit: Unit) -> float:
        dx = self.x - unit.x
        dy = self.y - unit.y
        return math.sqrt(dx * dx + dy * dy)


class CombatLog:
    """Combat log with scrolling messages"""

    def __init__(self):
        self.entries = []
        self.max_entries = 50

    def add(self, message: str, log_type: str = "normal"):
        """Add entry: type can be 'normal', 'hit', 'miss', 'critical'"""
        self.entries.insert(0, (message, log_type))
        if len(self.entries) > self.max_entries:
            self.entries.pop()


class Game:
    """Main game class managing battle state and logic"""

    def __init__(self):
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("Age of Sigmar 4th Edition - Battle Simulator")
        self.clock = pygame.time.Clock()
        self.font_small = pygame.font.Font(None, 20)
        self.font_medium = pygame.font.Font(None, 24)
        self.font_large = pygame.font.Font(None, 32)

        # Game state
        self.units: List[Unit] = []
        self.objectives: List[Objective] = []
        self.selected_unit: Optional[Unit] = None
        self.battle_round = 1
        self.turn_in_round = 1
        self.current_player = 1
        self.phase = Phase.HERO
        self.mode = None  # 'move', 'run', 'retreat', 'charge', 'shoot', 'attack'

        # Timer
        self.time_remaining = PHASE_TIMER_SECONDS
        self.timer_paused = False
        self.timer_ticks = 0

        # Command points and victory points
        self.command_points = [4, 4]
        self.victory_points = [0, 0]

        # Combat log
        self.log = CombatLog()

        # State
        self.running = True
        self.auto_advancing = False
        self.charge_distance = 0

        # Initialize game
        self.create_units()
        self.create_objectives()
        self.log.add("Welcome to Age of Sigmar Battle Simulator!", "critical")
        self.log.add("Based on AOS 4th Edition Core Rules", "hit")

        # Check if initial phase should auto-skip
        if not self.can_any_unit_act_in_current_phase():
            self.log.add(f"{self.phase.name} Phase - auto-skipping...", "hit")
            pygame.time.set_timer(pygame.USEREVENT, AUTO_SKIP_DELAY_MS, True)
            self.auto_advancing = True

    def create_units(self):
        """Create starting units for both players"""
        # Player 1 - Order (Stormcast Eternals)
        self.units.append(Unit(
            name="Liberator Squad",
            player=1,
            move=4 * PIXELS_PER_INCH,
            health=12,
            save=4,
            control=2,
            unit_type=UnitType.INFANTRY,
            keywords=["ORDER", "STORMCAST ETERNALS"],
            weapons=[
                Weapon(WeaponType.MELEE, 3, 3, 4, 1, 2, 3, []),
                Weapon(WeaponType.RANGED, 2, 4, 4, 0, 1, 6, [])
            ],
            x=BATTLEFIELD_X + 100,
            y=BATTLEFIELD_Y + 100,
            ward=6
        ))

        self.units.append(Unit(
            name="Prosecutor Squad",
            player=1,
            move=5 * PIXELS_PER_INCH,
            health=8,
            save=5,
            control=1,
            unit_type=UnitType.INFANTRY,
            keywords=["ORDER", "STORMCAST ETERNALS", "FLY"],
            weapons=[
                Weapon(WeaponType.RANGED, 3, 4, 4, 1, 1, 18, []),
                Weapon(WeaponType.MELEE, 2, 4, 4, 0, 1, 3, [])
            ],
            x=BATTLEFIELD_X + 100,
            y=BATTLEFIELD_Y + 250,
            ward=6
        ))

        self.units.append(Unit(
            name="Knight-Arcanum",
            player=1,
            move=4 * PIXELS_PER_INCH,
            health=15,
            save=3,
            control=2,
            unit_type=UnitType.INFANTRY,
            keywords=["ORDER", "STORMCAST ETERNALS", "HERO"],
            weapons=[
                Weapon(WeaponType.MELEE, 4, 3, 3, 2, 3, 3, ["Crit (2 Hits)"])
            ],
            x=BATTLEFIELD_X + 100,
            y=BATTLEFIELD_Y + 400,
            ward=6
        ))

        # Player 2 - Chaos
        self.units.append(Unit(
            name="Chaos Warriors",
            player=2,
            move=4 * PIXELS_PER_INCH,
            health=10,
            save=4,
            control=2,
            unit_type=UnitType.INFANTRY,
            keywords=["CHAOS", "SLAVES TO DARKNESS"],
            weapons=[
                Weapon(WeaponType.MELEE, 3, 4, 3, 1, 1, 3, ["Anti-Infantry (+1 Rend)"])
            ],
            x=BATTLEFIELD_X + 650,
            y=BATTLEFIELD_Y + 100,
            ward=None
        ))

        self.units.append(Unit(
            name="Bloodletters",
            player=2,
            move=4 * PIXELS_PER_INCH,
            health=6,
            save=6,
            control=1,
            unit_type=UnitType.INFANTRY,
            keywords=["CHAOS", "KHORNE", "DAEMON"],
            weapons=[
                Weapon(WeaponType.MELEE, 3, 4, 4, 1, 1, 3, ["Charge (+1 Damage)"])
            ],
            x=BATTLEFIELD_X + 650,
            y=BATTLEFIELD_Y + 250,
            ward=5
        ))

        self.units.append(Unit(
            name="Chaos Lord",
            player=2,
            move=4 * PIXELS_PER_INCH,
            health=18,
            save=3,
            control=2,
            unit_type=UnitType.INFANTRY,
            keywords=["CHAOS", "SLAVES TO DARKNESS", "HERO"],
            weapons=[
                Weapon(WeaponType.MELEE, 5, 3, 3, 2, 2, 3, ["Crit (Auto-wound)"])
            ],
            x=BATTLEFIELD_X + 650,
            y=BATTLEFIELD_Y + 400,
            ward=None
        ))

    def create_objectives(self):
        """Create objective markers"""
        self.objectives = [
            Objective(BATTLEFIELD_X + 375, BATTLEFIELD_Y + 150),  # Center-top
            Objective(BATTLEFIELD_X + 375, BATTLEFIELD_Y + 450),  # Center-bottom
            Objective(BATTLEFIELD_X + 200, BATTLEFIELD_Y + 300),  # Left
            Objective(BATTLEFIELD_X + 550, BATTLEFIELD_Y + 300),  # Right
        ]

    def roll_d6(self) -> int:
        return random.randint(1, 6)

    def roll_d3(self) -> int:
        return (self.roll_d6() + 1) // 2

    def handle_events(self):
        """Handle pygame events"""
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.running = False
            elif event.type == pygame.USEREVENT:
                # Handle auto-advance timer
                if self.auto_advancing:
                    self.auto_advancing = False
                    self.next_phase()
            elif event.type == pygame.KEYDOWN:
                self.handle_keypress(event.key)
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if event.button == 1:  # Left click
                    self.handle_left_click(event.pos)
                elif event.button == 3:  # Right click
                    self.selected_unit = None
                    self.mode = None

    def handle_keypress(self, key):
        """Handle keyboard input"""
        if key == pygame.K_ESCAPE:
            self.running = False
        elif key == pygame.K_SPACE:
            self.next_phase()
        elif key == pygame.K_p:
            self.toggle_timer()
        elif key == pygame.K_m and self.selected_unit and self.phase == Phase.MOVEMENT:
            self.mode = 'move'
            self.log.add(f"{self.selected_unit.name} ready to move", "hit")
        elif key == pygame.K_r and self.selected_unit and self.phase == Phase.MOVEMENT:
            self.mode = 'run'
            self.log.add(f"{self.selected_unit.name} ready to run", "hit")
        elif key == pygame.K_t and self.selected_unit and self.phase == Phase.MOVEMENT:
            if self.selected_unit.is_in_combat(self.units):
                self.mode = 'retreat'
                self.log.add(f"{self.selected_unit.name} ready to retreat", "hit")

    def handle_left_click(self, pos: Tuple[int, int]):
        """Handle left mouse click"""
        # Check if clicking on battlefield
        if (BATTLEFIELD_X <= pos[0] <= BATTLEFIELD_X + BATTLEFIELD_WIDTH and
            BATTLEFIELD_Y <= pos[1] <= BATTLEFIELD_Y + BATTLEFIELD_HEIGHT):

            # Check if clicking on a unit
            clicked_unit = self.get_unit_at_pos(pos)

            if clicked_unit:
                if clicked_unit.player == self.current_player:
                    self.select_unit(clicked_unit)
                elif self.selected_unit and (self.mode == 'shoot' or self.mode == 'attack'):
                    # Attacking enemy
                    if self.phase == Phase.SHOOTING:
                        self.perform_shooting_attack(self.selected_unit, clicked_unit)
                    elif self.phase == Phase.COMBAT:
                        self.perform_melee_attack(self.selected_unit, clicked_unit)
            elif self.selected_unit and self.mode in ['move', 'run', 'retreat', 'charge']:
                # Moving unit
                self.move_unit(self.selected_unit, pos[0] - BATTLEFIELD_X, pos[1] - BATTLEFIELD_Y)

    def get_unit_at_pos(self, pos: Tuple[int, int]) -> Optional[Unit]:
        """Get unit at mouse position"""
        for unit in self.units:
            if not unit.is_alive():
                continue
            dx = pos[0] - unit.x
            dy = pos[1] - unit.y
            if dx * dx + dy * dy <= 625:  # 25 pixel radius
                return unit
        return None

    def select_unit(self, unit: Unit):
        """Select a unit and set appropriate mode based on phase"""
        if not unit.is_alive() or unit.player != self.current_player:
            return

        self.selected_unit = unit

        if self.phase == Phase.HERO:
            self.log.add(f"{unit.name} selected. Hero Phase - no actions", "hit")
            self.mode = None

        elif self.phase == Phase.MOVEMENT:
            if unit.has_moved or unit.has_run or unit.has_retreated:
                self.log.add(f"{unit.name} has already moved!", "miss")
                self.mode = None
            elif unit.is_in_combat(self.units):
                self.log.add(f"{unit.name} in combat! Press T to Retreat (takes D3 mortal damage)", "miss")
                self.mode = None  # Wait for T key press
            else:
                self.log.add(f"{unit.name} selected. Press M (Move), R (Run), or click destination", "hit")
                self.mode = 'move'  # Default to move mode

        elif self.phase == Phase.SHOOTING:
            if not unit.has_ranged_weapon():
                self.log.add(f"{unit.name} has no ranged weapons!", "miss")
                self.mode = None
            elif unit.has_attacked:
                self.log.add(f"{unit.name} has already attacked!", "miss")
                self.mode = None
            elif unit.is_in_combat(self.units):
                self.log.add(f"{unit.name} is in combat and cannot shoot!", "miss")
                self.mode = None
            elif unit.has_run:
                self.log.add(f"{unit.name} ran this turn and cannot shoot!", "miss")
                self.mode = None
            else:
                self.mode = 'shoot'
                ranges = [w.range_inches for w in unit.weapons if w.weapon_type == WeaponType.RANGED]
                self.log.add(f"{unit.name} ready to shoot (ranges: {ranges}\")", "hit")

        elif self.phase == Phase.CHARGE:
            if unit.has_moved or unit.has_charged:
                self.log.add(f"{unit.name} cannot charge - already moved!", "miss")
                self.mode = None
            elif unit.has_retreated:
                self.log.add(f"{unit.name} cannot charge - retreated!", "miss")
                self.mode = None
            else:
                charge_roll = self.roll_d6() + self.roll_d6()
                self.charge_distance = charge_roll * PIXELS_PER_INCH
                self.mode = 'charge'
                self.log.add(f"{unit.name} charge roll: {charge_roll}\"", "critical")

        elif self.phase == Phase.COMBAT:
            if unit.has_attacked:
                self.log.add(f"{unit.name} has already fought!", "miss")
                self.mode = None
            else:
                self.mode = 'attack'
                self.log.add(f"{unit.name} ready to fight. Click enemy within 3\"", "hit")

    def move_unit(self, unit: Unit, new_x: int, new_y: int):
        """Move unit to new position"""
        distance = math.sqrt((new_x - (unit.x - BATTLEFIELD_X)) ** 2 +
                           (new_y - (unit.y - BATTLEFIELD_Y)) ** 2)

        # Calculate max move distance
        max_move = unit.move

        if self.mode == 'charge':
            max_move = self.charge_distance
        elif self.mode == 'run':
            run_bonus = self.roll_d6()
            max_move = unit.move + (run_bonus * PIXELS_PER_INCH)
            unit.has_run = True
            self.log.add(f"Run bonus: +{run_bonus}\"", "hit")
        elif self.mode == 'retreat':
            max_move = unit.move
            mortal_damage = self.roll_d3()
            unit.take_damage(mortal_damage)
            self.log.add(f"{unit.name} takes {mortal_damage} mortal damage!", "miss")
            unit.has_retreated = True

        if distance > max_move:
            self.log.add(f"Movement exceeds max distance!", "miss")
            return

        # Keep within bounds
        unit.x = BATTLEFIELD_X + max(0, min(BATTLEFIELD_WIDTH - 50, new_x))
        unit.y = BATTLEFIELD_Y + max(0, min(BATTLEFIELD_HEIGHT - 50, new_y))
        unit.has_moved = True

        if self.phase == Phase.CHARGE:
            unit.has_charged = True
            self.log.add(f"{unit.name} charged!", "hit")
        elif self.mode == 'retreat':
            self.log.add(f"{unit.name} retreated", "hit")
        elif self.mode == 'run':
            self.log.add(f"{unit.name} ran", "hit")
        else:
            self.log.add(f"{unit.name} moved", "hit")

        self.selected_unit = None
        self.mode = None
        self.check_phase_completion()

    def perform_shooting_attack(self, attacker: Unit, defender: Unit):
        """Perform ranged attack"""
        distance = attacker.distance_to(defender)
        ranged_weapons = [w for w in attacker.weapons if w.weapon_type == WeaponType.RANGED]

        if not ranged_weapons:
            self.log.add(f"{attacker.name} has no ranged weapons!", "miss")
            return

        viable_weapons = [w for w in ranged_weapons if distance <= w.get_range_pixels()]
        if not viable_weapons:
            self.log.add(f"{defender.name} not in range!", "miss")
            return

        self.log.add(f"{attacker.name} shoots at {defender.name}!", "critical")

        for weapon in viable_weapons:
            self.perform_weapon_attack(attacker, defender, weapon)

        attacker.has_attacked = True
        self.selected_unit = None
        self.mode = None
        self.check_phase_completion()

    def perform_melee_attack(self, attacker: Unit, defender: Unit):
        """Perform melee attack"""
        if not attacker.in_combat_range(defender):
            self.log.add(f"{defender.name} not within 3\" combat range!", "miss")
            return

        self.log.add(f"{attacker.name} attacks {defender.name} in melee!", "critical")

        melee_weapons = [w for w in attacker.weapons if w.weapon_type == WeaponType.MELEE]
        for weapon in melee_weapons:
            self.perform_weapon_attack(attacker, defender, weapon)

        attacker.has_attacked = True
        self.selected_unit = None
        self.mode = None
        self.check_phase_completion()

    def perform_weapon_attack(self, attacker: Unit, defender: Unit, weapon: Weapon):
        """Perform attack with specific weapon"""
        self.log.add(f"Using {weapon.weapon_type.value} weapon ({weapon.attacks}A)", "hit")

        damage_pool = []

        for i in range(weapon.attacks):
            # Hit roll
            hit_roll = self.roll_d6()
            is_crit = (hit_roll == 6)

            if hit_roll == 1:
                self.log.add(f"Attack {i+1}: Hit {hit_roll} - Auto fail!", "miss")
                continue

            if hit_roll < weapon.hit:
                self.log.add(f"Attack {i+1}: Hit {hit_roll} - Miss!", "miss")
                continue

            # Handle Crit (2 Hits)
            hits = 2 if is_crit and "Crit (2 Hits)" in weapon.abilities else 1
            if hits == 2:
                self.log.add(f"CRITICAL! Crit (2 Hits) - 2 hits!", "critical")

            for h in range(hits):
                # Wound roll
                auto_wound = is_crit and "Crit (Auto-wound)" in weapon.abilities

                if auto_wound:
                    self.log.add(f"CRITICAL! Auto-wound!", "critical")
                else:
                    wound_roll = self.roll_d6()
                    if wound_roll == 1:
                        self.log.add(f"Wound {wound_roll} - Auto fail!", "miss")
                        continue
                    if wound_roll < weapon.wound:
                        self.log.add(f"Wound {wound_roll} - Failed!", "miss")
                        continue

                # Calculate rend (Anti-X abilities)
                effective_rend = weapon.rend
                for ability in weapon.abilities:
                    if ability.startswith("Anti-") and "(+1 Rend)" in ability:
                        keyword = ability.split("-")[1].split(" ")[0].upper()
                        if (keyword in [k.upper() for k in defender.keywords] or
                            keyword in defender.unit_type.value):
                            effective_rend += 1
                            self.log.add(f"Anti-{keyword} activated! +1 Rend", "critical")

                # Save roll
                save_roll = self.roll_d6()
                modified_save = defender.save + effective_rend

                if save_roll != 1 and save_roll >= modified_save:
                    self.log.add(f"Save {save_roll} - Saved!", "hit")
                    continue

                # Calculate damage
                damage = weapon.damage

                # Charge bonus
                if attacker.has_charged and "Charge (+1 Damage)" in weapon.abilities:
                    damage += 1
                    self.log.add(f"Charge (+1 Damage) activated!", "critical")

                # Crit (Mortal)
                if is_crit and any("Crit (Mortal)" in a for a in weapon.abilities):
                    self.log.add(f"CRITICAL! {damage} mortal damage!", "critical")
                    defender.take_damage(damage)
                else:
                    damage_pool.append(damage)

        # Apply ward saves
        if damage_pool and defender.ward:
            self.log.add(f"Applying ward saves ({defender.ward}+)...", "hit")
            surviving_damage = []
            for dmg in damage_pool:
                ward_roll = self.roll_d6()
                if ward_roll < defender.ward:
                    surviving_damage.append(dmg)
                else:
                    self.log.add(f"Ward {ward_roll} - Saved!", "hit")
            damage_pool = surviving_damage

        # Deal damage
        total_damage = sum(damage_pool)
        if total_damage > 0:
            defender.take_damage(total_damage)
            self.log.add(f"{total_damage} damage dealt! {defender.name} has {defender.health} HP", "critical")

        if not defender.is_alive():
            self.log.add(f"{defender.name} has been slain!", "critical")
            self.check_victory()

    def check_victory(self):
        """Check if either player has won"""
        p1_alive = sum(1 for u in self.units if u.player == 1 and u.is_alive())
        p2_alive = sum(1 for u in self.units if u.player == 2 and u.is_alive())

        # For now just log, could add victory screen
        if p1_alive == 0:
            self.log.add("CHAOS WINS! All Order units destroyed!", "critical")
        elif p2_alive == 0:
            self.log.add("ORDER WINS! All Chaos units destroyed!", "critical")

    def score_objectives(self):
        """Score objectives at turn end"""
        self.log.add("=== SCORING OBJECTIVES ===", "critical")

        for i, obj in enumerate(self.objectives):
            p1_control = sum(u.control for u in self.units
                           if u.player == 1 and u.is_alive() and obj.distance_to_unit(u) <= 3 * PIXELS_PER_INCH)
            p2_control = sum(u.control for u in self.units
                           if u.player == 2 and u.is_alive() and obj.distance_to_unit(u) <= 3 * PIXELS_PER_INCH)

            prev_control = obj.controlled_by
            if p1_control > p2_control:
                obj.controlled_by = 1
            elif p2_control > p1_control:
                obj.controlled_by = 2

            if obj.controlled_by != prev_control:
                owner = "Order" if obj.controlled_by == 1 else "Chaos" if obj.controlled_by == 2 else "Neutral"
                self.log.add(f"Objective {i+1}: {owner} ({p1_control} vs {p2_control})", "hit")

        # Award VP
        p1_objectives = sum(1 for o in self.objectives if o.controlled_by == 1)
        p2_objectives = sum(1 for o in self.objectives if o.controlled_by == 2)

        self.victory_points[0] += p1_objectives
        self.victory_points[1] += p2_objectives

        self.log.add(f"Victory Points: Order {self.victory_points[0]} - Chaos {self.victory_points[1]}", "critical")

    def can_any_unit_act_in_current_phase(self) -> bool:
        """Check if any unit can act in current phase"""
        friendly_units = [u for u in self.units if u.player == self.current_player and u.is_alive()]
        enemy_units = [u for u in self.units if u.player != self.current_player and u.is_alive()]

        if self.phase == Phase.HERO:
            return False  # No abilities yet

        elif self.phase == Phase.MOVEMENT:
            return any(not u.has_moved and not u.is_in_combat(self.units) for u in friendly_units)

        elif self.phase == Phase.SHOOTING:
            if not enemy_units:
                return False
            for u in friendly_units:
                if (u.has_attacked or not u.has_ranged_weapon() or
                    u.is_in_combat(self.units) or u.has_run):
                    continue
                ranged_weapons = [w for w in u.weapons if w.weapon_type == WeaponType.RANGED]
                max_range = max(w.get_range_pixels() for w in ranged_weapons) if ranged_weapons else 0
                if any(u.distance_to(e) <= max_range for e in enemy_units):
                    return True
            return False

        elif self.phase == Phase.CHARGE:
            if not enemy_units:
                return False
            return any(not u.has_moved and not u.has_retreated and
                      any(u.distance_to(e) <= u.move and u.distance_to(e) > 3 * PIXELS_PER_INCH
                          for e in enemy_units)
                      for u in friendly_units)

        elif self.phase == Phase.COMBAT:
            if not enemy_units:
                return False
            return any(not u.has_attacked and any(u.in_combat_range(e) for e in enemy_units)
                      for u in friendly_units)

        elif self.phase == Phase.TURN_END:
            return False  # Always auto-skip

        return False

    def check_phase_completion(self):
        """Check if phase is complete and auto-advance"""
        if not self.can_any_unit_act_in_current_phase():
            self.log.add(f"{self.phase.name} Phase complete - auto-advancing...", "hit")
            pygame.time.set_timer(pygame.USEREVENT, AUTO_SKIP_DELAY_MS, True)
            self.auto_advancing = True

    def next_phase(self):
        """Advance to next phase"""
        # Score objectives at turn end
        if self.phase == Phase.TURN_END:
            self.score_objectives()

        # Advance phase
        phase_num = self.phase.value + 1

        if phase_num >= 6:  # End of turn
            phase_num = 0
            self.turn_in_round += 1

            if self.turn_in_round > 2:  # End of battle round
                self.battle_round += 1
                self.turn_in_round = 1
                self.command_points = [4, 4]
                self.log.add(f"=== BATTLE ROUND {self.battle_round} ===", "critical")

            self.current_player = 2 if self.current_player == 1 else 1

            # Reset units
            for unit in self.units:
                unit.reset()

            self.log.add(f"Player {self.current_player}'s Turn (Round {self.battle_round})", "critical")

        self.phase = Phase(phase_num)
        self.log.add(f"=== {self.phase.name} PHASE ===", "hit")
        self.time_remaining = PHASE_TIMER_SECONDS

        # Check if should auto-skip
        if not self.can_any_unit_act_in_current_phase():
            pygame.time.set_timer(pygame.USEREVENT, AUTO_SKIP_DELAY_MS, True)
            self.auto_advancing = True

    def toggle_timer(self):
        """Toggle timer pause state"""
        self.timer_paused = not self.timer_paused
        status = "paused" if self.timer_paused else "resumed"
        self.log.add(f"Timer {status}", "hit")

    def update(self):
        """Update game state"""
        # Update timer
        if not self.timer_paused and not self.auto_advancing:
            self.timer_ticks += 1
            if self.timer_ticks >= 60:  # 60 FPS = 1 second
                self.timer_ticks = 0
                self.time_remaining -= 1
                if self.time_remaining <= 0:
                    self.log.add("Time's up! Auto-advancing phase...", "critical")
                    self.next_phase()

    def render(self):
        """Render game screen"""
        self.screen.fill(COLOR_BG)

        # Draw battlefield
        battlefield_rect = pygame.Rect(BATTLEFIELD_X, BATTLEFIELD_Y, BATTLEFIELD_WIDTH, BATTLEFIELD_HEIGHT)
        pygame.draw.rect(self.screen, (45, 52, 78), battlefield_rect)
        pygame.draw.rect(self.screen, COLOR_GOLD, battlefield_rect, 3)

        # Draw grid
        for x in range(BATTLEFIELD_X, BATTLEFIELD_X + BATTLEFIELD_WIDTH, 50):
            pygame.draw.line(self.screen, (60, 70, 100), (x, BATTLEFIELD_Y),
                           (x, BATTLEFIELD_Y + BATTLEFIELD_HEIGHT), 1)
        for y in range(BATTLEFIELD_Y, BATTLEFIELD_Y + BATTLEFIELD_HEIGHT, 50):
            pygame.draw.line(self.screen, (60, 70, 100), (BATTLEFIELD_X, y),
                           (BATTLEFIELD_X + BATTLEFIELD_WIDTH, y), 1)

        # Draw objectives
        for i, obj in enumerate(self.objectives):
            color = (COLOR_ORDER if obj.controlled_by == 1 else
                    COLOR_CHAOS if obj.controlled_by == 2 else
                    (100, 100, 100))
            pygame.draw.circle(self.screen, color, (obj.x, obj.y), 15)
            pygame.draw.circle(self.screen, COLOR_GOLD_LIGHT, (obj.x, obj.y), 15, 3)
            text = self.font_small.render(str(i + 1), True, COLOR_GOLD_LIGHT)
            text_rect = text.get_rect(center=(obj.x, obj.y))
            self.screen.blit(text, text_rect)

        # Draw units
        for unit in self.units:
            if not unit.is_alive():
                continue

            color = COLOR_ORDER if unit.player == 1 else COLOR_CHAOS

            # Draw combat range for selected unit
            if unit == self.selected_unit:
                pygame.draw.circle(self.screen, COLOR_GOLD_LIGHT,
                                 (unit.x, unit.y), 3 * PIXELS_PER_INCH, 2)

            # Draw unit circle
            pygame.draw.circle(self.screen, color, (unit.x, unit.y), 25)
            if unit == self.selected_unit:
                pygame.draw.circle(self.screen, COLOR_GOLD_LIGHT, (unit.x, unit.y), 25, 3)
            else:
                pygame.draw.circle(self.screen, (255, 255, 255), (unit.x, unit.y), 25, 2)

            # Draw unit initial
            text = self.font_medium.render(unit.name[0], True, COLOR_WHITE)
            text_rect = text.get_rect(center=(unit.x, unit.y))
            self.screen.blit(text, text_rect)

            # Draw health bar
            bar_width = 50
            bar_height = 5
            bar_x = unit.x - bar_width // 2
            bar_y = unit.y + 30
            pygame.draw.rect(self.screen, (50, 50, 50), (bar_x, bar_y, bar_width, bar_height))
            health_width = int(bar_width * (unit.health / unit.max_health))
            health_color = COLOR_GREEN if unit.health > unit.max_health * 0.5 else COLOR_RED
            pygame.draw.rect(self.screen, health_color, (bar_x, bar_y, health_width, bar_height))

        # Draw UI panels
        self.draw_ui_left()
        self.draw_ui_right()

        pygame.display.flip()

    def draw_ui_left(self):
        """Draw left UI panel with unit cards"""
        panel_x, panel_y = 10, 20
        panel_width = 230

        # Player 1 units
        y_offset = panel_y
        title = self.font_medium.render("Order Army (P1)", True, COLOR_ORDER)
        self.screen.blit(title, (panel_x + 10, y_offset))
        y_offset += 30

        for unit in [u for u in self.units if u.player == 1]:
            self.draw_unit_card(unit, panel_x, y_offset, panel_width)
            y_offset += 120

    def draw_ui_right(self):
        """Draw right UI panel with game info"""
        panel_x = BATTLEFIELD_X + BATTLEFIELD_WIDTH + 20
        panel_y = 20
        panel_width = SCREEN_WIDTH - panel_x - 10

        # Phase info
        phase_rect = pygame.Rect(panel_x, panel_y, panel_width, 150)
        pygame.draw.rect(self.screen, COLOR_BG_LIGHT, phase_rect)
        pygame.draw.rect(self.screen, COLOR_GOLD, phase_rect, 2)

        # Phase name
        phase_text = self.font_large.render(f"{self.phase.name} PHASE", True, COLOR_GOLD_LIGHT)
        self.screen.blit(phase_text, (panel_x + 10, panel_y + 10))

        # Turn info
        turn_text = self.font_small.render(
            f"Round {self.battle_round} - Turn {self.turn_in_round} - Player {self.current_player}",
            True, COLOR_WHITE)
        self.screen.blit(turn_text, (panel_x + 10, panel_y + 45))

        # VP and CP
        vp_text = self.font_small.render(
            f"VP: Order {self.victory_points[0]} - Chaos {self.victory_points[1]}",
            True, COLOR_WHITE)
        self.screen.blit(vp_text, (panel_x + 10, panel_y + 70))

        cp_text = self.font_small.render(
            f"CP: Order {self.command_points[0]} - Chaos {self.command_points[1]}",
            True, COLOR_WHITE)
        self.screen.blit(cp_text, (panel_x + 10, panel_y + 90))

        # Timer
        timer_color = COLOR_GREEN if self.time_remaining > 10 else COLOR_RED
        timer_text = self.font_large.render(
            f"Time: {self.time_remaining}s" if not self.auto_advancing else "AUTO",
            True, timer_color)
        self.screen.blit(timer_text, (panel_x + 10, panel_y + 115))

        # Combat log
        log_y = panel_y + 170
        log_title = self.font_medium.render("Combat Log", True, COLOR_GOLD)
        self.screen.blit(log_title, (panel_x + 10, log_y))
        log_y += 30

        log_rect = pygame.Rect(panel_x, log_y, panel_width, 400)
        pygame.draw.rect(self.screen, (20, 20, 40), log_rect)
        pygame.draw.rect(self.screen, COLOR_GOLD, log_rect, 1)

        for i, (message, log_type) in enumerate(self.log.entries[:20]):
            color = (COLOR_GREEN if log_type == "hit" else
                    COLOR_RED if log_type == "miss" else
                    COLOR_GOLD_LIGHT if log_type == "critical" else
                    COLOR_WHITE)
            text = self.font_small.render(message[:45], True, color)  # Truncate long messages
            self.screen.blit(text, (panel_x + 5, log_y + 5 + i * 20))

        # Player 2 units (bottom right)
        y_offset = SCREEN_HEIGHT - 400
        title = self.font_medium.render("Chaos Army (P2)", True, COLOR_CHAOS)
        self.screen.blit(title, (panel_x + 10, y_offset))
        y_offset += 30

        for unit in [u for u in self.units if u.player == 2]:
            self.draw_unit_card(unit, panel_x, y_offset, panel_width)
            y_offset += 120

    def draw_unit_card(self, unit: Unit, x: int, y: int, width: int):
        """Draw unit card"""
        card_height = 110
        card_rect = pygame.Rect(x, y, width, card_height)

        # Background
        bg_color = (50, 50, 80) if unit.is_alive() else (30, 30, 30)
        pygame.draw.rect(self.screen, bg_color, card_rect)

        # Border
        border_color = COLOR_GOLD_LIGHT if unit == self.selected_unit else COLOR_GOLD
        pygame.draw.rect(self.screen, border_color, card_rect, 2)

        # Name
        name_text = self.font_small.render(unit.name, True, COLOR_GOLD_LIGHT)
        self.screen.blit(name_text, (x + 5, y + 5))

        # Stats
        stats_y = y + 25
        stats = [
            f"Move: {unit.move // PIXELS_PER_INCH}\"",
            f"HP: {unit.health}/{unit.max_health}",
            f"Save: {unit.save}+",
            f"Control: {unit.control}",
        ]
        if unit.ward:
            stats.append(f"Ward: {unit.ward}+")

        for i, stat in enumerate(stats):
            text = self.font_small.render(stat, True, COLOR_WHITE)
            self.screen.blit(text, (x + 5 + (i % 2) * 110, stats_y + (i // 2) * 18))

        # Weapons
        weapons_y = stats_y + 40
        for i, weapon in enumerate(unit.weapons[:2]):  # Show max 2 weapons
            w_type = "R" if weapon.weapon_type == WeaponType.RANGED else "M"
            w_text = f"{w_type}: {weapon.attacks}A {weapon.hit}+/{weapon.wound}+ -{weapon.rend}"
            text = self.font_small.render(w_text, True, COLOR_GOLD)
            self.screen.blit(text, (x + 5, weapons_y + i * 16))

    def run(self):
        """Main game loop"""
        while self.running:
            self.handle_events()
            self.update()
            self.render()
            self.clock.tick(60)

        pygame.quit()


def main():
    """Entry point"""
    game = Game()
    game.run()


if __name__ == "__main__":
    main()
