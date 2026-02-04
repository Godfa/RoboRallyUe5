# CLAUDE.md - Project Intelligence

## Project Overview
RobotRally is an Unreal Engine 5.7 C++ project implementing a digital Robot Rally board game.
Players program robots with movement cards, then watch them execute on a grid-based board.

## Build & Run
- **Engine**: Unreal Engine 5.7 (Win64, Development)
- **IDE**: Visual Studio 2022 Professional
- **Build**: Open `RobotRally.uproject` in UE5 Editor, or build via VS solution `RobotRally.sln`
- **Build command**: `T:\EpicGames\UE_5.7\Engine\Build\BatchFiles\Build.bat RobotRallyEditor Win64 Development -Project="H:\Repos\RoboRallyUe5\RobotRally.uproject"`
- **Module dependencies**: Core, CoreUObject, Engine, InputCore, EnhancedInput
- **Target settings**: BuildSettingsVersion.V6, IncludeOrderVersion.Latest

## Architecture

### C++ Classes (Source/RobotRally/)
- **ARobotPawn** (ACharacter) — Robot actor with grid coordinates and movement component
- **URobotMovementComponent** (UActorComponent) — Smooth grid-based movement via interpolation
- **AGridManager** (AActor) — 10x10 TMap<FIntVector, FTileData> grid with tile types
- **ARobotRallyGameMode** (AGameModeBase) — FSM: Programming -> Executing -> GameOver, 5 registers

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
- GridToWorld/WorldToGrid handle coordinate conversion relative to GridManager actor location

### Movement System
- URobotMovementComponent interpolates toward TargetLocation/TargetRotation each tick
- MoveInGrid(Distance) adds ForwardVector * Distance * GridSize to target
- RotateInGrid(Steps) adds Steps * 90 degrees to target yaw
- Movement speed: 5.0f (position), 10.0f (rotation)

## Code Conventions
- Unreal naming: A-prefix for actors, U-prefix for components, F-prefix for structs, E-prefix for enums
- UPROPERTY/UFUNCTION macros with Category tags for Blueprint exposure
- Copyright header: `// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.`
- Tabs for indentation (tab size 4)
- Opening brace on same line for control flow, next line for functions/classes

## Known Issues & Current State
- Project is in early development (Phase 1)
- GridX/GridY on ARobotPawn are not synced when RobotMovementComponent moves
- ProcessNextRegister() has no implementation connecting cards to robot actions
- RobotMovementComponent does not validate moves against GridManager
- No PlayerController, UI, health system, or hazard logic yet

## Language
- Project documentation and commit messages: Finnish or English
- Code comments and identifiers: English
