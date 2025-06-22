# DETAILEDCLAUDE.md - COMPREHENSIVE CYBERSOULS PROJECT DOCUMENTATION

## PROJECT OVERVIEW

CyberSouls is a cyberpunk-themed action game with dual-character mechanics, combining soulslike combat with hacking abilities. The project uses Unreal Engine 5.4 with a component-based architecture emphasizing clean code principles and interface-driven communication.

## GAME DESIGN PHILOSOPHY

### Core Gameplay Loop
1. **Combat Phase**: Player uses Default Character to engage enemies with lethal slash attacks and QuickHacks
2. **Mobility Phase**: Switch to CyberState for safe exploration with enhanced movement abilities
3. **Progression**: Complete quests to earn XP, unlock new abilities through inventory system
4. **Strategic Death**: Player death is hack-based (not physical damage), creating unique tactical considerations

### Unique Mechanics
- **Dual Character System**: Two distinct playstyles in one seamless experience
- **Hack-Based Death**: Physical invulnerability with vulnerability to hacking
- **One-Hit Combat**: Enemies die instantly to slashes (unless blocking/dodging)
- **AI Targeting Asymmetry**: Enemies only pursue combat character, ignore mobility character

## DETAILED ARCHITECTURE DOCUMENTATION

### Component-Based Design Principles

#### Interface-Driven Architecture
The project uses five core interfaces to enable loose coupling:

1. **ITargetable**: Entities that can be targeted by abilities or AI
   - Methods: GetTargetLocation(), IsTargetable(), GetTargetPriority()
   - Implemented by: Characters, destructible objects, hack terminals

2. **IAbilitySource**: Components that provide abilities
   - Methods: GetAvailableAbilities(), IsAbilityReady(), GetAbilityCooldown()
   - Implemented by: QuickHackManagerComponent, PassiveAbilityComponent

3. **IAttributeOwner**: Entities with modifiable attributes
   - Methods: GetAttribute(), ModifyAttribute(), GetMaxAttribute()
   - Implemented by: PlayerCyberStateAttributeComponent, enemy attribute systems

4. **IDamageReceiver**: Entities that can receive damage
   - Methods: ReceiveDamage(), GetDamageMultiplier(), OnDeath()
   - Implemented by: All characters, destructible environment

5. **ICombatant**: Entities participating in combat
   - Methods: GetCombatState(), SetCombatTarget(), GetThreatLevel()
   - Implemented by: Player characters, AI enemies

#### Component Communication Rules

**Event-Driven Updates**:
- Components expose delegates for state changes
- Other components bind to these delegates in BeginPlay()
- No direct component-to-component calls except through interfaces

**Example Pattern**:
```cpp
// In HealthComponent
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float);
FOnHealthChanged OnHealthChangedDelegate;

// In HUDComponent::BeginPlay()
HealthComp->OnHealthChangedDelegate.AddUObject(this, &UHUDComponent::UpdateHealthDisplay);
```

### Ability System Architecture

#### BaseAbilityComponent Structure
All abilities inherit from this base class which provides:
- **Activation System**: ActivateAbility(), DeactivateAbility(), CanActivateAbility()
- **Cooldown Management**: Built-in timer handling with events
- **Resource Checking**: Automatic validation of required resources
- **Input Binding**: Standardized input handling through ability slots

#### Ability Categories

**Combat Abilities** (Default Character):
1. **Slash Attack**: Instant kill melee with animation-driven hitbox
2. **QuickHacks**: 8 unique hacking abilities with varied effects
   - Cascade Virus: Spreads damage through connected enemies
   - Ghost Protocol: Temporary invisibility from hacking enemies
   - Charge Drain: Disables enemy dash abilities
   - Gravity Flip: Reverses enemy gravity temporarily
   - System Shock: Stuns mechanical enemies
   - Neural Disrupt: Confuses organic enemies
   - Data Breach: Reveals enemy weaknesses
   - Memory Wipe: Resets enemy alert status

**Movement Abilities** (CyberState):
1. **Double Jump**: Consumes 20 stamina, unlimited air jumps with stamina
2. **Dash**: 3D movement ability with charge system
   - Ground dash: 2D horizontal only
   - Air dash: Full 3D directional
   - Charge regeneration pauses in air

**Passive Abilities** (Both Characters):
1. **Execution Chains**: Combo multiplier on consecutive kills
2. **System Overcharge**: Ability cooldown reduction
3. **Neural Buffer**: Hack resistance increase
4. **Cyber Resilience**: Stamina regeneration boost
5. **Data Recovery**: XP gain increase
6. **Signal Boost**: Ability range extension

### Character System Details

#### Character Pool Management
The CharacterPoolManager maintains both characters in memory:
- Characters are spawned once at game start
- Switching uses SetActorHiddenInGame() and possession changes
- State preservation includes position, rotation, velocity, and resources
- Both characters visible in World Outliner is intentional

#### Resource Systems

**Default Character Resources**:
- **Integrity**: 100 HP, represents system integrity
- **HackProgress**: 0-200, death at 200 (primary death mechanic)
- **Ability Charges**: Individual cooldowns per QuickHack

