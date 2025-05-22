# Enemy Class Implementation Summary

This document describes the 6 separate enemy classes created for the Cybersouls project, each inheriting from the base `CybersoulsEnemyBase` class.

## Physical Enemies

### 1. ACybersoulsBasicEnemy
- **Health**: 100 HP
- **Attack Speed**: 2.0 attacks/second
- **Abilities**: Attack only
- **Special**: No special abilities, standard melee combat

### 2. ACybersoulsBlockEnemy
- **Health**: 150 HP (more durable)
- **Attack Speed**: 2.5 attacks/second
- **Abilities**: Attack + Block
- **Special**: Can block upper body attacks (3 charges)
- **Block Mechanic**: Only blocks attacks targeting the upper body

### 3. ACybersoulsDodgeEnemy
- **Health**: 80 HP (more fragile)
- **Attack Speed**: 1.5 attacks/second
- **Abilities**: Attack + Dodge
- **Special**: Can dodge leg attacks (3 charges)
- **Dodge Mechanic**: Performs a movement dodge when avoiding leg attacks
- **Dodge Parameters**: 300 unit distance, 800 speed

## Hacking Enemies

### 4. ACybersoulsNetrunner
- **Health**: 50 HP
- **Hack Rate**: 2.0 progress/second
- **Abilities**: Hack only
- **Special**: Basic continuous hacking, no QuickHacks

### 5. ACybersoulsBuffNetrunner
- **Health**: 60 HP
- **Hack Rate**: 1.5 progress/second
- **Abilities**: Hack + Firewall QuickHack
- **Special**: Can cast Firewall on self (30% chance per cycle)
- **Firewall**: 6s cast time, 12s cooldown, self-targeted

### 6. ACybersoulsDebuffNetrunner
- **Health**: 60 HP
- **Hack Rate**: 1.5 progress/second
- **Abilities**: Hack + SystemFreeze/InterruptProtocol QuickHacks
- **Special**: Can cast debuff QuickHacks on player (40% chance per cycle)
- **SystemFreeze**: 7s cast time, 14s cooldown, 60% preference
- **InterruptProtocol**: 5s cast time, 8s cooldown, 40% preference

## Implementation Details

Each enemy class:
1. Sets its own `EnemyType` in the constructor
2. Overrides `InitializeEnemy()` for specific stats/abilities
3. Physical enemies override combat methods (`TryBlock`, `TryDodge`)
4. Hacking enemies override `PerformHack` for unique behaviors
5. Inherits all base functionality (timers, AI, body parts)

## Usage in Blueprints

To use these enemies in Unreal:
1. Create Blueprint classes inheriting from each C++ class
2. Set up visual components (mesh, animations)
3. Configure AI controller settings
4. Place in levels or spawn dynamically

## Key Design Patterns

- **Inheritance**: All enemies inherit from `CybersoulsEnemyBase`
- **Polymorphism**: Virtual methods allow specialized behaviors
- **Data-Driven**: Stats and abilities configured per enemy type
- **Modular**: Each enemy class is self-contained and focused