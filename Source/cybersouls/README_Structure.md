# Cybersouls Source Code Structure

The source code has been organized following Unreal Engine conventions:

## Directory Structure

```
Source/cybersouls/
├── cybersouls.Build.cs       (Build configuration - root)
├── cybersouls.cpp            (Module implementation - root)
├── cybersouls.h              (Module header - root)
├── Public/                   (All header files)
│   ├── AI/
│   │   ├── PhysicalEnemyAIController.h
│   │   └── HackingEnemyAIController.h
│   ├── Abilities/
│   │   └── AbilityComponent.h
│   ├── Attributes/
│   │   └── AttributeComponent.h
│   ├── Character/
│   │   └── cybersoulsCharacter.h
│   ├── Combat/
│   │   ├── BodyPartComponent.h
│   │   └── TargetLockComponent.h
│   ├── Enemy/
│   │   └── CybersoulsEnemyBase.h
│   ├── Game/
│   │   └── cybersoulsGameMode.h
│   └── UI/
│       └── CybersoulsHUD.h
└── Private/                  (All cpp files)
    ├── AI/
    │   ├── PhysicalEnemyAIController.cpp
    │   └── HackingEnemyAIController.cpp
    ├── Abilities/
    │   └── AbilityComponent.cpp
    ├── Attributes/
    │   └── AttributeComponent.cpp
    ├── Character/
    │   └── cybersoulsCharacter.cpp
    ├── Combat/
    │   ├── BodyPartComponent.cpp
    │   └── TargetLockComponent.cpp
    ├── Enemy/
    │   └── CybersoulsEnemyBase.cpp
    ├── Game/
    │   └── cybersoulsGameMode.cpp
    └── UI/
        └── CybersoulsHUD.cpp
```

## Include Path Updates

All include paths have been updated to use the full path from the module root:
- `#include "cybersouls/Public/[subfolder]/[file].h"`

## Notes

- The three core files (cybersouls.Build.cs, cybersouls.cpp, cybersouls.h) remain in the root directory
- All other header files are now in the Public directory
- All other cpp files are now in the Private directory
- Game-related files (GameMode) are in their own Game subdirectory
- Character files are now in the Character subdirectory
- AI controllers are specialized by enemy type
- Include paths have been updated to reflect the new structure

## Key Components

### AI System
- **PhysicalEnemyAIController**: AI for melee enemies with search behavior and communication
- **HackingEnemyAIController**: AI for ranged enemies with hacking abilities and QuickHacks

### Character
- **cybersoulsCharacter**: Main player character class

This organization follows Unreal Engine best practices and makes the codebase more maintainable.