# Character System

## Overview
The character system features two distinct playable characters that can be switched during gameplay. Each character has unique abilities, resources, and movement characteristics designed for different playstyles.

## Playable Characters

### Default Character (cybersoulsCharacter)
**Class**: `AcybersoulsCharacter`
**Focus**: Combat specialist
**Resources**:
- Integrity: 100 (cosmetic health)
- HackProgress: 0-200 (death at 200)
- Block Charges: 3
- Dodge Charges: 3

**Abilities**:
- Slash (instant kill)
- Block (upper body defense)
- Dodge (leg defense)
- QuickHacks (1-4 keys)
- Camera Toggle (5 key)

**Movement**:
- Speed: 600 units/second
- Jump Height: 420 units
- Air Control: 0.2
- Gravity Scale: 1.0

### CyberState Character (PlayerCyberState)
**Class**: `APlayerCyberState`
**Focus**: Enhanced mobility
**Resource**:
- Stamina: 100 (regenerating)

**Abilities**:
- Double Jump (20 stamina)
- Dash (25 stamina)
- Camera Toggle (5 key)

**Movement**:
- Speed: 1200 units/second (2x)
- Jump Height: 600 units
- Air Control: 0.4 (2x)
- Gravity Scale: 0.8
- Double Jump: 1 air jump

## Character Switching System

### Controller Setup
**Class**: `ACyberSoulsPlayerController`
**Configuration**:
```cpp
// Blueprint settings
DefaultCharacterClass = BP_cybersoulsCharacter
CyberStateCharacterClass = BP_PlayerCyberState
InputConfig = DA_CyberSoulsInputConfig
```

### Switch Mechanics
**Input**: F key
**Process**:
1. Save current character state
2. Get current location/rotation
3. Spawn new character
4. Transfer camera settings
5. Destroy old character
6. Update HUD

**State Preservation**:
- XP values maintained
- Camera state transferred
- Position/rotation kept
- Resources reset to max

### Visual Feedback
**Character Indicator**: Top-right UI
**Switch Notification**: 
- Center screen for 2 seconds
- "Switched to [Character Name]"
- Fade in/out animation

## Input System

### Enhanced Input Setup
**Input Config**: `UCyberSoulsInputConfig`
**Data Asset**: `DA_CyberSoulsInputConfig`

### Input Mappings
**Shared Actions**:
- Move (WASD)
- Look (Mouse)
- Jump (Space)
- Switch Character (F)
- Toggle Camera (5)
- Restart (Enter)

**Default Character**:
- Slash (Left Click)
- Block (Right Click)
- Dodge (Shift)
- QuickHack1-4 (1-4 keys)

**CyberState Character**:
- Dash (Shift)
- Double Jump (Space in air)

## Character Components

### Default Character Components
```cpp
// Core Components
PlayerAttributeComponent
PlayerProgressionComponent
CrosshairTargetingComponent
SlashAbilityComponent
BlockAbilityComponent
DodgeAbilityComponent
QuickHackComponent
```

### CyberState Components
```cpp
// Core Components
PlayerCyberStateAttributeComponent
PlayerProgressionComponent
CrosshairTargetingComponent
DashAbilityComponent
DoubleJumpAbilityComponent
```

## Movement Characteristics

### Default Movement
**Ground**:
- Acceleration: 2048
- Deceleration: 2048
- Max Speed: 600

**Air**:
- Air Control: 0.2
- Falling lateral friction: 0

### CyberState Movement
**Ground**:
- Acceleration: 4096 (2x)
- Deceleration: 4096 (2x)
- Max Speed: 1200 (2x)

**Air**:
- Air Control: 0.4 (2x)
- Gravity: 0.8x
- Enhanced jump control

## Resource Management

### Default Character Resources
**Integrity**:
- Cosmetic only
- Reduced by physical attacks
- Converts to XP on quest complete

**HackProgress**:
- Lethal at 200
- Increased by enemy hacks
- Cannot be reduced

**Charges**:
- Non-regenerating
- Strategic resource
- 3 each for Block/Dodge

### CyberState Resources
**Stamina**:
- Max: 100
- Regen: 10/second
- Delay: 1 second after use
- Full regen in 10 seconds

## Character States

### Common States
- Idle
- Moving
- Jumping
- Falling
- Dead

### Default Character States
- Attacking (Slash)
- Blocking
- Dodging
- Casting (QuickHack)

### CyberState States
- Dashing
- Double Jumping
- Stamina Regenerating

## Death and Respawn

### Death Trigger
**Condition**: HackProgress >= 200
**Effects**:
- Input disabled
- Death overlay shown
- XP reset to 0
- Restart enabled

### Respawn Process
1. Press Enter on death screen
2. Level reloads completely
3. Default character spawns
4. XP reset (death) or preserved (quest complete)

## Performance Considerations

### Component Optimization
- Lazy loading of ability components
- Event-driven updates only
- Minimal tick usage

### Switch Optimization
- Seamless transition
- No loading screens
- Instant character swap
- Minimal memory overhead