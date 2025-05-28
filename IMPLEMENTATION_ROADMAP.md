# CYBERSOULS AI IMPLEMENTATION ROADMAP

## PHASE 0: PROTOTYPING (0.5 weeks)
**Objective**: Validate core mechanics before full implementation

### P0.1: Integrity Death System
- Add float Integrity property to PlayerCyberStateAttributeComponent
- Modify death logic: death occurs when Integrity<=0 OR HackProgress>=200
- Test dual death conditions with console commands
- **Validation**: Player dies from both conditions, correct death type triggered

### P0.2: Basic Buff Framework
- Create minimal BuffComponent with TMap<FName,float> for buff storage
- Implement ApplyBuff/RemoveBuff with timer management
- Add buff expiration using FTimerManager
- **Validation**: Buffs apply, persist for duration, remove correctly

### P0.3: Environmental Impact Base
- Add knockback calculation to melee hit detection
- Implement basic impulse application to hit actors
- Create ground impact area detection using sphere overlap
- **Validation**: Hits cause knockback, ground impacts affect nearby actors

---

## PHASE 1: FOUNDATION (2 weeks)
**Objective**: Implement robust core systems

### P1.1: Enhanced Death System
**Dependencies**: None
**Files**: PlayerCyberStateAttributeComponent.h/.cpp, IDamageReceiver.h, CyberSoulsPlayerController.cpp

- Add Integrity property with validation and change delegates
- Extend IDamageReceiver interface with EDamageType enum
- Implement unified CheckDeathConditions function
- Add death state management and cleanup
- Create damage type specific death animations
- **Validation**: Both death conditions work, proper state management, correct animations

### P1.2: BuffComponent System
**Dependencies**: None
**Files**: Combat/BuffComponent.h/.cpp, Combat/BuffData.h, Interfaces/ICombatant.h

- Create FBuffData struct with ID, duration, properties map
- Implement BuffComponent as UActorComponent with active buff tracking
- Add timer-based buff expiration with cleanup
- Integrate with ICombatant interface
- Create buff visual/audio feedback system
- **Validation**: Multiple simultaneous buffs, proper expiration, memory stable

### P1.3: Environmental Impact System
**Dependencies**: TargetingComponent
**Files**: Combat/EnvironmentalImpactComponent.h/.cpp

- Create EnvironmentalImpactComponent with force calculation methods
- Implement wall launch physics using hit normals
- Add ground shockwave with configurable radius and falloff
- Integrate with existing TargetingComponent for area detection
- Add environmental impact visual/audio effects
- **Validation**: Consistent wall launches, proper area effects, stable performance

---

## PHASE 2: ENEMY AI EXPANSION (2 weeks)
**Objective**: Enhance AI with buff coordination and advanced behaviors

### P2.1: NetrunnerAIController
**Dependencies**: BuffComponent, QuickHackManagerComponent
**Files**: AI/NetrunnerAIController.h/.cpp

- Create NetrunnerAIController inheriting BaseEnemyAIController
- Implement ally detection with faction filtering and caching
- Add buff targeting priority system (threatened > low health > random)
- Create QuickHack usage against player with HackProgress generation
- Implement buff cooldown management and animation triggers
- **Validation**: Consistent ally buffing, proper targeting, smooth AI behavior

### P2.2: Enhanced PhysicalEnemyAI
**Dependencies**: BuffComponent
**Files**: AI/PhysicalEnemyAIController.cpp

- Add block mechanics with stamina and directional calculation
- Implement dodge patterns with timing and animation integration
- Create reaction system for nearby lethal hits and environmental impacts
- Add damage mitigation for buffed state with visual feedback
- Implement group coordination responses
- **Validation**: Block/dodge functional, proper reactions, coordinated behavior

### P2.3: Enemy Communication Enhancement
**Dependencies**: Existing alert system
**Files**: AI/BaseEnemyAIController.cpp, Combat/AlertSystem.cpp

- Extend alert system for buff coordination requests
- Add proximity-based ally enhancement triggers
- Implement group reaction to player lethal attacks
- Create priority communication for netrunner assistance
- Add alert propagation for environmental threats
- **Validation**: Coordinated responses, proper communication, enhanced group tactics

---

## PHASE 3: COMBAT MECHANICS (2 weeks)
**Objective**: Implement visceral dual-layer combat

### P3.1: Enhanced Damage System
**Dependencies**: EnvironmentalImpactComponent, BuffComponent
**Files**: Combat/WeaponComponent.cpp, Combat/MeleeAttackComponent.cpp

- Modify slash system to handle blocked/dodged attacks with feedback
- Add wall launch calculation based on attack direction and force
- Implement ground shockwave generation with area damage
- Create damage falloff system for environmental effects
- Add impact visual effects and screen shake
- **Validation**: Blocked attacks show impact, environmental consequences functional

### P3.2: Player Physical Damage
**Dependencies**: Enhanced Death System
**Files**: Character classes, IDamageReceiver implementations

- Add physical damage handling (10 damage per hit reduces Integrity)
- Implement damage reduction/armor calculation system
- Create hit reaction animations and impact feedback
- Add physical damage immunity frames to prevent stunlock
- Balance enemy attack patterns and damage output
- **Validation**: Player takes correct damage, proper hit reactions, balanced gameplay

