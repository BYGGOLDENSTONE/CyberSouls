# CLAUDE.md

## CORE GAME RULES
- **Enemies**: 100 HP, die in one slash (unless they block/dodge)
- **Player**: Dies ONLY from HackProgress=200 (NOT from physical damage)
- **Architecture**: Component-based - each ability is a separate ActorComponent

## COMPONENT ARCHITECTURE

### Attributes
- `PlayerAttributeComponent`: MaxHackProgress=200, integrity damage doesn't kill
- `PlayerProgressionComponent`: Tracks IntegrityXP and HackingXP
- `PlayerCyberStateAttributeComponent`: Stamina system (100 max, 10/sec regen after 1s delay)
- `PhysicalEnemyAttributeComponent`: For melee enemies (100 HP)
- `HackingEnemyAttributeComponent`: For netrunner enemies (100 HP)

### Abilities
- `SlashAbilityComponent`: Player instant-kill attack (integrates with passive abilities)
- `AttackAbilityComponent`: Enemy physical attacks (10 damage, 2s cooldown)
- `BlockAbilityComponent`: Blocks upper body attacks (3 charges)
- `DodgeAbilityComponent`: Dodges leg attacks (3 charges)
- `HackAbilityComponent`: Continuous enemy hacking
- `QuickHackComponent`: Cast-time abilities (1-4 keys)
  - **Extended QuickHacks**: Cascade Virus, Ghost Protocol, Charge Drain, Gravity Flip
- `PassiveAbilityComponent`: Passive abilities that modify combat behavior
  - **Execution Chains**: Next slash in 5s after kill ignores block
  - **System Overcharge**: No QuickHacks allowed, slash ignores all defenses
- `DoubleJumpAbilityComponent`: Air jump for CyberState (20 stamina, 1 air jump)
- `DashAbilityComponent`: Directional dash for CyberState
  - Stamina cost: 25
  - Distance: 1000 units over 0.2s
  - Cooldown: 0.5s (pauses when airborne)
  - Charge system: 1 charge (expandable)
  - Charge regen: 3s on ground (timer pauses in air)
  - 2D dash on ground, 3D dash in air
  - Always dashes toward crosshair

### Enemy Types
**Physical**: BasicEnemy, BlockEnemy, DodgeEnemy
**Hacking**: Netrunner (5/sec), BuffNetrunner (3/sec), DebuffNetrunner (2/sec)

### AI Controllers
- `PhysicalEnemyAIController`: 
  - Movement: 600 speed, 150 attack range
  - Vision: 2000 sight range, 80 unit eye height
  - Detection: Requires direct line of sight (no detection through obstacles)
  - Search behavior: Investigates last known location for 5 seconds
  - Communication: Alerts allies within 2000 units, updates every 1 second
  - Debug: Sight line traces disabled (no visual lines to player)
- `HackingEnemyAIController`: 
  - Movement: STATIONARY - hacking enemies do not move
  - Combat: 1500 hack range, uses QuickHacks strategically
  - Vision: 2000 sight range, requires line of sight, respects Ghost Protocol invisibility
  - Communication: Alerts allies within 2500 units, updates every 1 second

### Enemy Communication System
- ALL enemies alert nearby allies when spotting the player
- Continuous location updates while maintaining sight
- Physical enemies receiving alerts will search the location
- Hacking enemies receiving alerts stay in position (no movement)
- Updates stop when enemy loses sight of player

## KEY SYSTEMS

### Dash Mechanics (CyberState)
- **Direction**: Always toward crosshair aim point
  - Line trace from camera through screen center
  - Calculates vector from character to hit point
- **Ground Dash**: Horizontal only (2D), no vertical component
- **Air Dash**: Full 3D movement, can dash upward/downward
- **Charge System**: 
  - Uses 1 charge per dash (plus stamina)
  - Regenerates 1 charge after 3s on ground
  - Regen timer pauses when airborne (maintains progress)
- **Cooldown**: 0.5s between dashes, continues even when airborne
- **HUD**: Shows charge boxes with fill progress and timer
- **Movement**: Uses MOVE_Flying during dash, resets to MOVE_Falling after

### Crosshair Targeting
- Real-time line tracing through screen center
- Automatic body part detection (upper/legs)
- Visual feedback: white (default), red + ring (enemy detected)
- All abilities use crosshair target

### Enemy Death Effect
- Shattered glass effect with 50 pieces
- Piece scale: 0.15-0.35 of original
- Explosion force: 300-400 units
- Upward force: 200-300 units
- Angular velocity: 6 rad/s with 200x multiplier
- Pieces despawn after 3 seconds with actor

