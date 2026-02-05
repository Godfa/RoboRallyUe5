# Tile Hazard System

**Created**: 2026-02-05
**Status**: 📋 Planned
**Complexity**: Medium
**Last Updated**: 2026-02-05

## Overview
Implement tile effects so that conveyors move robots, lasers deal damage, pits destroy robots, and checkpoints track progress. Currently tile types exist visually but have no gameplay effect.

## Current State Analysis

### Existing Components
- `AGridManager`: Stores tile data in TMap<FIntVector, FTileData>
- `ETileType`: Normal, Pit, ConveyorNorth/South/East/West, Laser, Checkpoint
- `FTileData`: TileType + CheckpointNumber
- `ARobotPawn`: Has GridX/GridY, ExecuteMoveCommand(), ExecuteRotateCommand()
- `URobotMovementComponent`: MoveInGrid(), RotateInGrid(), IsMoving(), IsRotating()
- `ARobotRallyGameMode`: ProcessNextRegister() executes cards sequentially

### What's Missing
- No health/damage system on robots
- No tile effect processing after robot moves
- No conveyor movement logic
- No laser firing logic
- No pit fall detection
- No checkpoint tracking
- No respawn system

## Feature Requirements

### 1. Robot Health System
**User Story**: As a player, my robot has health that decreases when taking damage.

**Features**:
- Robot starts with 10 health points
- Damage reduces health
- At 0 health, robot is destroyed
- Visual/log feedback when taking damage

### 2. Pit Hazard
**User Story**: As a player, if my robot moves onto a pit, it falls and is destroyed.

**Features**:
- Detect when robot enters pit tile
- Destroy robot (or respawn at last checkpoint)
- Log message for debugging

### 3. Conveyor Belts
**User Story**: As a player, after each register phase, conveyors move my robot one tile.

**Features**:
- After card execution, process conveyor tiles
- ConveyorNorth moves robot +Y
- ConveyorSouth moves robot -Y
- ConveyorEast moves robot +X
- ConveyorWest moves robot -X
- Chain conveyors (robot on conveyor moves to another conveyor)

### 4. Laser Hazard
**User Story**: As a player, lasers on tiles deal 1 damage to my robot each register.

**Features**:
- After card execution, process laser tiles
- Robot on laser tile takes 1 damage
- Log damage for debugging

### 5. Checkpoint System
**User Story**: As a player, I must reach checkpoints in order to win.

**Features**:
- Robot tracks last reached checkpoint number
- Checkpoints must be reached in order (1, then 2, then 3...)
- When robot reaches correct next checkpoint, update progress
- Win condition: reach all checkpoints

## Implementation Plan

### Phase 1: Robot Health System
**Complexity**: Simple

#### Task 1.1: Add health to ARobotPawn
- [ ] Add `UPROPERTY Health` (int32, default 10)
- [ ] Add `UPROPERTY MaxHealth` (int32, default 10)
- [ ] Add `TakeDamage(int32 Amount)` function
- [ ] Add `OnDeath` delegate for death events
- [ ] Add `bIsAlive` flag

**Files to modify**:
- `Source/RobotRally/RobotPawn.h`
- `Source/RobotRally/RobotPawn.cpp`

#### Task 1.2: Add checkpoint progress
- [ ] Add `UPROPERTY CurrentCheckpoint` (int32, default 0)
- [ ] Add `ReachCheckpoint(int32 Number)` function
- [ ] Add `OnCheckpointReached` delegate

**Files to modify**:
- `Source/RobotRally/RobotPawn.h`
- `Source/RobotRally/RobotPawn.cpp`

### Phase 2: Tile Effect Processing
**Complexity**: Medium

#### Task 2.1: Create tile effect processor in GameMode
- [ ] Add `ProcessTileEffects()` function
- [ ] Call after each register execution (after movement completes)
- [ ] Get robot's current tile from GridManager
- [ ] Switch on tile type and apply effect

**Files to modify**:
- `Source/RobotRally/RobotRallyGameMode.h`
- `Source/RobotRally/RobotRallyGameMode.cpp`

#### Task 2.2: Implement pit effect
- [ ] In ProcessTileEffects: if tile is Pit, call robot->TakeDamage(MaxHealth)
- [ ] Log "Robot fell into pit!"

#### Task 2.3: Implement laser effect
- [ ] In ProcessTileEffects: if tile is Laser, call robot->TakeDamage(1)
- [ ] Log "Robot hit by laser! Health: X"

