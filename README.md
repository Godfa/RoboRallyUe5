# RobotRally (UE5)

A digital implementation of the Robot Rally board game using Unreal Engine 5.7. Players program robots with movement cards and watch them execute commands on a grid-based game board.

## Requirements

- Unreal Engine 5.7
- Visual Studio 2022 (MSVC 14.38 or 14.44)
- Windows 10/11 SDK (22621)

## Opening the Project

1. Clone the repo to `H:\Repos\RoboRallyUe5` (or your preferred location)
2. Double-click `RobotRally.uproject` to open the project in UE5 Editor
3. The editor will compile the C++ module automatically

## Project Structure

```
Source/RobotRally/
  RobotRally.h/.cpp              Module registration
  RobotPawn.h/.cpp               Robot character (ACharacter)
  RobotMovementComponent.h/.cpp  Grid movement (interpolated)
  GridManager.h/.cpp             Game board (10x10 TMap grid)
  RobotRallyGameMode.h/.cpp      Game state machine (FSM)
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
| Checkpoint | Must be collected in order to win |

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

### Phase 1: Core Structure (C++) — IN PROGRESS
- [x] `ARobotPawn` — Header and CPP, basic structure
- [x] `URobotMovementComponent` — `MoveInGrid()` and `RotateInGrid()` with interpolation
- [x] `AGridManager` — Grid generation and tile checks
- [x] `ProcessNextRegister()` — Card execution connected to robot movement
- [ ] MovementComponent-GridManager connection (grid validation: pits, bounds, obstacles)
- [ ] Visual debug drawing for grid

### Phase 2: Logic Layer (GameMode & Cards)
- [x] `ARobotRallyGameMode` — State machine (Programming/Executing/GameOver), 5 registers
- [x] `FRobotCard` — Card type (ECardAction) and priority
- [x] Card execution system with test sequence
- [ ] `UCardDataAsset` — Data-driven approach for cards
- [ ] Card dealing and hand system (random cards to player)
- [ ] Priority-based execution order between robots

### Phase 3: Player Control
- [ ] `ARobotController` (APlayerController) — Mouse clicks and card inputs
- [ ] `HealthComponent` — Damage, lives, robot destruction and respawn
- [ ] Checkpoint collection system (flags collected in order)

### Phase 4: Field Hazards & Environment
- [ ] Conveyor belts — Move robot at end of turn based on direction
- [ ] Lasers (`BP_Hazard_Laser`) — Damage robot at end of register
- [ ] Pits — Robot is destroyed when entering pit
- [ ] Collision checks between robots (pushing)

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
