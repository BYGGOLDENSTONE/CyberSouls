# Game Flow System

## Overview
The game flow system manages quest progression, experience points, level completion, and restart mechanics. It provides two distinct restart paths based on success or failure, with different XP persistence rules.

## Quest System

### Quest Objectives
**Primary**: Eliminate all enemies in the level
**Tracking**: Automatic enemy count monitoring
**Completion**: Triggered when enemy count reaches 0

### Quest Completion Detection
```cpp
// In GameMode Tick
TArray<AActor*> Enemies;
GetWorld()->GetAllActorsOfClass(ABaseEnemy::StaticClass(), Enemies);

int32 AliveEnemies = 0;
for (AActor* Enemy : Enemies) {
    if (!Enemy->IsDead()) {
        AliveEnemies++;
    }
}

if (AliveEnemies == 0 && !bQuestCompleted) {
    OnQuestComplete();
}
```

## Experience System

### XP Types
**IntegrityXP**:
- Source: Remaining integrity on quest completion
- Rate: 1:1 (100 integrity = 100 XP)
- Purpose: Rewards avoiding damage

**HackingXP**:
- Source: Hack resistance (200 - HackProgress)
- Rate: 0.5x (100 resistance = 50 XP)
- Purpose: Rewards avoiding hacks

### XP Calculation
```cpp
void CalculateQuestXP() {
    int32 IntegrityXP = PlayerAttributes->GetIntegrity();
    int32 HackingXP = (200 - PlayerAttributes->GetHackProgress()) * 0.5f;
    
    ProgressionComponent->AddXP(IntegrityXP, HackingXP);
}
```

### XP Persistence
**Quest Complete**: XP preserved across restarts
**Player Death**: XP reset to 0
**Storage**: GameInstance (survives level reloads)

## Game States

### Active Gameplay
**Conditions**:
- Player alive (HackProgress < 200)
- Enemies remain in level
- All input enabled
**HUD**: Combat information displayed

### Quest Complete
**Trigger**: All enemies eliminated
**Display**:
```
QUEST COMPLETE
================
Integrity XP: +85
Hacking XP: +65
Total XP: 4,250
================
Play Again?
Press ENTER to restart
```
**State**:
- Player input active
- XP added to total
- Can restart with XP preserved

### Player Death
**Trigger**: HackProgress >= 200
**Display**:
```
SYSTEM COMPROMISED
==================
Start Again?
Press ENTER to restart
```
**State**:
- Player input disabled (except restart)
- Death overlay shown
- XP will reset on restart

## Restart System

### Restart Mechanics
**Input**: Enter key
**Implementation**: `UGameplayStatics::OpenLevel`
**Process**:
1. Save/Reset XP based on state
2. Open current level name
3. Fresh level load
4. Spawn default character

### XP Persistence Logic
```cpp
void RestartLevel(bool bResetXP) {
    if (bResetXP) {
        GameInstance->ResetPlayerXP();
    }
    // XP automatically persists if not reset
    
    FString CurrentLevel = GetWorld()->GetMapName();
    UGameplayStatics::OpenLevel(this, FName(*CurrentLevel));
}
```

### Restart Types

#### Success Restart (Play Again)
- Triggered after quest completion
- XP is PRESERVED
- Allows XP stacking across runs
- Maintains progression

#### Death Restart (Start Again)
- Triggered after system compromise
- XP is RESET to 0
- Fresh start penalty
- Encourages careful play

## Level Flow

### Typical Playthrough
1. **Level Start**:
   - Default character spawns
   - HUD initializes
   - Enemies activate

2. **Combat Phase**:
   - Player engages enemies
   - Resources consumed
   - Damage accumulated

3. **Resolution**:
   - Quest Complete → XP reward → Play again?
   - OR
   - Death → XP lost → Start again?

4. **Restart**:
   - Level reloads
   - Appropriate XP handling
   - Cycle continues

### Multi-Run Progression
```
Run 1: Complete → +150 XP (Total: 150)
Run 2: Complete → +175 XP (Total: 325)
Run 3: Death → Reset (Total: 0)
Run 4: Complete → +160 XP (Total: 160)
```

## GameMode Integration

### CybersoulsGameMode
**Responsibilities**:
- Quest tracking
- XP calculation
- Restart handling
- State management

**Key Functions**:
```cpp
void CheckQuestCompletion();
void OnQuestComplete();
void OnPlayerDeath();
void RestartLevel(bool bResetXP);
```

## UI Integration

### Quest Complete UI
**Widget**: `WBP_QuestComplete`
**Updates**:
- XP breakdown display
- Total XP counter
- Restart prompt

### Death Screen UI
**Widget**: `WBP_DeathScreen`
**Features**:
- Full screen overlay
- Red tint effect
- Restart prompt

## Save System Considerations

### Current Implementation
- XP stored in GameInstance
- Survives level reloads
- Lost on game exit

### Future Expansion Points
- Save XP to disk
- Multiple save slots
- Progression unlocks
- Stat tracking

## Performance Notes

### Optimization Strategies
- Enemy count cached
- Quest check throttled (0.5s)
- UI updates event-driven
- Minimal tick usage

### Level Loading
- OpenLevel ensures clean state
- No memory leaks from restart
- Consistent load times
- Proper actor cleanup