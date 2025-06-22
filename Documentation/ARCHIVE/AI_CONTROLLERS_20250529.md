# AI Controllers

## Overview
AI Controllers manage enemy behavior, perception, and decision-making. The system uses a base controller with specialized implementations for physical and hacking enemy types.

## Controller Hierarchy

### BaseEnemyAIController
**Location**: `Source/cybersouls/Public/AI/BaseEnemyAIController.h`
**Purpose**: Shared AI functionality for all enemies
**Dependencies**:
- AIPerceptionComponent
- AISightConfig
**Key Features**:
- Perception system setup
- Target management
- Death state handling
- Communication system
- Search behavior framework

### PhysicalEnemyAIController
**Location**: `Source/cybersouls/Public/AI/PhysicalEnemyAIController.h`
**Purpose**: Controls melee enemy behavior
**Inherits**: BaseEnemyAIController
**Dependencies**:
- AttackAbilityComponent
- PhysicalEnemyAttributeComponent
**Configuration**:
```cpp
// Movement
WalkSpeed: 600
AttackRange: 150
AcceptanceRadius: 50

// Perception
SightRange: 2000
LoseSightRadius: 2500
PeripheralVisionAngle: 90°
EyeHeight: 80 units

// Search Behavior
SearchDuration: 5 seconds
SearchRadius: 500 units
```

### HackingEnemyAIController
**Location**: `Source/cybersouls/Public/AI/HackingEnemyAIController.h`
**Purpose**: Controls netrunner enemy behavior
**Inherits**: BaseEnemyAIController
**Dependencies**:
- HackAbilityComponent
- HackingEnemyAttributeComponent
- QuickHackComponent (for special abilities)
**Configuration**:
```cpp
// Movement
WalkSpeed: 0 (STATIONARY)
HackRange: 1500

// Perception
SightRange: 2000
LoseSightRadius: 2500
PeripheralVisionAngle: 90°

// No search behavior (stationary)
```

## Core Systems

### Perception System
**Implementation**: Unreal's AI Perception Component
**Features**:
- Sight-based detection only
- Requires direct line of sight
- No detection through obstacles
- Automatic target loss on sight break
- Debug traces disabled in viewport

### Communication System
**Purpose**: Enemies alert nearby allies when spotting player
**Implementation**:
```cpp
// Alert broadcast
AlertRadius: Physical=2000, Hacking=2500
UpdateFrequency: 1 second
Information: Player location, timestamp

// Alert response
Physical enemies: Move to search location
Hacking enemies: Stay in position, prepare hacks
```

### Search Behavior (Physical Only)
**Triggers**: Lost sight of player or received alert
**Behavior**:
1. Move to last known location
2. Look around for 5 seconds
3. Return to patrol if no target found
4. Can be interrupted by new sightings

### Combat Decision Making

#### Physical Enemy Logic
```
IF (HasTarget AND InAttackRange)
    → Attack with 2s cooldown
ELIF (HasTarget AND NOT InRange)
    → Move to attack range
ELIF (SearchActive)
    → Continue search pattern
ELSE
    → Patrol or idle
```

#### Hacking Enemy Logic
```
IF (HasTarget AND InHackRange AND HasLineOfSight)
    → Continuous hacking
    → Use QuickHacks strategically
ELIF (HasTarget AND NOT InRange)
    → Wait (no movement)
ELSE
    → Idle in position
```

## Behavior Tree Integration

### Physical Enemy BT Structure
- **Root**: Selector
  - **Combat Sequence**: Check target → Move to range → Attack
  - **Search Sequence**: Has search location → Move → Look around
  - **Patrol Sequence**: Default behavior

### Hacking Enemy BT Structure
- **Root**: Selector
  - **Hack Sequence**: Check target → Verify range → Hack
  - **Idle**: Default state (no movement nodes)

## Performance Optimizations

### Tick Management
- Perception updates: Every 0.1 seconds
- Communication updates: Every 1.0 second
- Movement updates: Every frame (physical only)
- Search timer: Checked every 0.5 seconds

### Line of Sight Checks
- Cached for 0.1 seconds
- Single trace per check
- Early exit on obstacle hit
- No debug visualization in game

## Common Issues and Solutions

### Issue: Enemies detecting through walls
**Solution**: Ensure LineOfSightTo checks are enabled and collision channels are properly set

### Issue: Enemies not communicating
**Solution**: Verify communication radius and that receiving enemies are checking for alerts

### Issue: Hacking enemies moving
**Solution**: Ensure movement speed is 0 and no movement tasks in behavior tree