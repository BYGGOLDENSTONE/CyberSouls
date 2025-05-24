# Combat System

## Overview
The combat system implements a unique "one-hit kill" mechanic where enemies die instantly to player attacks, while the player can only die from accumulated hack damage. Physical damage to the player reduces integrity but doesn't cause death.

## Core Combat Rules

### Damage Types
1. **Physical Damage**: Reduces player integrity (cosmetic)
2. **Hack Damage**: Increases hack progress (lethal at 200)
3. **Slash Damage**: Instant kill (100 damage to enemies)

### Death Conditions
- **Player**: Dies ONLY when HackProgress reaches 200
- **Enemies**: Die from any damage (100 HP baseline)

## Targeting System

### Crosshair Targeting
**Implementation**: Real-time line tracing
**Process**:
1. Cast ray from camera through screen center
2. Check for enemy actors within range
3. Detect body part hit (mesh bone analysis)
4. Update crosshair visuals

### Body Part Detection
**System**: Automatic bone name analysis
```cpp
// Upper body detection
Bones: spine, chest, shoulder, arm, head, neck

// Lower body detection  
Bones: leg, thigh, foot, pelvis, hip

// Default: Upper body if ambiguous
```

### Target Validation
```cpp
bool IsValidTarget(AActor* Target) {
    // Check if enemy
    if (!Target->IsA<ABaseEnemy>()) return false;
    
    // Check if alive
    if (Target->IsDead()) return false;
    
    // Check line of sight
    if (!HasLineOfSightTo(Target)) return false;
    
    // Check range
    if (GetDistanceTo(Target) > MaxRange) return false;
    
    return true;
}
```

## Attack Resolution

### Player Slash Attack
**Damage**: 100 (instant kill)
**Process**:
1. Get crosshair target
2. Validate target
3. Check enemy defenses:
   - BlockEnemy + Upper body hit = Blocked
   - DodgeEnemy + Leg hit = Dodged
4. Apply damage if not defended
5. Trigger death effect

### Enemy Physical Attack
**Damage**: 10 integrity damage
**Process**:
1. Check range (150 units)
2. Select random body part
3. Check player defenses:
   - Block active + Upper hit = Blocked
   - Dodge active + Leg hit = Dodged
4. Apply integrity damage
5. 2-second cooldown

### Enemy Hack Attack
**Damage**: 2-5 hack progress/second
**Types**:
- Netrunner: 5/sec continuous
- BuffNetrunner: 3/sec + ally buffs
- DebuffNetrunner: 2/sec + player debuffs
**Process**:
1. Check range (1500 units)
2. Verify line of sight
3. Apply continuous damage while in range

## Defense Mechanics

### Player Defenses
**Block (3 charges)**:
- Negates upper body attacks
- Consumes 1 charge per block
- No regeneration

**Dodge (3 charges)**:
- Negates leg attacks
- Consumes 1 charge per dodge
- No regeneration

### Enemy Defenses
**BlockEnemy**:
- Blocks player upper body slashes
- Unlimited uses
- No cooldown

**DodgeEnemy**:
- Dodges player leg slashes
- Unlimited uses
- No cooldown

## Death Effects

### Enemy Shatter Effect
**Visual**: Glass shatter with physics
**Parameters**:
```cpp
PieceCount: 50
PieceScale: 0.15-0.35 (random)
ExplosionForce: 300-400 (random)
UpwardForce: 200-300 (random)
AngularVelocity: 6 rad/s
RotationMultiplier: 200
DespawnTime: 3 seconds
```

### Player Death
**Trigger**: HackProgress >= 200
**Effects**:
- "SYSTEM COMPROMISED" overlay
- Input disabled (except restart)
- Camera lock
- XP reset on restart

## Combat Flow

### Typical Engagement
1. **Detection**: Enemy spots player
2. **Approach**: Physical enemies close distance
3. **Combat Loop**:
   - Player: Aim → Slash → Kill/Blocked
   - Physical Enemy: Range check → Attack → Cooldown
   - Hacking Enemy: Range check → Continuous hack
4. **Resolution**: Enemy death or player compromised

### Multi-Enemy Combat
- Enemies communicate player location
- Coordinated attacks from multiple angles
- Mixed physical/hacking pressure
- Resource management (blocks/dodges)

## Balance Considerations

### Player Advantages
- Instant kill capability
- Unlimited slash attacks
- Ranged targeting
- Mobility options (CyberState)

### Enemy Advantages
- Numbers
- No friendly fire
- Continuous hacking pressure
- Specialized defenses
- Communication system