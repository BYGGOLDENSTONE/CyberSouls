# CHANGELOG.md

## Metadata
- **Category**: LOGS
- **Type**: Version History
- **Version**: Current
- **Last Updated**: 2025-01-24
- **Dependencies**: Git version control

## Purpose
Tracks version history, features, fixes, and changes for the Cybersouls project.

## Version Format
[MAJOR].[MINOR].[PATCH]
- MAJOR: Breaking changes or major features
- MINOR: New features, backwards compatible
- PATCH: Bug fixes and minor improvements

---

## [0.4.0] - Latest
### Added
- Dual character system with hot-swapping (F key)
- PlayerCyberState character with enhanced mobility
- Stamina system for CyberState abilities
- DoubleJumpAbilityComponent (20 stamina cost)
- DashAbilityComponent (25 stamina, 1000 units/0.2s)
- Character-specific HUD adaptation
- Visual feedback for character switching
- Game restart system with XP persistence
- Death/completion screen improvements

### Changed
- Refactored input system to use CyberSoulsInputConfig
- Updated HUD to support multiple character types
- Modified player controller for character management
- Enhanced death handling with input restrictions

### Fixed
- Input binding conflicts between characters
- HUD update issues during character switch
- XP persistence across level restarts

---

## [0.3.0] - Previous
### Added
- Enemy communication system
- AI search behavior for lost targets
- Crosshair targeting with body part detection
- Shattered glass death effect for enemies
- Quest completion with XP rewards
- Camera switching system (Key 5)

### Changed
- Hacking enemies now stationary
- Improved AI perception system
- Disabled debug line traces in viewport
- Updated enemy alert ranges

### Fixed
- Enemy detection through walls
- AI movement stuttering
- Death effect performance issues

---

## [0.2.0] - Early Development
### Added
- Component-based ability system
- Basic enemy types (Physical and Hacking)
- Player attribute system (Integrity/HackProgress)
- QuickHack abilities (Keys 1-4)
- Block and Dodge mechanics
- Basic AI behavior trees

### Changed
- Migrated from inheritance to composition
- Restructured project folders
- Improved ability validation

### Fixed
- Ability cooldown issues
- Component initialization order
- Enemy spawn problems

---

## [0.1.0] - Initial Commit
### Added
- Basic project structure
- Core game mode and player controller
- Initial character class
- Simple combat system
- Basic enemy placeholder

### Known Issues
- No AI behavior
- Limited combat options
- No progression system

---

## Upcoming Features (Planned)
- [ ] Level progression system
- [ ] Additional enemy types
- [ ] Environmental hazards
- [ ] Multiplayer support
- [ ] Save/Load system
- [ ] Audio implementation
- [ ] Visual effects polish
- [ ] Performance optimization

## Development Notes
- All versions use Unreal Engine 5.4
- Component architecture established in v0.2
- AI system foundation completed in v0.3
- Character system expanded in v0.4