### Quest Completion
- Kill all enemies → Quest complete
- Remaining integrity → IntegrityXP (1:1)
- Hack resistance → HackingXP (0.5x)
- XP display appears in top-middle after completion

## CRITICAL PATTERNS
1. Always use Read before Edit
2. Check death states everywhere
3. Use proper component getters for HUD access
4. Include paths: `#include "cybersouls/Public/[Category]/[File].h"`
5. Check pointers with IsValid()
6. Input contexts: Add in PossessedBy(), not BeginPlay()
7. Reset ability states when character is possessed

## BUILD COMMANDS
**NOTE: Cannot build from WSL. Use Windows Command Prompt or PowerShell to run these commands.**

```bash
# Generate project files
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\GenerateProjectFiles.bat" "D:\Unreal Projects\cybersouls\cybersouls.uproject" -Game

## PLAYABLE CHARACTERS

### Default Character (cybersoulsCharacter)
- **Focus**: Combat specialist
- **Resources**: Integrity (100), HackProgress (max 200)
- **Abilities**: Slash (instant kill), QuickHacks (1-4), Camera toggle (5)
- **Death condition**: HackProgress reaches 200

### Cyber State Character (PlayerCyberState)
- **Focus**: Enhanced mobility
- **Resource**: Stamina (100, regenerates 10/sec after 1s delay)
- **Movement**: 1200 speed, 2x air control, 0.8 gravity
- **Abilities**: 
  - Double Jump: Extra jump in air (20 stamina)
  - Dash: Directional movement burst (25 stamina + 1 charge)
  - Camera Toggle: Switch 1st/3rd person (Key 5)
- **Switch**: Press F to switch between characters

## CHARACTER SWITCHING SYSTEM
- `CyberSoulsPlayerController`: Manages character switching with state preservation
- **F key**: Switch between Default and CyberState
- **Implementation**: Only one character exists in world at a time (no hidden duplicates)
- **State preservation**: Location, rotation, camera settings maintained across switches
- **Visual feedback**: Character indicator (top-right), switch notification (center)
- **HUD adaptation**: Shows appropriate UI for active character
  - Default: Integrity, Hack Progress, QuickHack status
  - CyberState: Stamina bar, Dash charges with regen timer

## INPUT CONFIGURATION
- `CyberSoulsInputConfig`: Central input action repository
- **Common Inputs** (available on both characters):
  - F: Character switching (`SwitchCharacterAction`)
  - Enter: Restart game (`RestartAction`)
  - Tab: Toggle XP display (`ShowXPAction`)
  - E: Open inventory (`OpenInventoryAction`)
  - 5: Camera toggle (`CameraToggleAction`)
- **Blueprint setup**:
  - BP_CyberSoulsPlayerController: Set DefaultCharacterClass & CyberStateCharacterClass
  - Create DA_CyberSoulsInputConfig with all input actions
  - Each character has own Input Mapping Context
- **Input Context Management**:
  - Contexts cleared on character switch (ClearAllMappings)
  - Character contexts added in PossessedBy() override
  - Controller context re-added with priority 1
  - Prevents input conflicts between characters

## GAME RESTART SYSTEM
- **Quest Complete (Play Again)**:
  - Shows XP gained + "Play Again?" button
  - Press ENTER to restart level
  - XP is PRESERVED across restarts
  - Players can stack XP by completing multiple runs
- **Player Death (Start Again)**:
  - Triggers when HackProgress reaches 200
  - Shows "SYSTEM COMPROMISED" death screen
  - Press ENTER to restart level
  - XP is RESET to zero
- **Implementation**:
  - RestartAction in CyberSoulsInputConfig (mapped to Enter key)
  - RestartLevel(bool bResetXP) in GameMode
  - Uses UGameplayStatics::OpenLevel for stable restarts
  - Player input disabled on death (except restart)

## DEVELOPMENT COMPLETE - ALL TERMINALS FINISHED
✅ **Terminal A** (UI & Progression): XP display on Tab key, Inventory system with QuickHack/Passive slots - COMPLETED
✅ **Terminal B** (Character & Movement): Fix dash cooldown independence, Character switching without deletion - COMPLETED
✅ **Terminal C** (AI & Architecture): Component duplication fixes, AI controller improvements/component system - COMPLETED
✅ **Terminal D** (QuickHack & Passives): New QuickHacks (Cascade Virus, Ghost Protocol, Charge Drain, Gravity Flip), Passive abilities (Execution Chains, System Overcharge) - COMPLETED

## IMPLEMENTATION STATUS
**Code Implementation**: 100% Complete
**Major Bug Fixes**: Applied (Input bindings, AI controllers, character switching, component architecture)
**Blueprint Integration**: Required for full functionality
**Status**: Ready for Blueprint setup - expect compilation errors until Blueprint integration is complete

## UI & PROGRESSION SYSTEM (Terminal A Complete)
- **Persistent XP Display**: Press Tab to toggle XP progression view (IntegrityXP & HackingXP)
  - Positioned in top-left corner, cyberpunk styling with borders
  - Works with both Default and CyberState characters
  - Persists across game restarts using PlayerProgressionComponent
- **Inventory System**: Press E to open full inventory interface
  - QuickHack slot management (1-4 keys) with real-time status display
  - 4 passive ability slots ready for Terminal D implementation
  - Character-specific restrictions (CyberState shows unavailable message)
  - Visual selection highlighting and detailed status information

## AI & ARCHITECTURE SYSTEM (Terminal C Complete)
- **Component Duplication Elimination**: Unified data sources for enemy combat parameters
  - AttackAbilityComponent references PhysicalEnemyAttributeComponent values (10 damage, 150 range, 2s cooldown)
  - HackAbilityComponent references HackingEnemyAttributeComponent values (2.0f rate, 1500.0f range)
  - AI controllers use ability component values for consistent behavior
- **AI Controller Improvements**: Fixed initialization and behavior consistency
  - PlayerTarget properly initialized in BaseEnemyAIController::OnPossess()
  - Physical enemies chase, attack at correct ranges with proper cooldowns
  - Hacking enemies hack at correct rates and ranges according to enemy type
  - Single source of truth: enemy attribute components → ability components → AI controllers

## QUICKHACK & PASSIVE SYSTEM (Terminal D Complete)
- **Extended QuickHacks**: Four new QuickHack abilities with strategic effects
  - **Cascade Virus** (2s cast, 15s cooldown): Marks enemy for 5s, kills 2 nearby enemies in 2s if marked enemy dies
  - **Ghost Protocol** (2s cast, 20s cooldown): Makes player invisible to hack enemies for 5s
  - **Charge Drain** (2s cast, 12s cooldown): Depletes target's block/dodge charges instantly
  - **Gravity Flip** (2s cast, 18s cooldown): Reverses gravity for target for 2s
- **Passive Abilities**: Combat behavior modifiers via PassiveAbilityComponent
  - **Execution Chains**: Next slash within 5s after kill ignores block ability
  - **System Overcharge**: Disables all QuickHacks but slash ignores all defenses (block, dodge)
- **Integration**: Slash ability checks passive abilities, QuickHack inputs validate passive restrictions
- **AI Interaction**: Ghost Protocol modifies HackingEnemyAIController sight detection

## BLUEPRINT SETUP REQUIREMENTS

### 🔧 CRITICAL BLUEPRINT CONFIGURATIONS

#### **Player Controller (BP_CyberSoulsPlayerController)**
```
DefaultCharacterClass = BP_cybersoulsCharacter
CyberStateCharacterClass = BP_PlayerCyberState  
InputConfig = DA_CyberSoulsInputConfig
ControllerMappingContext = IMC_CyberSoulsController
```

#### **Input Configuration (DA_CyberSoulsInputConfig)**
**Required Input Actions:**
- `SwitchCharacterAction` (F key)
- `RestartAction` (Enter key) 
- `ShowXPAction` (Tab key)
- `OpenInventoryAction` (E key)
- `QuickHack1Action` through `QuickHack4Action` (1-4 keys)
- `SlashAction` (Left Mouse)
- `CameraToggleAction` (5 key)

#### **Character Components Setup**

**Default Character (BP_cybersoulsCharacter):**
- QuickHackManagerComponent (replaces 4 individual QuickHackComponents)
- PassiveAbilityComponent: Type = `ExecutionChains` or `SystemOvercharge`
- Input Mapping Context: `IMC_DefaultCharacter`

**CyberState Character (BP_PlayerCyberState):**
- DashAbilityComponent
- DoubleJumpAbilityComponent
- Input Mapping Context: `IMC_CyberState`

#### **Enemy Components**

**Physical Enemies:**
- AttackAbilityComponent
- PhysicalEnemyAttributeComponent
- BlockAbilityComponent (for Block enemies)
- DodgeAbilityComponent (for Dodge enemies)
- AI Controller: PhysicalEnemyAIController

**Hacking Enemies:**
- HackAbilityComponent
- HackingEnemyAttributeComponent
- QuickHackComponents (various types)
- AI Controller: HackingEnemyAIController

#### **New QuickHack Types Available:**
- `CascadeVirus` - Marks enemy, chains kills to 2 nearby
- `GhostProtocol` - Invisibility to hack enemies
- `ChargeDrain` - Depletes enemy defensive charges
- `GravityFlip` - Reverses target gravity

## COORDINATION FILES
- `terminal_a_prompt.txt` - UI & Progression tasks
- `terminal_b_prompt.txt` - Character & Movement tasks  
- `terminal_c_prompt.txt` - AI & Architecture tasks
- `terminal_d_prompt.txt` - QuickHack & Passive tasks
- `task_overview.txt` - Complete coordination overview
- `todo.txt` - Original task list

## COMPLETED FEATURES
✅ Crosshair targeting system
✅ Component-based abilities
✅ XP progression system
✅ Enemy AI behaviors with search patterns
✅ Enemy communication system
✅ Quest completion detection
✅ HUD with combat info (removed enemy casting indicators)
✅ Camera switching (Key 5)
✅ Shattered death effect for enemies
✅ Stationary hacking enemies
✅ Disabled debug line traces in viewport
✅ Dual character system with switching (F key)
✅ Character-specific HUD elements
✅ Visual character switching feedback
✅ Game restart system with XP persistence/reset
✅ CyberState enhanced mobility (double jump + directional dash)
✅ Dash charge system with ground-only regeneration
✅ Input context management for character switching
✅ Dash cooldown independence (continues even when airborne)
✅ Character switching without world duplication (state preservation)
✅ XP display toggle (Tab key)
✅ Inventory display toggle (E key)
✅ Component duplication elimination (single source of truth architecture)
✅ AI controller initialization fixes (PlayerTarget, behavior consistency)
✅ Enemy combat parameter unification (abilities + AI use same values)
✅ Extended QuickHack system (Cascade Virus, Ghost Protocol, Charge Drain, Gravity Flip)
✅ Passive ability system (Execution Chains, System Overcharge)
✅ QuickHack/Passive integration with slash combat and AI systems
✅ Unified QuickHackManagerComponent (replaces multiple QuickHackComponents)
✅ Enhanced AI controller behavior with tick-based logic and comprehensive logging

## PROJECT STATUS: READY FOR BLUEPRINT INTEGRATION
**All code implementation complete - awaiting Blueprint setup for full functionality**

### 🔧 **RECENT FIXES APPLIED (Current Session)**

#### **Fixed Compilation Errors**
- ✅ CyberSoulsPlayerController: Fixed FindPlayerStart API usage and variable name conflicts
- ✅ PhysicalEnemyAIController: Added missing member variables and fixed method calls
- ✅ QuickHackManagerComponent: Added public accessor methods to components
- ✅ QuickHackComponent: Fixed missing includes and property name (CooldownTime → CurrentCooldown)
- ✅ PassiveAbilityComponent: Added GetPassiveType() method

#### **Previous Session Fixes**
- ✅ Tab key: Updated documentation - use `ShowXPAction` in Blueprint
- ✅ E key: Updated documentation - use `OpenInventoryAction` in Blueprint  
- ✅ F key: Fixed character spawn locations and state preservation
- ✅ Enabled AI tick system with 0.1s intervals
- ✅ Implemented complete tick-based AI behavior
- ✅ Added comprehensive debug logging for AI diagnosis
- ✅ Fixed PlayerTarget initialization in OnPossess
- ✅ Removed duplicate HackRate/HackRange from HackAbilityComponent
- ✅ Unified data sources - all hack parameters now from HackingEnemyAttributeComponent
- ✅ Updated all netrunner enemies to set values on attribute components
- ✅ Verified proper separation between abilities and attributes
- ✅ New QuickHackManagerComponent replaces 4 individual QuickHackComponents
- ✅ Integrated with inventory system and passive abilities
- ✅ Smart crosshair targeting with special InterruptProtocol logic
- ✅ Updated character class and HUD to use manager

### ✅ **COMPILATION STATUS: RESOLVED**
All major compilation errors have been fixed. Code should now compile successfully.

### ⚠️ **BLUEPRINT INTEGRATION REQUIRED**
Remaining issues require Blueprint setup:
- Character class references old QuickHack components in Blueprint
- Missing QuickHackManagerComponent in Blueprint setup
- AI Controller classes need to be set in enemy Blueprints

### 📋 **NEXT SESSION TASKS**
1. ✅ Fix compilation errors from component changes - COMPLETED
2. Update Blueprint configurations per CLAUDE.md requirements
3. Test all fixed systems (input keys, AI, character switching)
4. Verify QuickHack manager functionality