# Modular Component System

This document describes the modular component system implemented for Cybersouls, where each ability is its own component and enemies are built like "LEGO" by combining different components.

## Design Philosophy

- Each ability is a separate component
- Enemies are assembled by adding specific components
- Player has separate attribute system from enemies
- All enemies die in one hit unless they use defensive abilities

## Component Types

### 1. Attribute Components

#### PlayerAttributeComponent
- **Integrity**: Player health (doesn't cause death at 0)
- **HackProgress**: Death when reaches 100
- **Status Flags**: bCanUseAbilities, bIsImmobilized, bHasFirewall

#### EnemyAttributeComponent
- **Integrity**: 1.0 (dies in one hit)
- **Death Event**: Triggers when integrity reaches 0

### 2. Ability Components

#### Base Components
- **BaseAbilityComponent**: Abstract base for all abilities
- **AttackAbilityComponent**: Physical melee attacks
- **HackAbilityComponent**: Continuous hacking ability
- **SlashAbilityComponent**: Player's main attack (10 damage)

#### Defensive Components
- **BlockAbilityComponent**: Blocks upper body attacks (charge-based)
- **DodgeAbilityComponent**: Dodges leg attacks (charge-based)

#### QuickHack Components
- **QuickHackComponent**: Cast-time based abilities
- **InterruptProtocol**: Cancels enemy QuickHacks
- **SystemFreeze**: Immobilizes target
- **Firewall**: Protects against hacking

## Enemy Composition

### Physical Enemies
1. **BasicEnemy**
   - EnemyAttributeComponent
   - AttackAbilityComponent

2. **BlockEnemy**
   - EnemyAttributeComponent
   - AttackAbilityComponent
   - BlockAbilityComponent (3 charges, upper body only)

3. **DodgeEnemy**
   - EnemyAttributeComponent
   - AttackAbilityComponent
   - DodgeAbilityComponent (3 charges, legs only)

### Hacking Enemies
4. **Netrunner**
   - EnemyAttributeComponent
   - HackAbilityComponent (2.0/sec)

5. **BuffNetrunner**
   - EnemyAttributeComponent
   - HackAbilityComponent (1.5/sec)
   - QuickHackComponent (Firewall)

6. **DebuffNetrunner**
   - EnemyAttributeComponent
   - HackAbilityComponent (1.5/sec)
   - QuickHackComponent (SystemFreeze)
   - QuickHackComponent (InterruptProtocol)

## Player Composition

- **PlayerAttributeComponent**: Special player attributes
- **SlashAbilityComponent**: Main attack ability
- **QuickHackComponent** (InterruptProtocol)
- **QuickHackComponent** (SystemFreeze)
- **QuickHackComponent** (Firewall)
- **TargetLockComponent**: Camera and targeting

## Key Mechanics

### One-Hit Kill System
- All enemies have 1 integrity
- Slash deals 10 damage (instant kill)
- Enemies survive only if they block/dodge

### Defensive Abilities
- **Block**: Only works on upper body attacks
- **Dodge**: Only works on leg attacks
- Both use charge system (limited uses)

### Player Death
- Only from HackProgress reaching 100
- Integrity damage doesn't kill player
- QuickHacks can disable abilities or immobilize

## Usage Example

To create a new enemy type that can both block and dodge:

```cpp
// In the new enemy class constructor
EnemyAttributes = CreateDefaultSubobject<UEnemyAttributeComponent>(TEXT("EnemyAttributes"));
AttackAbility = CreateDefaultSubobject<UAttackAbilityComponent>(TEXT("AttackAbility"));
BlockAbility = CreateDefaultSubobject<UBlockAbilityComponent>(TEXT("BlockAbility"));
DodgeAbility = CreateDefaultSubobject<UDodgeAbilityComponent>(TEXT("DodgeAbility"));

// Configure in InitializeEnemy()
BlockAbility->MaxBlockCharges = 2;
DodgeAbility->MaxDodgeCharges = 2;
```

## Benefits

1. **Modularity**: Easy to mix and match abilities
2. **Extensibility**: Add new abilities without modifying existing code
3. **Clarity**: Each component has a single responsibility
4. **Reusability**: Components can be shared across different enemy types
5. **Testing**: Individual components can be tested in isolation

## Implementation Notes

- Components automatically handle their own timers and cooldowns
- Death events are propagated through the attribute component
- QuickHacks use cast time system with interruption mechanics
- All abilities respect player status flags (immobilized, disabled)