#### Task 2.4: Implement checkpoint effect
- [ ] In ProcessTileEffects: if tile is Checkpoint
- [ ] Check if CheckpointNumber == robot->CurrentCheckpoint + 1
- [ ] If yes, call robot->ReachCheckpoint(CheckpointNumber)
- [ ] Log "Checkpoint X reached!"

### Phase 3: Conveyor System
**Complexity**: Medium

#### Task 3.1: Add conveyor processing
- [ ] Add `ProcessConveyors()` function in GameMode
- [ ] Call after ProcessTileEffects
- [ ] Get robot's current tile
- [ ] If conveyor, determine direction and move robot

**Files to modify**:
- `Source/RobotRally/RobotRallyGameMode.h`
- `Source/RobotRally/RobotRallyGameMode.cpp`

#### Task 3.2: Implement conveyor movement
- [ ] Map ETileType to direction delta:
  - ConveyorNorth: (0, +1)
  - ConveyorSouth: (0, -1)
  - ConveyorEast: (+1, 0)
  - ConveyorWest: (-1, 0)
- [ ] Call robot movement component to move in that direction
- [ ] Wait for movement to complete before processing next effect

#### Task 3.3: Handle conveyor chains
- [ ] After conveyor moves robot, check new tile
- [ ] If also conveyor, queue another move
- [ ] Limit chain depth to prevent infinite loops (max 10)

### Phase 4: Register Phase Integration
**Complexity**: Simple

#### Task 4.1: Update ProcessNextRegister flow
- [ ] Current: Execute card → wait → next register
- [ ] New: Execute card → wait → ProcessTileEffects → ProcessConveyors → wait → next register

**Files to modify**:
- `Source/RobotRally/RobotRallyGameMode.cpp`

#### Task 4.2: Add phase completion timer
- [ ] After tile effects and conveyors, add small delay before next register
- [ ] Allows player to see what happened

### Phase 5: Win/Lose Conditions
**Complexity**: Simple

#### Task 5.1: Implement death handling
- [ ] When robot health <= 0, set bIsAlive = false
- [ ] Stop processing registers for dead robot
- [ ] Log "Robot destroyed!"
- [ ] Transition to GameOver state

#### Task 5.2: Implement win condition
- [ ] Track total checkpoints in level (from GridManager)
- [ ] When robot reaches last checkpoint, trigger win
- [ ] Log "Victory! All checkpoints reached!"
- [ ] Transition to GameOver state with win flag

**Files to modify**:
- `Source/RobotRally/RobotRallyGameMode.h`
- `Source/RobotRally/RobotRallyGameMode.cpp`
- `Source/RobotRally/GridManager.h`
- `Source/RobotRally/GridManager.cpp`

## Technical Considerations

### Execution Order
```
1. Player presses E to start execution
2. For each register (0-4):
   a. Execute card action (move/rotate)
   b. Wait for movement to complete
   c. Process tile effects (pit, laser, checkpoint)
   d. Process conveyors (may trigger chain)
   e. Wait for conveyor movement
   f. Check win/lose conditions
   g. Small delay for visual feedback
   h. Next register
3. Return to Programming phase (or GameOver)
```

### State Machine Update
```
Programming → Executing → GameOver
                ↓
         [Per Register]
         Execute Card
              ↓
         Tile Effects
              ↓
         Conveyors
              ↓
         Check Win/Lose
```

### Risks & Mitigations

**Risk**: Conveyor infinite loop
**Mitigation**: Max chain depth of 10, log warning if exceeded

**Risk**: Multiple robots on same tile (future)
**Mitigation**: Design effects to work per-robot, will need collision system later

**Risk**: Effect timing feels wrong
**Mitigation**: Add configurable delays between phases

## Success Criteria

- [ ] Robot has health that decreases when damaged
- [ ] Robot dies when health reaches 0
- [ ] Pit tiles instantly kill robot
- [ ] Laser tiles deal 1 damage per register
- [ ] Conveyor tiles move robot in correct direction
- [ ] Conveyor chains work (max 10 depth)
- [ ] Checkpoints track progress in order
- [ ] Win condition triggers when all checkpoints reached
- [ ] All effects logged to console for debugging
- [ ] Code compiles without warnings

## Future Enhancements (Out of Scope)

- Multiple robots with turn order
- Pushing robots (collision)
- Walls blocking movement
- Rotating conveyors
- Express conveyors (move 2 tiles)
- Laser beams (not just tiles)
- Respawn system instead of instant death
- Visual effects for damage/death
- UI showing health and checkpoints
