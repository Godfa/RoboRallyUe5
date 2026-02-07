# CLAUDE.md - Project Intelligence

## Project Overview
RobotRally is an Unreal Engine 5.7 C++ project implementing a digital Robot Rally board game.
Players program robots with movement cards, then watch them execute on a grid-based board.

## 📖 Key Documentation
- **[README.md](README.md)**: Project overview and phased implementation plan
- **[Feature Plans](docs/plans/)**: Implementation plans for complex features

## Assistant Roles & Collaboration Workflow

### 🎯 When to Use This Workflow

**Simple Tasks** (implement directly):
- Typo fixes, small tweaks, single-line changes
- No plan file needed

**Medium Tasks** (lightweight planning):
- New component, single feature, bug fix
- Brief analysis in chat → implement → verify build
- Optional: lightweight plan in chat

**Complex Tasks** (full workflow required):
- Multi-file features, architectural changes, new systems
- Full Architect → Developer → Reviewer cycle
- **Mandatory**: Create plan file in `docs/plans/YYYY-MM-DD_feature-name.md`

### 🏗 The Architect (Planning Role)
- Analyze feature requests and design implementation path
- Search codebase for similar patterns to reuse
- Identify files to create/modify and potential risks
- Create checklist-based plan file for complex tasks

### 🛠 The Developer (Default Role)
- Build functional, efficient code based on approved plan
- Follow existing patterns and Unreal conventions
- Always ensure code compiles before finishing
- Update plan checklist as tasks are completed

### 🔍 The Reviewer
- Audit code for quality and consistency
- Check: UE naming conventions, UPROPERTY usage, memory safety
- Verify no compilation warnings/errors
- Provide actionable feedback

## Build & Run
- **Engine**: Unreal Engine 5.7 (Win64, Development)
- **IDE**: Visual Studio 2022 Professional
- **Build**: Open `RobotRally.uproject` in UE5 Editor, or build via VS solution
- **Build command**: `cmd.exe /c "T:\EpicGames\UE_5.7\Engine\Build\BatchFiles\Build.bat RobotRallyEditor Win64 Development -Project=\"H:\Repos\RoboRallyUe5\RobotRally.uproject\""`
- **Live Coding issue**: If build fails with "Live Coding is active", press **Ctrl+Alt+F11** in editor
- **Module dependencies**: Core, CoreUObject, Engine, InputCore, EnhancedInput

## Architecture

### C++ Classes (Source/RobotRally/)
- **ARobotPawn** (ACharacter) — Robot actor with grid coordinates, health, checkpoints, customizable meshes
- **URobotMovementComponent** (UActorComponent) — Smooth grid-based movement via interpolation
- **AGridManager** (AActor) — 10x10 TMap<FIntVector, FTileData> grid with tile types
- **ARobotRallyGameMode** (AGameModeBase) — FSM: Programming -> Executing -> GameOver, 5 registers, tile effects
- **ARobotRallyHUD** (AHUD) — On-screen health bar, checkpoint counter, event log

### Key Types
- **ETileType**: Normal, Pit, ConveyorNorth/South/East/West, Laser, Checkpoint
- **ECardAction**: Move1, Move2, Move3, MoveBack, RotateRight, RotateLeft, UTurn
- **FRobotCard**: Action + Priority
- **FTileData**: TileType + CheckpointNumber
- **EGameState**: Programming, Executing, GameOver

### Grid System
- Grid uses FIntVector keys (x, y, 0) in a TMap
- TileSize = GridSize = 100.0 units
- Out-of-bounds tiles return ETileType::Pit
- GridToWorld/WorldToGrid handle coordinate conversion

### Movement System
- URobotMovementComponent interpolates toward TargetLocation/TargetRotation each tick
- MoveInGrid(Distance) moves robot forward/backward in grid units
- RotateInGrid(Steps) rotates robot (1 step = 90 degrees)
- MoveToWorldPosition(Target) for conveyor/external movement
- Press E to execute programmed cards (test sequence)

### Tile Hazard System
- After each move (card or WASD), ProcessTileEffects() checks current tile
- Pits: instant death (full damage)
- Lasers: 1 damage per register
- Checkpoints: tracked in order, win when all reached
- Conveyors: move robot 1 tile, chain up to 10 deep
- Win/Lose: GameOver state on death or all checkpoints collected

### Asset Customization
- **RobotPawn**: `BodyMeshAsset`, `DirectionMeshAsset`, `BodyColor`, `DirectionColor`
- **GridManager**: `TileMeshAsset`, `TileTypeMeshes` (per-type override)
- Import FBX from Blender, create Blueprint from C++ class, assign meshes in editor

## Code Conventions
- Unreal naming: A-prefix for actors, U-prefix for components, F-prefix for structs, E-prefix for enums
- UPROPERTY/UFUNCTION macros with Category tags for Blueprint exposure
- UPROPERTY(EditDefaultsOnly) for customizable assets with engine fallbacks
- Copyright header: `// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.`
- Tabs for indentation (tab size 4)
- Opening brace on same line for control flow, next line for functions/classes

## General Guidance
- **No Time Estimates**: Never include time estimates in plans or explanations. Use complexity labels: **Simple**, **Medium**, **Complex**
- **GameMode creates scene**: MainMap is intentionally empty - everything spawns in BeginPlay()
- **Test in PIE**: Press Play to see the game, WASD to move robot, E to execute cards

## Current State
- Card execution system (5 registers with test sequence)
- Grid visualization with colored tiles
- Robot movement with smooth interpolation
- Customizable meshes via UPROPERTY
- Tile hazard system: pits, lasers, conveyors, checkpoints
- Health system with damage and death
- Win/lose conditions (all checkpoints / robot death)
- On-screen HUD: health bar, checkpoint counter, game state, event log

## Language
- Project documentation and commit messages: Finnish or English
- Code comments and identifiers: English