**CyberState Resources**:
- **Stamina**: 100 max, 10/sec regeneration after 1s delay
- **Dash Charges**: 1 charge, 3s ground regeneration
- **Movement Stats**: 1200 speed, 2x air control, 0.8 gravity

### AI System Architecture

#### AI Controller Types

**PhysicalEnemyAIController**:
- Movement Speed: 600 units/second
- Detection Range: 150 units
- Behavior: Active pursuit, melee combat focus
- Alert System: Notifies allies within 500 units
- Search Pattern: Investigates last known position

**HackingEnemyAIController**:
- Movement: Stationary turret behavior
- Detection Range: 1500 units
- Behavior: Long-range hacking attacks
- Special: Respects Ghost Protocol invisibility
- Attack Pattern: Increases player HackProgress

#### AI Communication System
- **UpdatePlayerTarget()**: Broadcast system for character switches
- **Alert Propagation**: Chain alerts through nearby enemies
- **Target Priority**: Always prioritizes Default Character
- **State Machine**: Idle → Alert → Combat → Search → Idle

### UI/UX Systems

#### HUD System Architecture

**Dynamic HUD Components**:
1. **Character-Specific Display**:
   - Default: Integrity, HackProgress, QuickHack slots
   - CyberState: Stamina bar, dash charges

2. **Ability Display**:
   - Real-time ability names from Blueprint data
   - Cooldown timers with visual indicators
   - Resource cost previews

#### Inventory System Implementation

**Technical Design**:
- **Canvas-Based Rendering**: Direct slate drawing for reliability
- **Click Detection**: Precise coordinate mapping system
- **Dropdown Menus**: Programmatically generated option lists

**Visual Design**:
- **Layout**: 4 QuickHack slots, 2 Passive slots
- **Color Coding**: Blue borders (QuickHacks), Magenta (Passives)
- **Selection Feedback**: Yellow highlight for equipped abilities
- **XP Display**: Top section shows Integrity and Hacking XP

**Interaction Flow**:
1. Tab opens inventory (mouse cursor enabled)
2. Click slot to open dropdown
3. Click ability to equip instantly
4. Click outside to close dropdown
5. Tab again to close inventory

### Save System Architecture

#### Progression Persistence

**Save Game Class** (UCybersoulsSaveGame):
```cpp
UPROPERTY()
int32 IntegrityXP;

UPROPERTY()
int32 HackingXP;

UPROPERTY()
TArray<FString> UnlockedQuickHacks;

UPROPERTY()
TArray<FString> UnlockedPassives;

UPROPERTY()
FString LastCheckpoint;
```

**Save Triggers**:
- Quest completion (XP preserved)
- Manual save points
- Pre-boss encounters

**Load Behavior**:
- On game start: Load last save
- On death: Reset XP, maintain unlocks
- On restart: Preserve or reset based on context

### Performance Optimization Strategies

#### Component Optimization
1. **Event-Driven Updates**: Minimize tick usage
2. **Cached References**: Store component pointers in BeginPlay()
3. **Pooled Objects**: Reuse projectiles and effects
4. **LOD Systems**: Aggressive culling for distant enemies

#### Memory Management
1. **Character Pooling**: No spawn/destroy on switch
2. **Ability Pooling**: Reusable ability effect actors
3. **UI Caching**: Pre-create UI elements

### Input System Configuration

#### Enhanced Input System
- **Input Actions**: Defined in DA_CyberSoulsInputConfig
- **Context Mapping**: Character-specific input contexts
- **Modifier Keys**: Shift/Ctrl for ability variants

#### Key Bindings
**Combat**:
- LMB: Slash attack
- 1-4: QuickHack slots
- 5: Camera toggle

**Movement**:
- WASD: Standard movement
- Space: Jump/Double Jump
- Shift: Dash

**System**:
- F: Character switch
- Tab: Inventory
- Enter: Restart/Respawn
- Esc: Pause menu

### Blueprint Integration Requirements

#### Critical Blueprint Setup

**BP_CyberSoulsPlayerController**:
```
Properties to Set:
- DefaultCharacterClass = BP_cybersoulsCharacter
- CyberStateCharacterClass = BP_PlayerCyberState  
- InputConfig = DA_CyberSoulsInputConfig
- CharacterPoolSize = 2
```

**Character Blueprint Components**:
```
BP_cybersoulsCharacter must have:
- QuickHackManagerComponent
- PassiveAbilityComponent
- TargetingComponent
- PlayerProgressionComponent
- PlayerCyberStateAttributeComponent (for resource management)

BP_PlayerCyberState must have:
- DashAbilityComponent
- DoubleJumpAbilityComponent
- TargetingComponent
- PlayerProgressionComponent
- StaminaComponent
```

**UI Blueprint Requirements**:
```
BP_CybersoulsHUD:
- InventoryWidgetClass = WBP_Inventory
- HUDWidgetClass = WBP_GameHUD

WBP_Inventory:
- Must inherit from UInventoryWidget C++ class
- No UMG setup needed (programmatic creation)
```

