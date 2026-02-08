# Changelog

All notable changes to the RobotRally UE5 project.

---

## [0.4.0] - 2026-02-08

### Phase 3 Complete: Player Control & Lives System

#### Added
- **ARobotController** class for player input handling
  - WASD movement during Programming phase
  - Card selection (number keys 1-9)
  - Execute program (E key)
  - Undo selection (Backspace key)
  - Input bindings in `Config/DefaultInput.ini`

- **Lives & Respawn System**
  - 3 lives per robot (configurable)
  - Respawn at last checkpoint on death
  - Full health restore on respawn
  - Lives counter in HUD

- **Checkpoint System**
  - Sequential checkpoint collection (must reach in order 1→2→3...)
  - Out-of-order checkpoint feedback
  - Respawn point saved at each checkpoint
  - Victory condition when all checkpoints collected

#### Changed
- Separated input handling from GameMode to dedicated ARobotController
- Health system now integrated with respawn mechanics
- Event log shows detailed feedback for checkpoint errors
- HUD displays lives counter with heart symbols

#### Documentation
- Created comprehensive technical documentation (`docs/TECHNICAL_DOCUMENTATION.md`)
- Created user guide with controls and strategies (`docs/USER_GUIDE.md`)
- Updated README with documentation links
- Added this changelog

---

## [0.3.0] - 2026-02-05

### Phase 2 Complete: Card System & Tile Hazards

#### Added
- **Card Deck System**
  - 84-card deck with Fisher-Yates shuffle
  - 7 card types with priority values
  - Hand dealing based on robot damage
  - Discard pile and deck reshuffling

- **Card Programming**
  - 5 registers for card selection
  - Hand size reduction with damage (9 cards → 5 minimum)
  - Register visualization in HUD
  - Card priority display

- **Tile Hazards**
  - Conveyor belts (4 directions) with chain support
  - Laser tiles (1 damage per register)
  - Pit tiles (instant death)
  - Checkpoint tiles with sequential collection

- **Tile Effect Processing**
  - Automatic tile effects after each card execution
  - Conveyor chain system (max 10 links)
  - Conveyor arrow visualization
  - Checkpoint number labels

#### Changed
- GameMode now manages full card lifecycle
- Movement triggers tile effect processing
- HUD displays card hand and registers
- Event log shows tile effect feedback

---

## [0.2.0] - 2026-01-20

### Phase 1 Complete: Core Structure

#### Added
- **ARobotPawn** (ACharacter base class)
  - Grid-based positioning (GridX, GridY)
  - Health system (0-10 HP)
  - Death system with OnDeath delegate
  - Customizable body and direction meshes
  - Runtime material creation with color tinting

- **URobotMovementComponent**
  - Grid-based movement (`MoveInGrid`, `RotateInGrid`)
  - Smooth interpolation (VInterpTo, RInterpTo)
  - 4-direction facing system (North, East, South, West)
  - Movement validation with GridManager
  - OnGridPositionChanged delegate

- **AGridManager**
  - 10×10 grid using TMap<FIntVector, FTileData>
  - Dynamic tile mesh spawning
  - GridToWorld / WorldToGrid conversions
  - Tile type system (Normal, Pit, Conveyor, Laser, Checkpoint)
  - Runtime tile coloring

- **ARobotRallyGameMode**
  - State machine (Programming, Executing, GameOver)
  - 5-register system
  - Card execution pipeline
  - Test scene setup in BeginPlay()

- **ARobotRallyHUD**
  - Health bar display
  - Checkpoint counter
  - Game state indicator
  - Event log system (5-second fade, max 8 messages)

#### Technical
- Grid tile size: 100 UE units
- Movement interpolation speed: 5.0 units/sec
- Rotation interpolation: 5.0 deg/sec
- Grid coordinates use FIntVector(X, Y, 0)

---

## [0.1.0] - 2026-01-15

### Phase 0: Project Setup

#### Added
- Created UE5.7 C++ project structure
- Module dependencies: Core, CoreUObject, Engine, InputCore, EnhancedInput
- Basic folder structure (`Source/`, `Content/`, `Config/`)
- Copyright headers and code style guidelines
- Git repository initialization

#### Configuration
- Visual Studio 2022 integration
- Enhanced Input plugin enabled
- Development build configuration
- Tab-based indentation (size 4)

---

## Implementation Status

| Phase | Status | Version |
|-------|--------|---------|
| Phase 0: Project Setup | ✅ Complete | 0.1.0 |
| Phase 1: Core Structure | ✅ Complete | 0.2.0 |
| Phase 2: Card System | ✅ Complete | 0.3.0 |
| Phase 3: Player Control | ✅ Complete | 0.4.0 |
| Phase 4: Field Hazards | ✅ Complete | 0.4.0 |
| Phase 5: UI/UMG | ⚠️ Partial | — |
| Phase 6: Content & Polish | ❌ Not Started | — |

---

## Known Issues

### Current Bugs
- Conveyor arrows may Z-fight with tiles (increase Z-offset to 5.0)
- HUD text can overlap with long messages (needs text wrapping)
- Rapid card execution may cause movement overlap (mitigated with timers)

### Limitations
- Single-player only (no multi-robot support)
- No robot-robot collision
- No UI/UMG widgets (using Canvas HUD)
- No board walls or obstacles
- No sound effects or particle effects

---

## Upcoming Features

### Phase 5: UI/UMG
- Mouse-driven card selection
- Drag-and-drop register programming
- Modern widget-based HUD
- Card tooltips and descriptions
- Game over screen with restart button

### Phase 6: Content & Polish
- Multiple board layouts
- Board editor/designer
- Repair stations (heal damage)
- Flags (alternative checkpoints)
- Walls and obstacles
- Robot-robot collision and pushing
- Sound effects and visual effects
- Main menu and settings

### Future Enhancements
- Multiplayer (2-8 robots)
- AI opponents
- Custom card decks
- Campaign mode
- Online leaderboards

---

## Version Numbering

Format: `MAJOR.MINOR.PATCH`

- **MAJOR**: Phase completion (0.x = in development, 1.0 = Phase 6 complete)
- **MINOR**: Feature additions within phase
- **PATCH**: Bug fixes and minor tweaks

---

**Copyright (c) 2026 Robot Rally Team. All Rights Reserved.**
