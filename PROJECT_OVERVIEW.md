# CYBERSOULS PROJECT OVERVIEW

**Generated**: May 29, 2025  
**Project Version**: 0.8.3  
**Unreal Engine**: 5.4  
**Architecture**: Component-Based with Interface-Driven Communication

## RELATED DOCUMENTATION
- [Implementation Roadmap](IMPLEMENTATION_ROADMAP.md) - Detailed development phases
- [Changelog](Documentation/LOGS/CHANGELOG.md) - Version history
- [Known Issues](Documentation/LOGS/KNOWN_ISSUES.md) - Active bug tracking
- [Error Log](Documentation/LOGS/ERROR_LOG.md) - Common errors and solutions
- [Coding Standards](Documentation/CORE/CODING_STANDARDS.md) - Team guidelines
- [Quick Reference](CLAUDE.md) - Daily development cheat sheet

## TABLE OF CONTENTS
1. [Executive Summary](#executive-summary)
2. [Project Architecture](#project-architecture)
3. [Component System](#component-system)
4. [Feature Implementation Status](#feature-implementation-status)
5. [Code Quality Assessment](#code-quality-assessment)
6. [Modularity Analysis](#modularity-analysis)
7. [Refactoring Recommendations](#refactoring-recommendations)
8. [Documentation Consolidation](#documentation-consolidation)
9. [Build & Setup Instructions](#build--setup-instructions)
10. [Known Issues & Solutions](#known-issues--solutions)

---

## EXECUTIVE SUMMARY

CyberSouls is a cyberpunk-themed action game implementing dual-character mechanics with soulslike combat and hacking abilities. The project demonstrates strong component-based architecture with excellent separation of concerns. The codebase follows UE5 best practices and is well-positioned for feature expansion.

### Key Strengths
- **Clean Architecture**: Proper use of interfaces (ITargetable, IAbilitySource, IAttributeOwner, IDamageReceiver, ICombatant)
- **Modular Design**: Each ability is a self-contained component
- **Good Documentation**: Comprehensive markdown files throughout
- **Performance Conscious**: Event-driven updates, minimal tick usage

### Areas for Improvement
- Some monolithic classes could benefit from further decomposition
- Documentation is scattered across 22 .md files
- Character pooling system could be more robust
- Some coupling between UI and game logic

---

## PROJECT ARCHITECTURE

### Core Design Principles
1. **Component-Based Architecture**: Functionality split into reusable ActorComponents
2. **Interface-Driven Communication**: Components communicate via 5 core interfaces
3. **Event-Driven Updates**: Delegates and events minimize tick usage
4. **Data-Driven Configuration**: Blueprint-configurable properties via UPROPERTY

### Directory Structure
```
Source/cybersouls/
├── Private/
│   ├── Abilities/        # Ability implementations
│   ├── AI/              # AI controllers
│   ├── Attributes/      # Stat management
│   ├── Character/       # Player characters
│   ├── Combat/          # Combat mechanics
│   ├── Components/      # Utility components
│   ├── Enemy/           # Enemy classes
│   ├── Game/            # Game mode/state
│   ├── Player/          # Player-specific
│   ├── SaveGame/        # Persistence
│   └── UI/              # HUD and widgets
└── Public/
    └── [Mirrors Private structure]
```

### Interface Architecture
```cpp
// Core Interfaces
ITargetable       → Entities that can be targeted
IAbilitySource    → Components providing abilities
IAttributeOwner   → Entities with modifiable stats
IDamageReceiver   → Entities that receive damage
ICombatant        → Combat participants
```

---

## COMPONENT SYSTEM

### Player Components

#### Combat Components
- **SlashAbilityComponent**: Instant-kill melee attack
- **QuickHackManagerComponent**: Manages 8 QuickHack abilities
- **PassiveAbilityComponent**: 6 passive abilities

#### Movement Components
- **DashAbilityComponent**: 3D dash ability (ground/air variants)
- **DoubleJumpAbilityComponent**: Stamina-based air jumps

#### System Components
- **PlayerAttributeComponent**: Integrity/HackProgress management
- **PlayerCyberStateAttributeComponent**: Stamina system
- **PlayerProgressionComponent**: XP and persistence
- **TargetingComponent**: Real-time crosshair targeting

### Enemy Components

#### Base Components
- **EnemyAttributeComponent**: Basic enemy stats (100 HP)
- **AttackAbilityComponent**: Physical melee attacks
- **HackAbilityComponent**: Continuous hacking

#### Specialized Components
- **BlockAbilityComponent**: Upper body defense (3 charges)
- **DodgeAbilityComponent**: Leg attack evasion (3 charges)
- **BuffComponent**: (Planned) Temporary stat modifications

### AI Controllers
- **BaseEnemyAIController**: Shared AI functionality
- **PhysicalEnemyAIController**: Melee enemy behavior
- **HackingEnemyAIController**: Stationary netrunner behavior

---

## FEATURE IMPLEMENTATION STATUS

### ✅ COMPLETED (100%)
- [x] Dual character system with hot-swapping
- [x] Component-based ability system
- [x] Interface-driven architecture
- [x] Enemy AI with perception and communication
- [x] Crosshair targeting with body part detection
- [x] Death effects and shatter physics
- [x] XP persistence and progression
- [x] Save game system
- [x] Inventory with dropdown menus
- [x] HUD adaptation for character types
- [x] Input system with Enhanced Input
- [x] 6 enemy types (3 physical, 3 hacking)

### 🔄 IN PROGRESS (70%)
- [ ] Buff system framework (prototyped)
- [ ] Environmental impact mechanics (designed)
- [ ] Advanced enemy behaviors (partial)

### 📋 PLANNED (0%)
- [ ] Skill trees and upgrade system
- [ ] Boss encounters
- [ ] Multiplayer support
- [ ] Level editor tools
- [ ] Audio implementation
- [ ] VFX polish pass

---

## CODE QUALITY ASSESSMENT

### Strengths
1. **Consistent Naming**: Follows UE conventions (U/A/F prefixes)
2. **Proper Includes**: Uses full paths from Source folder
3. **UPROPERTY Usage**: Good exposure for Blueprint configuration
4. **Error Handling**: Consistent IsValid() checks
5. **Memory Management**: Proper component lifecycle

### Code Patterns
```cpp
// Good: Interface usage
if (ITargetable* Target = Cast<ITargetable>(Actor))
{
    if (Target->CanBeTargeted())
    {
        // Process target
    }
}

// Good: Event-driven updates
OnHealthChangedDelegate.AddUObject(this, &UHUDComponent::UpdateHealthDisplay);

// Good: Component validation
if (UAbilityComponent* AbilityComp = GetOwner()->FindComponentByClass<UAbilityComponent>())
{
    AbilityComp->ActivateAbility();
}
```

### Areas Needing Attention
1. **FindComponentByClass Usage**: Should migrate to interface-based lookups
2. **Magic Numbers**: Some hardcoded values need UPROPERTY exposure
3. **Tick Functions**: A few components still use tick unnecessarily

---

## MODULARITY ANALYSIS

### Well-Modularized Systems
1. **Ability System**: Each ability is completely self-contained
2. **AI Controllers**: Clean separation between enemy types
3. **Attribute Components**: Distinct player vs enemy attributes
4. **Input System**: Character-specific input contexts

### Systems Needing Decomposition

#### 1. cybersoulsCharacter Class
**Current Issues**:
- Handles too many responsibilities
- Direct component references
- Mixed input handling

**Recommended Refactor**:
```cpp
// Split into:
CharacterBase → Core character functionality
CharacterCombat → Combat-specific logic
CharacterInput → Input handling
CharacterState → State management
```

#### 2. HUD System
**Current Issues**:
- Monolithic CybersoulsHUD class
- Direct game logic coupling

**Recommended Refactor**:
- Separate HUD widgets per system
- Use ViewModel pattern
- Event-based updates only

#### 3. Enemy Base Class
**Current Issues**:
- Some shared logic could be components
- Death effect handling mixed with core logic

**Recommended Refactor**:
- Extract death effects to component
- Move shared behaviors to components
- Use composition over inheritance

---

## REFACTORING RECOMMENDATIONS

### Priority 1: Critical Architecture Improvements

#### 1.1 Implement Proper Component Discovery
Replace all FindComponentByClass with interface-based discovery:
```cpp
// Instead of:
UHealthComponent* Health = FindComponentByClass<UHealthComponent>();

// Use:
if (IAttributeOwner* AttrOwner = Cast<IAttributeOwner>(Actor))
{
    UAttributeComponent* AttrComp = AttrOwner->GetAttributeComponent();
}
```

#### 1.2 Create Component Manager
Implement a central component registry:
```cpp
class UComponentManager : public UGameInstanceSubsystem
{
    TMap<FName, UActorComponent*> ComponentRegistry;
    
    void RegisterComponent(FName Key, UActorComponent* Component);
    UActorComponent* GetComponent(FName Key);
};
```

#### 1.3 Decouple Character Classes
Break down monolithic character classes:
- Extract state management to FSM component
- Move ability management to dedicated component
- Separate input handling from character logic

### Priority 2: System Enhancements

#### 2.1 Implement Buff System
Complete the planned buff framework:
```cpp
class UBuffComponent : public UActorComponent
{
    TMap<FName, FBuffData> ActiveBuffs;
    
    void ApplyBuff(const FBuffData& Buff);
    void RemoveBuff(FName BuffID);
    void TickBuffs(float DeltaTime);
};
```

#### 2.2 Add Environmental Impact
Implement physics-based combat reactions:
```cpp
class UEnvironmentalImpactComponent : public UActorComponent
{
    void ApplyKnockback(FVector Direction, float Force);
    void CreateGroundShockwave(FVector Origin, float Radius);
    void HandleWallCollision(FHitResult Hit);
};
```

#### 2.3 Enhance AI Communication
Improve enemy coordination:
- Add squad-based behaviors
- Implement tactical positioning
- Create role-based AI strategies

### Priority 3: Feature Additions

#### 3.1 Skill Tree System
```cpp
class USkillTreeComponent : public UActorComponent
{
    TMap<FName, FSkillNode> UnlockedSkills;
    
    bool CanUnlockSkill(FName SkillID);
    void UnlockSkill(FName SkillID);
    void ApplySkillEffects(FName SkillID);
};
```

#### 3.2 Advanced Movement
- Wall running for CyberState
- Ledge grabbing system
- Parkour mechanics

#### 3.3 Combo System
- Chain attacks for combat character
- Timing-based damage multipliers
- Special finisher moves

---

## DOCUMENTATION CONSOLIDATION

### Current Documentation Files (TO BE ARCHIVED)
The following 22 .md files contain valuable information that has been consolidated into this overview:

**Root Level**:
- ✓ CLAUDE.md - Quick reference guide
- ✓ DETAILEDCLAUDE.md - Comprehensive documentation
- ✓ IMPLEMENTATION_ROADMAP.md - Development roadmap

**Documentation/CORE/**:
- ✓ ARCHITECTURE.md - Core principles
- ✓ CODING_STANDARDS.md - Code guidelines
- ✓ FILE_STRUCTURE.md - Directory organization

**Documentation/SYSTEMS/**:
- ✓ CHARACTER_SYSTEM.md - Character mechanics
- ✓ COMBAT_SYSTEM.md - Combat rules
- ✓ ENEMY_SYSTEM.md - Enemy types
- ✓ GAME_FLOW.md - Game loop

**Documentation/COMPONENTS/**:
- ✓ ABILITIES.md - Ability components
- ✓ AI_CONTROLLERS.md - AI systems
- ✓ ATTRIBUTES.md - Stat management
- ✓ UI_COMPONENTS.md - UI systems

**Documentation/LOGS/**:
- ✓ CHANGELOG.md - Version history
- ✓ ERROR_LOG.md - Common errors
- ✓ KNOWN_ISSUES.md - Bug tracking

**Source/**:
- ✓ README_Component_System.md - Component details
- ✓ README_Enemy_Classes.md - Enemy implementation
- ✓ README_Implementation.md - Feature status
- ✓ README_Includes.md - Include guidelines
- ✓ README_Structure.md - Code organization

### Recommended Documentation Structure
```
PROJECT_OVERVIEW.md (this file) - Single source of truth
API_REFERENCE.md - Generated from code comments
QUICK_START.md - For new developers
RELEASE_NOTES.md - User-facing changes only
```

---

## BUILD & SETUP INSTRUCTIONS

### Prerequisites
- Unreal Engine 5.4
- JetBrains Rider 2024.x OR Visual Studio 2022
- Windows 10/11 (WSL not supported)
- 16GB RAM minimum
- .NET Framework 4.8 or later

### Build Steps - JetBrains Rider (Recommended)
```bash
# 1. Generate project files for Rider
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\GenerateProjectFiles.bat" "D:\Unreal Projects\cybersouls\cybersouls.uproject" -Game -Rider

# 2. Open cybersouls.uproject directly in Rider
# Rider will auto-detect and configure the Unreal Engine integration

# 3. Select build configuration: "Development Editor | Win64"

# 4. Build: Ctrl+F9 or Build → Build Solution

# 5. Launch: Select "cybersouls (Development Editor)" and press F5
```

### Build Steps - Visual Studio
```bash
# 1. Generate project files
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\GenerateProjectFiles.bat" "D:\Unreal Projects\cybersouls\cybersouls.uproject" -Game

# 2. Open cybersouls.sln in Visual Studio

# 3. Build Configuration: Development Editor

# 4. Launch from Unreal Engine or F5 in VS
```

### Rider-Specific Configuration
1. **Install Unreal Engine Support**:
   - File → Settings → Plugins → Search "Unreal Engine"
   - Install and restart Rider

2. **Configure Unreal Engine Path**:
   - File → Settings → Build, Execution, Deployment → Unreal Engine
   - Set Engine Installation: `C:\Program Files\Epic Games\UE_5.4`

3. **Recommended Rider Settings**:
   ```
   Editor → Code Style → C++ → Naming:
   - Class prefix: A, U, F, S, I (per UE conventions)
   - Function naming: PascalCase
   - Parameter naming: PascalCase
   
   Editor → Inspection Settings:
   - Enable "Unreal Engine 5" inspection profile
   - Set severity for UE5-specific warnings
   ```

4. **Useful Rider Features for UE5**:
   - **Unreal Header Tool Integration**: Auto-generates reflection code
   - **Blueprint References**: Find C++ usage in Blueprints
   - **UPROPERTY/UFUNCTION Completion**: IntelliSense for UE macros
   - **Hot Reload**: Compile without closing editor (Ctrl+Alt+F11)
   - **Attach to Process**: Debug → Attach to Process → UnrealEditor.exe

### Blueprint Setup Requirements
```
BP_CyberSoulsPlayerController:
├── DefaultCharacterClass = BP_cybersoulsCharacter
├── CyberStateCharacterClass = BP_PlayerCyberState
└── InputConfig = DA_CyberSoulsInputConfig

Character Blueprints must include:
├── TargetingComponent
├── PlayerProgressionComponent
└── Character-specific ability components
```

### Critical Configuration
1. Set InventoryWidgetClass in BP_CybersoulsHUD
2. Configure input actions in DA_CyberSoulsInputConfig
3. Ensure NavMesh covers playable areas
4. Set enemy AI controllers in enemy blueprints

---

## KNOWN ISSUES & SOLUTIONS

### Critical Issues

#### Character Switch During Death
**Problem**: Switching characters while dying causes undefined behavior  
**Solution**: Add death state check to switch validation
```cpp
if (PlayerAttributes && PlayerAttributes->GetHackProgress() > 180)
{
    return; // Prevent switch near death
}
```

#### Build Failures in WSL
**Problem**: Cannot build from WSL environment  
**Solution**: Always use Windows Command Prompt or PowerShell

### Common Development Issues

#### Components Not Found
**Problem**: FindComponentByClass returns null  
**Solution**: Ensure component is added in constructor with UPROPERTY
**Rider Tip**: Use "Find Usages" (Alt+F7) to verify component initialization

#### Input Not Working
**Problem**: Actions not triggering  
**Solution**: Verify input mapping context is added on possession
**Rider Tip**: Set breakpoint in SetupPlayerInputComponent and debug

#### AI Not Detecting Player
**Problem**: Enemies ignore player  
**Solution**: Check perception configuration and nav mesh
**Rider Tip**: Use "Attach to Process" to debug AI behavior in PIE

#### Rider-Specific Issues

##### Intellisense Not Working
**Problem**: No code completion for UE types  
**Solution**: 
1. Regenerate project files with `-Rider` flag
2. File → Invalidate Caches and Restart
3. Ensure .uproject association is correct

##### Hot Reload Failing
**Problem**: Changes not reflected after compile  
**Solution**: 
1. Use Live Coding (Ctrl+Alt+F11) instead
2. If persists, close UE editor and rebuild
3. Delete Binaries/Intermediate folders if needed

##### Debugging Breakpoints Not Hit
**Problem**: Breakpoints ignored during debugging  
**Solution**: 
1. Ensure "Development Editor" configuration
2. Attach to UnrealEditor.exe process
3. Check symbol loading in Debug window

### Performance Optimizations
- Limit death effects to 50 pieces
- Use object pooling for projectiles
- Disable tick on idle components
- LOD aggressive culling for enemies

---

## CONCLUSION

CyberSouls demonstrates solid architectural foundations with its component-based design and interface-driven communication. The project is well-documented and follows UE5 best practices. The main opportunities for improvement lie in further modularizing monolithic classes and implementing the planned features like the buff system and environmental impacts.

### Next Steps
1. Implement Priority 1 refactoring recommendations
2. Complete buff system and environmental mechanics
3. Add skill tree progression
4. Polish combat feel and game balance
5. Implement audio and VFX

### Development Philosophy
> "Keep components focused, interfaces clean, and communication event-driven."

The project is ready for content expansion and team scaling. The modular architecture ensures new features can be added without disrupting existing systems.

---

**Document maintained by**: AI Assistant  
**Last updated**: May 29, 2025  
**Next review**: After Priority 1 refactoring complete