### P3.3: Buffed Enemy Mechanics
**Dependencies**: BuffComponent, Enhanced Damage System
**Files**: Enemy classes, Combat/BuffComponent.cpp

- Create "lethal damage survived" state for buffed enemies
- Add heavy impact visual/audio feedback for survived lethal hits
- Implement area reaction triggers when buffed enemy hit
- Create buff visual indicators and status effects
- Balance buff duration and damage mitigation values
- **Validation**: Buffed enemies survive lethal hits, clear visual feedback, balanced mechanics

---

## PHASE 4: PROGRESSION SYSTEM (2 weeks)
**Objective**: Implement dual XP and upgrade systems

### P4.1: Dual XP System
**Dependencies**: Enhanced combat systems
**Files**: PlayerProgression/XPManager.h/.cpp, UI/HUDWidget.cpp

- Create IntegrityXP calculation based on physical combat performance
- Create HackProgressXP calculation based on cyberspace actions
- Implement XP award triggers on quest completion and combat events
- Add XP persistence using save game system
- Create XP display and progression feedback UI
- **Validation**: XP calculates correctly, persists between sessions, clear progression feedback

### P4.2: Upgrade Trees
**Dependencies**: Dual XP System
**Files**: PlayerProgression/UpgradeSystem.h/.cpp, UI/UpgradeTreeWidget.cpp

- Design physical combat upgrade tree with branching paths
- Design cyberspace ability upgrade tree with specializations
- Create upgrade point allocation and validation system
- Implement upgrade persistence and save/load functionality
- Add upgrade preview and cost calculation UI
- **Validation**: Both trees functional, proper gating, persistent upgrades

### P4.3: Ability Enhancement
**Dependencies**: Upgrade Trees, existing ability components
**Files**: Abilities/, PlayerProgression/UpgradeSystem.cpp

- Make existing abilities scalable with upgrade modifiers
- Create upgrade application system for ability modification
- Implement dynamic ability stat calculation based on upgrades
- Add upgrade effect visualization and feedback
- Balance progression curves and upgrade costs
- **Validation**: Abilities scale properly, upgrades apply correctly, balanced progression

---

## PHASE 5: INTEGRATION & POLISH (2 weeks)
**Objective**: Refine and optimize all systems

### P5.1: System Integration
**Dependencies**: All previous phases
**Tasks**: Cross-system testing and bug fixes

- Test all dual-layer combat scenarios and edge cases
- Verify death condition handling under all circumstances
- Validate XP calculation accuracy across all scenarios
- Ensure buff system stability with complex interactions
- Performance profiling and optimization pass
- **Validation**: All systems work together, stable performance, no critical bugs

### P5.2: Balance Pass
**Dependencies**: System Integration
**Tasks**: Gameplay tuning and optimization

- Tune enemy health, damage, and behavior values
- Balance netrunner abilities and HackProgress generation rates
- Adjust environmental impact forces and areas
- Optimize upgrade costs, benefits, and progression curves
- Fine-tune buff durations and effectiveness
- **Validation**: Balanced gameplay, appropriate difficulty curve, engaging progression

### P5.3: UI/UX Polish
**Dependencies**: All systems functional
**Files**: UI/, HUD components

- Update HUD for dual resource display (Integrity + HackProgress)
- Add comprehensive buff/debuff status indicators with tooltips
- Create intuitive upgrade tree interface with clear progression paths
- Implement environmental impact feedback (screen effects, audio)
- Add tutorial integration for new mechanics
- **Validation**: Clear UI communication, intuitive interaction, comprehensive feedback

---

## CRITICAL DEPENDENCIES MAP
```
BuffComponent → NetrunnerAI → Enhanced Combat
Enhanced Death System → Physical Damage → Integrity Management  
Environmental Impact → Enhanced Damage → Area Effects
XP System → Upgrade Trees → Ability Enhancement
```

## FILE STRUCTURE
```
Source/CyberSouls/
├── Combat/
│   ├── BuffComponent.h/.cpp
│   ├── BuffData.h  
│   ├── EnvironmentalImpactComponent.h/.cpp
│   └── WeaponComponent.cpp (modified)
├── AI/
│   ├── NetrunnerAIController.h/.cpp
│   └── PhysicalEnemyAIController.cpp (modified)
├── PlayerProgression/
│   ├── XPManager.h/.cpp
│   └── UpgradeSystem.h/.cpp
├── Interfaces/
│   ├── IDamageReceiver.h (modified)
│   └── ICombatant.h (modified)
└── UI/
    ├── HUDWidget.cpp (modified)
    └── UpgradeTreeWidget.cpp
```

## SUCCESS CRITERIA PER PHASE
- P0: Core mechanics prototyped and validated
- P1: Foundation systems robust and tested
- P2: AI demonstrates coordinated dual-layer threats
- P3: Visceral combat with environmental consequences functional
- P4: Progression system drives player engagement
- P5: All systems integrated, balanced, and optimized

**TOTAL TIMELINE: 8.5 weeks**