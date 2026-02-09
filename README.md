# RobotRally (UE5)

A digital implementation of the Robot Rally board game using Unreal Engine 5.7. Players program robots with movement cards and watch them execute commands on a grid-based game board.

**Status**: Phase 5 Complete (Multiplayer & Walls) | [Technical Docs](docs/TECHNICAL_DOCUMENTATION.md) | [User Guide](docs/USER_GUIDE.md) | [Changelog](CHANGELOG.md)

## Requirements

- Unreal Engine 5.7
- Visual Studio 2022 (MSVC 14.38 or 14.44)
- Windows 10/11 SDK (22621)

## Quick Start

1. Clone the repo to `H:\Repos\RoboRallyUe5` (or your preferred location)
2. Double-click `RobotRally.uproject` to open the project in UE5 Editor
3. The editor will compile the C++ module automatically
4. Press **Play (Alt+P)** to start the game
5. Use **1-9 keys** to select 5 cards, press **E** to execute
6. See [User Guide](docs/USER_GUIDE.md) for detailed controls

## Documentation

- **[Technical Documentation](docs/TECHNICAL_DOCUMENTATION.md)** - Architecture, class reference, API docs
- **[User Guide](docs/USER_GUIDE.md)** - Game rules, controls, tips & strategies
- **[CLAUDE.md](CLAUDE.md)** - AI assistant project instructions

## Project Structure

```
Source/RobotRally/
  RobotRally.h/.cpp              Module registration
  RobotPawn.h/.cpp               Robot character (ACharacter)
  RobotController.h/.cpp         Player input handling (WASD, cards)
  RobotAIController.h/.cpp       AI robot controller (Easy/Medium)
  RobotMovementComponent.h/.cpp  Grid movement with collision & walls
  GridManager.h/.cpp             Game board with wall system
  RobotRallyGameMode.h/.cpp      Game state machine, card deck
  RobotRallyGameState.h/.cpp     Network replicated game state
  RobotRallyPlayerState.h/.cpp   Per-player replicated data
  RobotRallyGameInstance.h/.cpp  Session management (LAN host/join)
  RobotRallyHUD.h/.cpp           On-screen HUD (health, events, cards)
```

## Game Mechanics

### Game States
1. **Programming** — Player selects 5 movement cards into registers
2. **Executing** — Robots execute commands register by register
3. **GameOver** — Game ends

### Card Types
| Card | Action |
|---|---|
| Move1 / Move2 / Move3 | Move 1-3 tiles forward |
| MoveBack | Move 1 tile backward |
| RotateRight | Turn 90° right |
| RotateLeft | Turn 90° left |
| UTurn | Turn 180° |

### Tile Types
| Type | Effect |
|---|---|
| Normal | No effect |
| Pit | Robot is destroyed |
| Conveyor (N/S/E/W) | Moves robot at end of turn |
| Laser | Damages robot |
| Checkpoint | Must be collected in sequential order to win (1→2→3...) |
| **Walls** | Blocks movement between tiles (on tile edges N/S/E/W) |

## Architecture

```
ARobotRallyGameMode          Game state machine, turn logic
  |
  +-- ARobotPawn             Robot character on grid
  |     +-- URobotMovementComponent  Interpolated grid movement
  |
  +-- AGridManager           Game board, tile data, coordinate conversions
```

### Grid System
- `TMap<FIntVector, FTileData>` — key (x, y, 0), value contains type and checkpoint number
- `GridToWorld()` / `WorldToGrid()` — conversions between grid and world coordinates
- Tile size: 100 UE units
- Outside board = pit

### Movement System
- `MoveInGrid(Distance)` — sets target point in forward vector direction
- `RotateInGrid(Steps)` — rotates target rotation in 90° steps
- Tick function smoothly interpolates to target (`VInterpTo` / `RInterpTo`)

## Implementation Plan

### Phase 0: Project Creation & Structure — COMPLETE
- [x] UE5 project created named RobotRally
- [x] Folder structure per standards
- [x] Plugin settings (Enhanced Input)

### Phase 1: Core Structure (C++) — COMPLETE
- [x] `ARobotPawn` — Header and CPP, basic structure
- [x] `URobotMovementComponent` — `MoveInGrid()` and `RotateInGrid()` with interpolation
- [x] `AGridManager` — Grid generation and tile checks
- [x] `ProcessNextRegister()` — Card execution connected to robot movement
- [x] MovementComponent-GridManager connection (grid validation: bounds check)
- [ ] Visual debug drawing for grid

### Phase 2: Logic Layer (GameMode & Cards)
- [x] `ARobotRallyGameMode` — State machine (Programming/Executing/GameOver), 5 registers
- [x] `FRobotCard` — Card type (ECardAction) and priority
- [x] Card execution system with test sequence
- [x] Card dealing and hand system (84-card deck, Fisher-Yates shuffle, hand size based on damage)
- [x] Card selection (1-9 keys) and register programming
- [ ] `UCardDataAsset` — Data-driven approach for cards (optional enhancement)
- [ ] Priority-based execution order between robots (requires multi-robot support)

### Phase 3: Player Control — COMPLETE
- [x] `ARobotController` (APlayerController) — Input handling for WASD movement and card selection
- [x] Health system — Damage, death via `ApplyDamage()`, `OnDeath` delegate
- [x] Checkpoint collection system (flags collected in order)
- [x] Robot lives and respawn (3 lives, respawn at last checkpoint)

### Phase 4: Field Hazards & Environment — COMPLETE
- [x] Conveyor belts — Move robot at end of register, chain support (max 10)
- [x] Lasers — 1 damage per register
- [x] Pits — Robot destroyed when entering pit
- [x] Tile effect processing after each card execution
- [x] Win condition (all checkpoints) and lose condition (robot death)
- [x] Checkpoint visual numbering (3D text labels visible from top-down view)
- [x] Collision checks between robots (pushing)

### Phase 5: Multiplayer & Wall System — COMPLETE
- [x] Wall system on tile edges (N/S/E/W bitfield storage)
- [x] Wall visual rendering (dark gray meshes at tile boundaries)
- [x] Movement validation against walls (robots, pushing, conveyors)
- [x] Network architecture (GameState, PlayerState, GameInstance)
- [x] LAN multiplayer (2-8 players using OnlineSubsystem Null)
- [x] Server-authoritative movement and tile effects
- [x] RPC system for card selection and program commitment
- [x] Robot collision detection and chain pushing mechanics
- [x] AI controller framework (Easy/Medium difficulties)
- [x] Priority-based execution order for multiple robots

### Phase 5: UI & Interface
- [ ] `WBP_ProgrammingDeck` — Card selection view (9 cards in hand, 5 to registers)
- [ ] `WBP_CardSlot` — Single card UI representation
- [ ] `WBP_HUD` — Health, lives, current game phase, checkpoint progress

### Phase 6: Content & Polish
- [ ] `/Content/RobotRally/Maps/` — Game level(s)
- [ ] `/Content/RobotRally/Blueprints/` — Blueprint child classes from C++
- [ ] `/Content/RobotRally/Data/` — DataTables and DataAssets for grids
- [ ] Sound effects and visual effects (movement, damage, checkpoint)

### Verification Plan
- [ ] Automated tests: GridManager coordinate conversions, collision checks
- [ ] Manual testing: card selection, robot movement, hazards, checkpoint collection

## License

Copyright (c) 2026 Robot Rally Team. All Rights Reserved.
