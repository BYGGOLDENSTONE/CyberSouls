# FILE_STRUCTURE.md

## Metadata
- **Category**: CORE
- **Type**: File Structure Documentation
- **Version**: 1.0
- **Last Updated**: 2025-01-24
- **Dependencies**: Unreal Engine Project Structure

## Purpose
Documents the directory layout and file organization patterns for the Cybersouls project.

## Project Root Structure
```
/mnt/d/Unreal Projects/cybersouls/
├── Config/                     # Engine and project configuration
├── Content/                    # Binary assets (Blueprint, Materials, etc.)
├── Documentation/              # Project documentation
│   ├── CORE/                  # Core documentation files
│   └── LOGS/                  # Changelog and issue tracking
├── Source/                     # C++ source code
│   └── cybersouls/            # Main game module
├── cybersouls.uproject        # Project file
└── CLAUDE.md                  # AI assistant instructions
```

## Source Code Structure
```
Source/cybersouls/
├── Public/                     # Header files (.h)
│   ├── Abilities/             # Ability components
│   ├── AI/                    # AI controllers and tasks
│   ├── Character/             # Character classes
│   ├── Combat/                # Combat system
│   ├── Components/            # Generic components
│   ├── HUD/                   # UI and HUD classes
│   ├── Input/                 # Input configuration
│   ├── Interfaces/            # Game interfaces
│   ├── Player/                # Player-specific components
│   ├── Progression/           # XP and leveling
│   └── Utility/               # Helper classes
├── Private/                    # Implementation files (.cpp)
│   └── [Same structure as Public]
├── cybersouls.Build.cs        # Build configuration
├── cybersouls.cpp             # Module implementation
└── cybersouls.h               # Module header
```

## Include Path Patterns

### Standard Include Format
```cpp
// From any .cpp file
#include "cybersouls/Public/[Category]/[FileName].h"

// Examples:
#include "cybersouls/Public/Abilities/SlashAbilityComponent.h"
#include "cybersouls/Public/AI/BaseEnemyAIController.h"
#include "cybersouls/Public/Character/cybersoulsCharacter.h"
```

### Internal Includes (Same Module)
```cpp
// Within the same category
#include "OtherFileInSameFolder.h"

// From different category
#include "../OtherCategory/FileName.h"
```

## File Naming Conventions

### C++ Files
- **Components**: `[Name]Component.h/cpp` (e.g., `SlashAbilityComponent`)
- **Abilities**: `[Name]AbilityComponent.h/cpp` (e.g., `DashAbilityComponent`)
- **Controllers**: `[Type]Controller.h/cpp` (e.g., `BaseEnemyAIController`)
- **Characters**: `[Name]Character.h/cpp` or just name (e.g., `PlayerCyberState`)
- **Interfaces**: `I[Name].h` (e.g., `ITargetable.h`)

### Blueprint Assets
- **Prefix Convention**:
  - `BP_` for Blueprints (e.g., `BP_BasicEnemy`)
  - `WBP_` for Widget Blueprints (e.g., `WBP_PlayerHUD`)
  - `BTT_` for Behavior Tree Tasks
  - `BTS_` for Behavior Tree Services
  - `BT_` for Behavior Trees
  - `BB_` for Blackboards
  - `DA_` for Data Assets (e.g., `DA_CyberSoulsInputConfig`)

## Module Dependencies

### cybersouls.Build.cs
```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core",
    "CoreUObject", 
    "Engine",
    "InputCore",
    "EnhancedInput",
    "AIModule",
    "NavigationSystem",
    "GameplayTasks",
    "UMG",
    "Slate",
    "SlateCore"
});
```

## Content Organization

### Asset Folders
```
Content/
├── Characters/                 # Character blueprints and meshes
│   ├── Player/
│   └── Enemies/
├── Abilities/                  # Ability effects and data
├── AI/                        # Behavior trees and blackboards
├── UI/                        # HUD and menu widgets
├── Input/                     # Input mappings and configs
├── Materials/                 # Materials and textures
├── Effects/                   # Particle systems and effects
└── Maps/                      # Level files
```

## Key File Locations

### Core Classes
- Player Controller: `Source/cybersouls/Public/Player/CyberSoulsPlayerController.h`
- Game Mode: `Source/cybersouls/Public/cybersoulsGameMode.h`
- Default Character: `Source/cybersouls/Public/Character/cybersoulsCharacter.h`
- Cyber State: `Source/cybersouls/Public/Character/PlayerCyberState.h`

### Component Systems
- Attribute Components: `Source/cybersouls/Public/Components/[Name]AttributeComponent.h`
- Ability Components: `Source/cybersouls/Public/Abilities/[Name]AbilityComponent.h`
- Player Components: `Source/cybersouls/Public/Player/[Name]Component.h`

### Configuration
- Input Config: `Source/cybersouls/Public/Input/CyberSoulsInputConfig.h`
- Project Settings: `Config/DefaultGame.ini`
- Engine Settings: `Config/DefaultEngine.ini`

## Build Output
```
Binaries/
├── Win64/                     # Windows builds
└── [Platform]/                # Other platform builds

Intermediate/
├── Build/                     # Compilation artifacts
└── ProjectFiles/              # Generated project files
```

## Documentation Files
```
Documentation/
├── CORE/
│   ├── ARCHITECTURE.md        # System architecture
│   ├── CODING_STANDARDS.md    # Code conventions
│   └── FILE_STRUCTURE.md      # This file
└── LOGS/
    ├── CHANGELOG.md           # Version history
    ├── ERROR_LOG.md           # Common errors
    └── KNOWN_ISSUES.md        # Bug tracking
```

## Import Guidelines

### Adding New Files
1. Place header in appropriate `Public/[Category]/` folder
2. Place source in matching `Private/[Category]/` folder
3. Use full include path: `#include "cybersouls/Public/[Category]/[File].h"`
4. Update relevant documentation if adding new category

### Blueprint Integration
1. Create C++ base class first
2. Create Blueprint in matching Content folder
3. Use consistent naming (e.g., `ABasicEnemy` → `BP_BasicEnemy`)
4. Set default values in Blueprint, not C++