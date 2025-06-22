# Enemy System

## Overview
The enemy system features two distinct categories: physical melee enemies and hacking netrunners. All enemies share core behaviors like perception and communication but differ in combat approach and movement patterns.

## Enemy Types

### Physical Enemies

#### BasicEnemy
**Class**: `ABasicEnemy`
**Attributes**:
- Health: 100 HP
- Speed: 600 units/second
- Attack Damage: 10 integrity
- Attack Range: 150 units
- Attack Cooldown: 2 seconds
**Behavior**:
- Standard melee attacker
- Searches last known location
- No special defenses

#### BlockEnemy
**Class**: `ABlockEnemy`
**Inherits**: BasicEnemy stats
**Special Feature**: Blocks upper body slashes
**Behavior**:
- Immune to upper body targeted attacks
- Vulnerable to leg attacks
- Same movement/search patterns

#### DodgeEnemy
**Class**: `ADodgeEnemy`
**Inherits**: BasicEnemy stats
**Special Feature**: Dodges leg slashes
**Behavior**:
- Immune to leg targeted attacks
- Vulnerable to upper body attacks
- Same movement/search patterns

### Hacking Enemies

#### Netrunner
**Class**: `ANetrunner`
**Attributes**:
- Health: 100 HP
- Speed: 0 (STATIONARY)
- Hack Rate: 5/second
- Hack Range: 1500 units
**Behavior**:
- No movement capability
- Continuous hacking when in range
- Requires line of sight

#### BuffNetrunner
**Class**: `ABuffNetrunner`
**Inherits**: Netrunner base
**Special Features**:
- Hack Rate: 3/second
- Buffs nearby allies
- Buff Range: 1000 units
**Buffs Applied**:
- Increased movement speed
- Reduced ability cooldowns

#### DebuffNetrunner
**Class**: `ADebuffNetrunner`
**Inherits**: Netrunner base
**Special Features**:
- Hack Rate: 2/second
- Debuffs player
- Uses QuickHacks
**Debuffs Applied**:
- Slowed movement
- Reduced defense charges
- Ability disruption

## Perception System

### Configuration
```cpp
// Sight Configuration
SightRadius: 2000 units
LoseSightRadius: 2500 units
PeripheralVisionAngle: 90 degrees
AutoSuccessRange: 500 units

// Detection Requirements
- Direct line of sight required
- No detection through walls/obstacles
- Height difference considered
```

### Detection States
1. **Unaware**: No target, patrol/idle
2. **Investigating**: Lost sight, searching
3. **Combat**: Active target engagement
4. **Alert**: Received communication about target

## Communication System

### Alert Mechanism
**Trigger**: Enemy spots player
**Broadcast**:
- Radius: 2000 units (physical), 2500 units (hacking)
- Frequency: Every 1 second while target visible
- Information: Player location, timestamp

### Response Behavior
**Physical Enemies**:
- Move to reported location
- Begin 5-second search
- Can be interrupted by direct sighting

**Hacking Enemies**:
- Acknowledge alert
- Remain in position
- Prepare hacking abilities

## Movement Patterns

### Physical Enemy Movement
**Patrol State**:
- Waypoint-based or stationary
- Slow walk speed (300 units/sec)
- Random wait times at waypoints

**Combat Movement**:
- Direct path to target
- Full speed (600 units/sec)
- Stops at attack range (150 units)

**Search Pattern**:
- Move to last known location
- 360-degree rotation scan
- 5-second duration
- Random nearby point checks

### Hacking Enemy Movement
**All States**: No movement (speed = 0)
**Rotation**: Track player when visible

## AI Behavior Trees

### Physical Enemy BT
```
Root Selector
├── Combat Sequence
│   ├── Has Valid Target?
│   ├── In Attack Range?
│   │   └── Attack
│   └── Move To Attack Range
├── Search Sequence  
│   ├── Has Search Location?
│   ├── Move To Location
│   └── Look Around (5 sec)
└── Patrol/Idle
```

### Hacking Enemy BT
```
Root Selector
├── Hack Sequence
│   ├── Has Valid Target?
│   ├── In Hack Range?
│   ├── Has Line of Sight?
│   └── Continuous Hack
└── Idle (No Movement)
```

## Spawn System

### Level Placement
- Manually placed in level
- Grouped by encounter design
- Mixed types for variety

### Spawn Parameters
```cpp
// Physical Enemy Spawn
Location: Designer-specified
Rotation: Face patrol direction
AI Controller: PhysicalEnemyAIController

// Hacking Enemy Spawn  
Location: Strategic positions (elevated, corners)
Rotation: Face likely approach
AI Controller: HackingEnemyAIController
```

## Death Handling

### Death Sequence
1. Stop all AI behavior
2. Clear perception
3. Broadcast death event
4. Play shatter effect
5. Destroy after 3 seconds

### Death Effect Parameters
```cpp
ShatterPieces: 50
PieceLifetime: 3 seconds
PhysicsEnabled: true
CollisionDisabled: After spawn
```

## Performance Optimization

### LOD System
- Perception updates reduced at distance
- Animation quality scales
- Behavior tree complexity reduces

### Pooling Considerations
- Reuse AI controllers
- Recycle perception components
- Cache pathfinding results

## Enemy Encounters

### Design Principles
1. **Mixed Composition**: Combine physical and hacking
2. **Sight Lines**: Place hackers with good coverage
3. **Flanking Routes**: Multiple approach paths
4. **Defense Variety**: Mix Block and Dodge enemies

### Example Encounters
**Corridor Defense**:
- 2 BasicEnemy (front)
- 1 BlockEnemy (middle)
- 1 Netrunner (back, elevated)

**Open Area**:
- 3 DodgeEnemy (patrol)
- 2 DebuffNetrunner (corners)
- Communication enables coordination