### Development History & Bug Fixes

#### Version 0.8.3 (5/28/2025) - Dropdown Inventory
**Major Achievement**: Complete inventory system overhaul
- Transitioned from cycling to full dropdown menus
- Fixed click detection coordinate misalignment
- Implemented visual feedback system
- Added all 14 abilities to selection system

**Technical Challenges Solved**:
1. **Coordinate Alignment**: Fixed offset between visual and click areas
2. **Dropdown Behavior**: Single dropdown at a time, click-outside-to-close
3. **Performance**: Efficient slate rendering without UMG overhead

#### Version 0.8 (5/27/2025) - System Stability
**Critical Fixes**:
1. **XP Crash Prevention**: Exception 0x80000003 eliminated
2. **Save System Reliability**: Delayed loading for consistency
3. **Widget Cleanup**: Proper cleanup before level restart
4. **Linking Errors**: Added Slate/SlateCore modules

#### Version 0.7 - AI Targeting Fix
**Major Fix**: AI now correctly ignores CyberState character
- Implemented UpdatePlayerTarget broadcast system
- Enhanced AI state machine for character switching
- Added comprehensive null checks

### Code Quality Standards

#### Naming Conventions
- **Classes**: PascalCase with U/A/F prefix (UCybersoulsCharacter)
- **Functions**: PascalCase (ActivateAbility)
- **Variables**: PascalCase for UPROPERTY, camelCase for locals
- **Constants**: SCREAMING_SNAKE_CASE
- **Interfaces**: I prefix (ITargetable)

#### Documentation Standards
- **Headers**: Every public method needs brief description
- **Complex Logic**: Inline comments for non-obvious code
- **TODOs**: Include ticket number and assignee

#### Error Handling Patterns
```cpp
// Standard validation pattern
if (!IsValid(Component))
{
    UE_LOG(LogCybersouls, Warning, TEXT("Component is null in %s"), *GetName());
    return;
}

// Safe pointer access
if (UAbilityComponent* AbilityComp = Cast<UAbilityComponent>(Component))
{
    AbilityComp->ActivateAbility();
}
```

### Testing & Debugging

#### Debug Commands
```cpp
// Console commands for testing
cybersouls.SetHackProgress 150  // Set hack progress
cybersouls.GiveXP Integrity 1000  // Award XP
cybersouls.UnlockAllAbilities  // Unlock everything
cybersouls.ToggleAIDebug  // Show AI perception
```

#### Common Issues & Solutions

**Issue**: Inventory not opening
- Check: InventoryWidgetClass set in HUD Blueprint
- Check: Input binding in project settings
- Verify: Mouse cursor settings

**Issue**: Character switch not working
- Check: Both character classes set in PlayerController
- Verify: CharacterPoolManager initialization
- Debug: Log possession changes

**Issue**: AI not targeting correctly
- Verify: TargetingComponent on both characters
- Check: AI Controller cast success
- Debug: UpdatePlayerTarget() broadcast

### Performance Profiling Results

#### Frame Time Budget (60 FPS target)
- **Rendering**: 8ms (shadows, post-processing)
- **Game Thread**: 6ms (gameplay logic)
- **Physics**: 2ms (character movement)
- **UI**: 1ms (HUD updates)

#### Optimization Opportunities
1. **AI LOD System**: Reduce AI updates for distant enemies
2. **Ability Effect Pooling**: Reuse particle systems
3. **UI Batching**: Combine draw calls for inventory

### Future Development Roadmap

#### Planned Features
1. **Skill Trees**: Branching progression paths
2. **Boss Encounters**: Multi-phase hack battles
3. **Multiplayer**: Co-op character control
4. **Level Editor**: Community content tools

#### Technical Debt
1. **Refactor**: Move UI to CommonUI framework
2. **Optimize**: Implement ability effect pooling
3. **Enhance**: Add visual scripting support
4. **Modernize**: Migrate to UE5 Niagara fully

### Build & Deployment

#### Build Configuration
```bash
# Generate project files (Windows only)
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\GenerateProjectFiles.bat" "D:\Unreal Projects\cybersouls\cybersouls.uproject" -Game

# Build configurations
Development Editor - For testing
Development - For profiling  
Shipping - For distribution
```

#### Package Settings
- **Target Platforms**: Windows 64-bit
- **Texture Compression**: DXT5
- **Audio Compression**: Ogg Vorbis
- **Pak File**: Enabled for distribution

### Conclusion

CyberSouls represents a unique blend of soulslike combat and cyberpunk mobility, built on a robust component-based architecture. The dual-character system creates emergent gameplay through AI targeting asymmetry, while the hack-based death mechanic subverts traditional combat expectations. With comprehensive systems for abilities, progression, and UI interaction, the game provides a solid foundation for future expansion.

**Current Version**: 0.8.3 - Fully playable with complete inventory system
**Development Status**: Core systems complete, ready for content expansion
**Technical Achievement**: Clean architecture with minimal technical debt