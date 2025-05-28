# CLAUDE.md - CYBERSOULS QUICK REFERENCE

**For detailed documentation, see DETAILEDCLAUDE.md**

## CORE RULES
- **Death**: HackProgress=200 only (players immune to physical damage)
- **Combat**: Enemies die in one slash, 100 HP
- **Architecture**: Component-based with interface communication
- **AI**: Enemies chase Default Character only, ignore CyberState

## 🏗️ CRITICAL ARCHITECTURE (ALWAYS FOLLOW)
- **NEVER use FindComponentByClass** - use interfaces
- **Interfaces**: ITargetable, IAbilitySource, IAttributeOwner, IDamageReceiver, ICombatant
- **Components communicate via events/delegates**
- **ALL abilities inherit BaseAbilityComponent**
- **Movement abilities use TargetingComponent**
- **NO hardcoded values** - use UPROPERTY
- **Include format**: `#include "cybersouls/Public/[Category]/[File].h"`

## CHARACTERS

**Default (Combat)**: Slash, QuickHacks 1-4, Camera 5
- Resources: Integrity 100, HackProgress 0-200

**CyberState (Mobility)**: Double Jump, Dash
- Resources: Stamina 100, regens 10/sec

## KEY BINDINGS
- **F**: Switch character
- **Tab**: Inventory (dropdown menus)
- **Enter**: Restart
- **1-4**: QuickHacks
- **5**: Camera toggle
- **LMB**: Slash

## SYSTEMS
- **Character Pool**: No destroy/spawn on switch
- **Dash**: Toward crosshair (2D ground, 3D air)
- **Save**: XP persists on quest complete, resets on death
- **Inventory**: Canvas-based dropdowns, 8 QuickHacks + 6 Passives

## BLUEPRINT SETUP
```
PlayerController:
- DefaultCharacterClass = BP_cybersoulsCharacter
- CyberStateCharacterClass = BP_PlayerCyberState

Both Characters Need:
- TargetingComponent
- PlayerProgressionComponent

HUD:
- InventoryWidgetClass = WBP_Inventory
```

## BUILD
```bash
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\GenerateProjectFiles.bat" "D:\Unreal Projects\cybersouls\cybersouls.uproject" -Game
```

## CURRENT VERSION: v0.8.3
✅ Dropdown inventory complete
✅ XP persistence working
✅ AI targeting fixed
✅ All core systems operational