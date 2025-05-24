# ARCHITECTURE.md

## Metadata
- **Category**: CORE
- **Type**: Architecture Documentation
- **Version**: 1.0
- **Last Updated**: 2025-01-24
- **Dependencies**: Unreal Engine 5.4

## Purpose
Defines the core architectural principles and component structure of the Cybersouls project.

## Component-Based Architecture

### Core Principles
1. **Single Responsibility**: Each component handles one specific aspect of functionality
2. **Loose Coupling**: Components communicate through interfaces and delegates
3. **Composition over Inheritance**: Favor ActorComponent composition
4. **Data-Driven Design**: Configuration through DataAssets and Blueprint

### Component Categories

#### Attribute Components
- `PlayerAttributeComponent`: Core player stats (Integrity, HackProgress)
- `PlayerCyberStateAttributeComponent`: Stamina system for CyberState
- `PhysicalEnemyAttributeComponent`: Enemy health management
- `HackingEnemyAttributeComponent`: Netrunner enemy attributes

#### Ability Components
- `SlashAbilityComponent`: Instant-kill melee attack
- `BlockAbilityComponent`: Upper body defense (3 charges)
- `DodgeAbilityComponent`: Leg attack evasion (3 charges)
- `HackAbilityComponent`: Continuous hacking damage
- `QuickHackComponent`: Cast-time abilities (1-4 keys)
- `DoubleJumpAbilityComponent`: Air mobility for CyberState
- `DashAbilityComponent`: Quick movement burst for CyberState

#### System Components
- `PlayerProgressionComponent`: XP and level tracking
- `CrosshairTargetingComponent`: Real-time target detection

### AI Architecture

#### Controllers
- `BaseEnemyAIController`: Abstract base for all enemy AI
- `PhysicalEnemyAIController`: Melee enemy behavior
- `HackingEnemyAIController`: Stationary netrunner behavior

#### Behavior Trees
- Modular task nodes for movement, combat, and communication
- Blackboard-driven state management
- Service nodes for continuous updates

### Character System

#### Playable Characters
1. **cybersoulsCharacter** (Default)
   - Combat-focused gameplay
   - QuickHack abilities
   - Integrity/HackProgress management

2. **PlayerCyberState**
   - Mobility-focused gameplay
   - Stamina-based abilities
   - Enhanced movement parameters

#### Character Switching
- Managed by `CyberSoulsPlayerController`
- Hot-swappable during gameplay (F key)
- Separate input contexts per character

## Data Flow

### Input System
1. `CyberSoulsInputConfig` (DataAsset) → Input Actions
2. Input Mapping Context → Character-specific bindings
3. PlayerController → Input distribution to components

### Combat System
1. Crosshair targeting → Target detection
2. Ability activation → Validation checks
3. Damage application → Death handling
4. XP calculation → Progression update

### AI Communication
1. Enemy spots player → Alert broadcast
2. Nearby enemies receive alert → Search behavior
3. Continuous updates while maintaining sight
4. Communication stops on sight loss

## Memory Management
- Component lifecycle tied to owning Actor
- Proper cleanup in DestroyComponent()
- Weak object pointers for cross-references
- Delegate unbinding on destruction

## Performance Considerations
- Line trace optimization for targeting
- Pooled death effect pieces
- Efficient AI perception updates
- Minimal tick usage in components