# UI Components

## Overview
The UI system provides real-time combat information, character status, and game state feedback. It adapts dynamically based on the active character and game events.

## Main HUD System

### CyberSoulsHUD
**Location**: `Source/cybersouls/Public/UI/CyberSoulsHUD.h`
**Purpose**: Primary HUD displaying combat information
**Dependencies**:
- PlayerAttributeComponent
- PlayerCyberStateAttributeComponent
- CrosshairTargetingComponent
**Key Features**:
- Dynamic UI adaptation for character switching
- Real-time attribute updates
- Crosshair with targeting feedback
- Quest completion overlay
- Death screen overlay

## HUD Elements

### Combat Information Panel (Left Side)
**Default Character Display**:
- Integrity: Current/Max (e.g., "85/100")
- Hack Progress: Current/Max with bar (e.g., "45/200")
- Block Charges: Visual indicators (3 max)
- Dodge Charges: Visual indicators (3 max)

**CyberState Character Display**:
- Stamina: Current/Max with bar (e.g., "75/100")
- Regeneration status indicator

### Character Indicator (Top Right)
- Shows active character name
- "Default" or "CyberState"
- Updates on character switch

### Crosshair System (Center)
**Components**:
- Center dot
- Four directional lines
**States**:
- White: No target
- Red with ring: Enemy targeted
**Target Info Display**:
- Enemy name
- Body part targeted (Upper Body/Legs)
- Distance to target

### Switch Notification (Center)
- Appears for 2 seconds on character switch
- "Switched to [Character Name]"
- Fade in/out animation

## Overlay Systems

### Quest Complete Overlay
**Trigger**: All enemies eliminated
**Display Elements**:
- "QUEST COMPLETE" header
- XP breakdown:
  - Integrity XP gained
  - Hacking XP gained
  - Total XP earned
- "Play Again?" prompt
- "Press ENTER to restart" instruction
**Behavior**:
- Appears in screen center
- Blocks other UI elements
- Enables restart input

### Death Screen Overlay
**Trigger**: HackProgress reaches 200
**Display Elements**:
- "SYSTEM COMPROMISED" header
- Red tinted background
- "Start Again?" prompt
- "Press ENTER to restart" instruction
**Behavior**:
- Full screen coverage
- Disables player input (except restart)
- XP reset on restart

## UI Update System

### Event-Driven Updates
```cpp
// Attribute changes
PlayerAttributes->OnIntegrityChanged.AddDynamic(HUD, &ACyberSoulsHUD::UpdateIntegrity);
PlayerAttributes->OnHackProgressChanged.AddDynamic(HUD, &ACyberSoulsHUD::UpdateHackProgress);

// Stamina changes
StaminaComponent->OnStaminaChanged.AddDynamic(HUD, &ACyberSoulsHUD::UpdateStamina);

// Target changes
TargetingComponent->OnTargetChanged.AddDynamic(HUD, &ACyberSoulsHUD::UpdateCrosshair);
```

### Update Frequency
- Attributes: On change only (event-driven)
- Crosshair: Every frame (tick)
- Overlays: On state change

## Blueprint Integration

### Widget Blueprints
- **WBP_CyberSoulsHUD**: Main HUD layout
- **WBP_QuestComplete**: Quest completion overlay
- **WBP_DeathScreen**: Death screen overlay
- **WBP_Crosshair**: Crosshair widget

### Binding Examples
```cpp
// Text binding for integrity
FText::Format(LOCTEXT("Integrity", "Integrity: {0}/{1}"), 
    CurrentIntegrity, MaxIntegrity);

// Progress bar binding
StaminaBar->SetPercent(CurrentStamina / MaxStamina);

// Visibility binding
ChargeIcon->SetVisibility(ChargeCount > Index ? 
    ESlateVisibility::Visible : ESlateVisibility::Hidden);
```

## Performance Considerations

### Optimization Strategies
1. **Event-Driven Updates**: Only update when values change
2. **Cached References**: Store component references on BeginPlay
3. **Conditional Visibility**: Hide unused elements for inactive character
4. **Pooled Widgets**: Reuse overlay widgets instead of creating new

### Update Batching
```cpp
// Batch multiple updates in single frame
bPendingUpdate = true;
// In Tick:
if (bPendingUpdate) {
    UpdateAllElements();
    bPendingUpdate = false;
}
```

## Customization Points

### Styling
- Colors defined in HUD class defaults
- Fonts set in widget blueprints
- Animations configured in UMG

### Layout
- Anchor presets for different screen sizes
- Scalable UI elements
- Safe zone considerations