# KNOWN_ISSUES.md

## Metadata
- **Category**: LOGS
- **Type**: Bug Tracking
- **Version**: 1.0
- **Last Updated**: 2025-01-24
- **Dependencies**: Current build v0.4.0

## Purpose
Tracks known bugs, limitations, and workarounds for the Cybersouls project.

## Critical Issues

### [CRIT-001] Character Switch During Death
**Status**: Open
**Severity**: High
**Description**: Switching characters while one is dying can cause undefined behavior
**Reproduction**:
1. Take damage until near death
2. Press F to switch characters during death animation
3. Game state becomes inconsistent

**Workaround**: Disable character switching when HackProgress > 180
**Planned Fix**: Add death state check to switch validation

---

## High Priority Issues

### [HIGH-001] Enemy Perception Through Thin Walls
**Status**: Open
**Severity**: High
**Description**: Enemies can sometimes detect player through very thin geometry
**Reproduction**:
1. Stand behind thin wall or pillar
2. Enemy may still gain sight perception
3. Occurs with walls < 10 units thick

**Workaround**: Use thicker collision geometry
**Planned Fix**: Add additional line trace validation

### [HIGH-002] Stamina Regeneration During Actions
**Status**: Open
**Severity**: High
**Description**: Stamina continues regenerating during dash/jump animations
**Reproduction**:
1. Use dash ability
2. Stamina starts regenerating before dash completes
3. Can chain abilities faster than intended

**Workaround**: None currently
**Planned Fix**: Add action state tracking to stamina component

---

## Medium Priority Issues

### [MED-001] HUD Flicker on Character Switch
**Status**: Open
**Severity**: Medium
**Description**: Brief HUD flicker when switching between characters
**Reproduction**:
1. Press F to switch characters
2. HUD disappears for 1-2 frames
3. New HUD appears

**Workaround**: Cosmetic issue only
**Planned Fix**: Implement smoother HUD transition

### [MED-002] Death Effect Performance
**Status**: Open
**Severity**: Medium
**Description**: Multiple enemies dying simultaneously causes frame drop
**Reproduction**:
1. Kill 5+ enemies with single slash
2. Shatter effect causes temporary lag
3. More noticeable on lower-end systems

**Workaround**: Reduce MaxPieceCount in death effect
**Planned Fix**: Implement object pooling for pieces

### [MED-003] AI Communication Range Overlap
**Status**: Open
**Severity**: Medium
**Description**: Overlapping alert ranges can cause communication loops
**Reproduction**:
1. Multiple enemies in overlapping alert ranges
2. One spots player
3. Alerts can bounce between enemies

**Workaround**: Space enemies further apart
**Planned Fix**: Add communication cooldown

---

## Low Priority Issues

### [LOW-001] Crosshair Jitter on Edges
**Status**: Open
**Severity**: Low
**Description**: Crosshair targeting flickers between body parts at boundaries
**Reproduction**:
1. Aim at boundary between upper body and legs
2. Crosshair rapidly switches detection
3. Visual indicator flickers

**Workaround**: Aim at center of body parts
**Planned Fix**: Add detection smoothing/hysteresis

### [LOW-002] XP Display Overlap
**Status**: Open
**Severity**: Low
**Description**: Quest complete XP text can overlap with other UI
**Reproduction**:
1. Complete quest with full HUD visible
2. XP display may overlap stamina bar
3. Only occurs in specific resolutions

**Workaround**: UI clears after 3 seconds
**Planned Fix**: Adjust UI anchor points

### [LOW-003] Input Buffer on Death
**Status**: Open
**Severity**: Low
**Description**: Input commands buffered during death can trigger after restart
**Reproduction**:
1. Die while holding movement key
2. Press Enter to restart
3. Character may move immediately on spawn

**Workaround**: Release all keys before restart
**Planned Fix**: Clear input buffer on level load

---

## Limitations

### Design Limitations
- Maximum 50 enemies per level (performance)
- Single player only (no multiplayer support)
- No save system (progression only)
- Fixed camera angles (no free camera)

### Technical Limitations
- WSL build not supported (use Windows)
- Hot reload may miss some changes
- Blueprint nativization not tested
- Console platform compatibility unknown

### Balance Limitations
- Enemy AI has perfect accuracy when attacking
- No difficulty settings
- Fixed ability cooldowns (not upgradeable)
- Linear XP progression only

---

## Workaround Summary

### General Workarounds
1. **Performance**: Reduce enemy count or death effect pieces
2. **AI Issues**: Space enemies appropriately, avoid thin walls
3. **Input Issues**: Clear inputs before major actions
4. **HUD Issues**: Wait for transitions to complete

### Development Workarounds
1. **Build Issues**: Always build from Windows
2. **Hot Reload**: Restart editor if changes don't apply
3. **Blueprint Errors**: Compile C++ first, then Blueprint

---

## Fixed Issues (Historical)

### [FIXED-001] Enemy Casting Indicators
**Status**: Closed in v0.4.0
**Description**: HUD showed incorrect casting indicators
**Fix**: Removed casting indicators from HUD

### [FIXED-002] Debug Line Traces
**Status**: Closed in v0.3.0
**Description**: AI sight lines visible in game
**Fix**: Disabled bShowDebugLines

### [FIXED-003] Character Input Conflicts
**Status**: Closed in v0.4.0
**Description**: Both characters received input simultaneously
**Fix**: Implemented proper input context switching

---

## Reporting New Issues

### Issue Template
```
### [SEVERITY-###] Brief Title
**Status**: Open/In Progress/Closed
**Severity**: Critical/High/Medium/Low
**Description**: Clear description of the issue
**Reproduction**:
1. Step by step
2. To reproduce
3. The issue

**Workaround**: Temporary solution if any
**Planned Fix**: Intended solution
```

### Severity Guidelines
- **Critical**: Game-breaking, blocks progression
- **High**: Major gameplay impact, no easy workaround
- **Medium**: Noticeable issue, has workaround
- **Low**: Minor issue, cosmetic, or edge case