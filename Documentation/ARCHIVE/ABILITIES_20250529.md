# Ability Components

## Overview
Ability components implement individual player and enemy abilities using Unreal Engine's ActorComponent system. Each ability is self-contained with its own cooldowns, resource costs, and execution logic.

## Player Abilities

### SlashAbilityComponent
**Location**: `Source/cybersouls/Public/Player/SlashAbilityComponent.h`
**Purpose**: Player's primary attack - instant kill on enemies
**Dependencies**: 
- CrosshairTargetingComponent (for target detection)
- UInputAction (for input binding)
**Key Features**:
- Instant kill mechanic (100 damage)
- Uses crosshair targeting system
- No resource cost or cooldown
- Respects enemy Block/Dodge states

### DashAbilityComponent
**Location**: `Source/cybersouls/Public/Player/DashAbilityComponent.h`
**Purpose**: Quick dash movement for CyberState character
**Dependencies**:
- PlayerCyberStateAttributeComponent (stamina management)
- CharacterMovementComponent
**Key Features**:
- Cost: 25 stamina
- Distance: 1000 units over 0.2 seconds
- Cooldown: 0.5 seconds
- Direction: Forward based on character rotation

### DoubleJumpAbilityComponent
**Location**: `Source/cybersouls/Public/Player/DoubleJumpAbilityComponent.h`
**Purpose**: Air jump capability for CyberState character
**Dependencies**:
- PlayerCyberStateAttributeComponent (stamina management)
- CharacterMovementComponent
**Key Features**:
- Cost: 20 stamina per jump
- Allows 1 additional jump in air
- Resets on landing
- Jump velocity: 600 units

### BlockAbilityComponent
**Location**: `Source/cybersouls/Public/Player/BlockAbilityComponent.h`
**Purpose**: Blocks upper body attacks
**Dependencies**:
- CrosshairTargetingComponent
- PlayerAttributeComponent
**Key Features**:
- 3 block charges
- Blocks attacks targeting upper body
- Visual feedback on successful block
- Charges don't regenerate

### DodgeAbilityComponent
**Location**: `Source/cybersouls/Public/Player/DodgeAbilityComponent.h`
**Purpose**: Dodges leg attacks
**Dependencies**:
- CrosshairTargetingComponent
- PlayerAttributeComponent
**Key Features**:
- 3 dodge charges
- Dodges attacks targeting legs
- Visual feedback on successful dodge
- Charges don't regenerate

### QuickHackComponent
**Location**: `Source/cybersouls/Public/Player/QuickHackComponent.h`
**Purpose**: Cast-time hacking abilities
**Dependencies**:
- CrosshairTargetingComponent
- PlayerProgressionComponent
**Key Features**:
- 4 different hacks (keys 1-4)
- Variable cast times (1-3 seconds)
- Different effects per hack type
- Uses crosshair for targeting

## Enemy Abilities

### AttackAbilityComponent
**Location**: `Source/cybersouls/Public/AI/AttackAbilityComponent.h`
**Purpose**: Physical enemy melee attacks
**Dependencies**:
- BaseEnemyAIController
- PhysicalEnemyAttributeComponent
**Key Features**:
- Damage: 10 integrity damage
- Cooldown: 2 seconds
- Range: 150 units
- Targets random body part (upper/legs)

### HackAbilityComponent
**Location**: `Source/cybersouls/Public/AI/HackAbilityComponent.h`
**Purpose**: Continuous hacking by netrunner enemies
**Dependencies**:
- HackingEnemyAIController
- HackingEnemyAttributeComponent
**Key Features**:
- Continuous hack when in range (1500 units)
- Hack rates: 2-5 per second (enemy type dependent)
- Requires line of sight
- No cooldown (continuous effect)

## Common Patterns

### Ability Execution Flow
1. Check prerequisites (cooldown, resources, valid target)
2. Consume resources (stamina, charges)
3. Execute ability logic
4. Apply effects (damage, movement, etc.)
5. Start cooldown timer
6. Trigger visual/audio feedback

### Input Binding
- Player abilities use Enhanced Input System
- Bound through character's SetupPlayerInputComponent
- Input actions defined in CyberSoulsInputConfig

### Target Validation
- Most abilities use CrosshairTargetingComponent
- Line trace from camera through crosshair
- Automatic body part detection
- Range and line-of-sight checks