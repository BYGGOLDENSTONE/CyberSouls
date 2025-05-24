# Attribute Components

## Overview
Attribute components manage character stats, resources, and progression. They handle health, stamina, hack progress, and experience tracking for both players and enemies.

## Player Attribute Components

### PlayerAttributeComponent
**Location**: `Source/cybersouls/Public/Player/PlayerAttributeComponent.h`
**Purpose**: Core player stats for default character
**Dependencies**: None (base component)
**Key Stats**:
- Integrity: 100 (visual health, doesn't cause death)
- HackProgress: 0-200 (death at 200)
- Block Charges: 3
- Dodge Charges: 3
**Key Features**:
- Death only from HackProgress reaching 200
- Integrity damage is cosmetic
- Charges don't regenerate
- Broadcasts attribute change events

### PlayerCyberStateAttributeComponent
**Location**: `Source/cybersouls/Public/Player/PlayerCyberStateAttributeComponent.h`
**Purpose**: Stamina system for CyberState character
**Dependencies**: None (base component)
**Key Stats**:
- Max Stamina: 100
- Regen Rate: 10/second
- Regen Delay: 1 second after use
**Key Features**:
- Automatic regeneration system
- Delay timer after stamina use
- Used by Dash and DoubleJump abilities
- Full regen from 0 to 100 in 10 seconds

### PlayerProgressionComponent
**Location**: `Source/cybersouls/Public/Player/PlayerProgressionComponent.h`
**Purpose**: XP and progression tracking
**Dependencies**: 
- GameInstance (for persistence)
- PlayerAttributeComponent (for quest completion)
**Key Features**:
- IntegrityXP: 1:1 from remaining integrity
- HackingXP: 0.5x from hack resistance
- XP persists across level restarts
- Reset on player death
- Tracks total XP earned

## Enemy Attribute Components

### PhysicalEnemyAttributeComponent
**Location**: `Source/cybersouls/Public/AI/PhysicalEnemyAttributeComponent.h`
**Purpose**: Stats for melee enemies
**Dependencies**: None (base component)
**Key Stats**:
- Health: 100 HP
- Attack Damage: 10
- Attack Cooldown: 2 seconds
**Enemy Types**:
- BasicEnemy: Standard melee
- BlockEnemy: Can block upper attacks
- DodgeEnemy: Can dodge leg attacks

### HackingEnemyAttributeComponent
**Location**: `Source/cybersouls/Public/AI/HackingEnemyAttributeComponent.h`
**Purpose**: Stats for netrunner enemies
**Dependencies**: None (base component)
**Key Stats**:
- Health: 100 HP
- Hack Range: 1500 units
- Hack Rate: Varies by type
**Enemy Types**:
- Netrunner: 5 hack/second
- BuffNetrunner: 3 hack/second (buffs allies)
- DebuffNetrunner: 2 hack/second (debuffs player)

## Attribute System Architecture

### Damage Processing
```cpp
// Player takes integrity damage (cosmetic)
PlayerAttributes->TakeIntegrityDamage(10);

// Player takes hack damage (lethal at 200)
PlayerAttributes->AddHackProgress(5);
if (PlayerAttributes->GetHackProgress() >= 200) {
    // Trigger death
}

// Enemy takes damage
EnemyAttributes->TakeDamage(100); // Instant kill
```

### Resource Management
```cpp
// Stamina consumption
if (StaminaComponent->CanUseStamina(25)) {
    StaminaComponent->UseStamina(25);
    // Execute ability
}

// Charge consumption
if (PlayerAttributes->GetBlockCharges() > 0) {
    PlayerAttributes->UseBlockCharge();
    // Block attack
}
```

### XP Calculation
```cpp
// On quest completion
int32 IntegrityXP = PlayerAttributes->GetIntegrity();
int32 HackingXP = (200 - PlayerAttributes->GetHackProgress()) * 0.5f;
ProgressionComponent->AddXP(IntegrityXP, HackingXP);
```

## Events and Delegates

### Common Delegates
- `OnIntegrityChanged`: Broadcast on integrity damage
- `OnHackProgressChanged`: Broadcast on hack progress
- `OnStaminaChanged`: Broadcast on stamina use/regen
- `OnChargesChanged`: Broadcast on charge consumption
- `OnDeath`: Broadcast on component owner death

### HUD Integration
All attribute components broadcast changes that the HUD listens to for real-time updates. The HUD subscribes to these events on BeginPlay and updates the appropriate UI elements.