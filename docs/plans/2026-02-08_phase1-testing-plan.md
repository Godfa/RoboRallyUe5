# Phase 1 Testing Plan — Core Structure (C++)

**Created**: 2026-02-08
**Status**: Draft
**Complexity**: Medium

## Overview

This document outlines the testing procedures for Phase 1 of the RobotRally project, focusing on the core C++ classes and grid-based movement system. Phase 1 establishes the foundational architecture that all subsequent phases depend on.

## Phase 1 Components Under Test

- **ARobotPawn** — Robot character actor with grid position tracking
- **URobotMovementComponent** — Smooth grid-based movement with interpolation
- **AGridManager** — 10x10 tile-based game board with coordinate conversion
- **Card Execution** — ProcessNextRegister() integration with movement
- **Grid Validation** — Bounds checking and tile type queries

---

## Testing Checklist

### 1. Grid System (AGridManager)

#### 1.1 Grid Generation
- [ ] **Test**: GridManager spawns correctly in BeginPlay()
  - **Steps**: Play in Editor (PIE), check World Outliner for AGridManager instance
  - **Expected**: Single GridManager actor present

- [ ] **Test**: Grid dimensions are 10x10
  - **Steps**: Add log output for grid size in GenerateGrid()
  - **Expected**: Output shows 100 tiles generated (0-9 on both axes)

- [ ] **Test**: All tiles have default type (Normal)
  - **Steps**: Query each grid position with GetTileType()
  - **Expected**: All return ETileType::Normal initially

#### 1.2 Coordinate Conversion
- [ ] **Test**: GridToWorld() produces correct world positions
  - **Steps**: Convert (0,0) → should be (0, 0, 0) world units
  - **Steps**: Convert (5,5) → should be (500, 500, 0) world units
  - **Expected**: Formula: WorldPos = GridPos * 100.0

- [ ] **Test**: WorldToGrid() produces correct grid positions
  - **Steps**: Convert world (250, 250, 0) → should be grid (2, 2, 0)
  - **Steps**: Convert world (999, 999, 0) → should be grid (9, 9, 0)
  - **Expected**: Formula: GridPos = Floor(WorldPos / 100.0)

- [ ] **Test**: Round-trip conversion is consistent
  - **Steps**: Grid(3,7) → World → Grid
  - **Expected**: Final result is (3,7,0)

#### 1.3 Tile Queries
- [ ] **Test**: GetTileType() returns correct types
  - **Steps**: Set (5,5) to ETileType::Pit, query it
  - **Expected**: Returns Pit for (5,5), Normal for others

- [ ] **Test**: Out-of-bounds queries return Pit
  - **Steps**: Query positions (-1,0), (10,10), (5,15)
  - **Expected**: All return ETileType::Pit

- [ ] **Test**: GetCheckpointNumber() returns correct values
  - **Steps**: Set (3,3) as Checkpoint with Number=2
  - **Expected**: GetCheckpointNumber(3,3) returns 2

---

### 2. Robot Pawn (ARobotPawn)

#### 2.1 Initialization
- [ ] **Test**: RobotPawn spawns at correct grid position
  - **Steps**: Spawn robot at grid (5,5) via GameMode
  - **Expected**: Actor world location is (500, 500, 0)

- [ ] **Test**: Initial facing direction is North (0°)
  - **Steps**: Check GetActorRotation().Yaw
  - **Expected**: Yaw = 0 or 360 (facing North in UE5 coordinates)

- [ ] **Test**: GridPosition field tracks current position
  - **Steps**: Spawn at (5,5), check GridPosition variable
  - **Expected**: GridPosition = (5, 5, 0)

#### 2.2 Grid Position Tracking
- [ ] **Test**: GridPosition updates after movement
  - **Steps**: Move robot 1 tile North, check GridPosition
  - **Expected**: GridPosition changes from (5,5) to (5,6)

- [ ] **Test**: GridPosition updates after rotation (no position change)
  - **Steps**: Rotate robot 90°, check GridPosition
  - **Expected**: GridPosition remains unchanged

---

### 3. Movement Component (URobotMovementComponent)

#### 3.1 Grid Movement (MoveInGrid)
- [ ] **Test**: MoveInGrid(1) moves robot 1 tile forward
  - **Steps**: Robot at (5,5) facing North, call MoveInGrid(1)
  - **Expected**: Robot moves to (5,6)

- [ ] **Test**: MoveInGrid(2) moves robot 2 tiles forward
  - **Steps**: Robot at (5,5) facing East, call MoveInGrid(2)
  - **Expected**: Robot moves to (7,5)

- [ ] **Test**: MoveInGrid(-1) moves robot 1 tile backward
  - **Steps**: Robot at (5,5) facing North, call MoveInGrid(-1)
  - **Expected**: Robot moves to (5,4)

- [ ] **Test**: Movement respects facing direction
  - **Steps**: Test North/South/East/West facings with MoveInGrid(1)
  - **Expected**: Movement direction matches robot's facing

#### 3.2 Grid Rotation (RotateInGrid)
- [ ] **Test**: RotateInGrid(1) rotates 90° clockwise
  - **Steps**: Robot facing North, call RotateInGrid(1)
  - **Expected**: Robot faces East (Yaw = 90°)

- [ ] **Test**: RotateInGrid(-1) rotates 90° counter-clockwise
  - **Steps**: Robot facing North, call RotateInGrid(-1)
  - **Expected**: Robot faces West (Yaw = 270°)

- [ ] **Test**: RotateInGrid(2) rotates 180° (U-Turn)
  - **Steps**: Robot facing North, call RotateInGrid(2)
  - **Expected**: Robot faces South (Yaw = 180°)

- [ ] **Test**: Rotation wraps around correctly
  - **Steps**: Robot facing North, call RotateInGrid(5) (450°)
  - **Expected**: Robot faces East (450° mod 360° = 90°)

#### 3.3 Interpolation
- [ ] **Test**: Movement is smooth (not instant)
  - **Steps**: Call MoveInGrid(1), observe movement over time
  - **Expected**: Robot smoothly interpolates from start to target (use VInterpTo)

- [ ] **Test**: Rotation is smooth (not instant)
  - **Steps**: Call RotateInGrid(1), observe rotation over time
  - **Expected**: Robot smoothly interpolates rotation (use RInterpTo)

- [ ] **Test**: Interpolation speed is reasonable
  - **Steps**: Time how long a 1-tile move takes
  - **Expected**: Movement completes in ~0.5-1.0 seconds (adjust InterpSpeed if needed)

- [ ] **Test**: Multiple movements queue correctly
  - **Steps**: Call MoveInGrid(1) twice in quick succession
  - **Expected**: Robot completes first move, then starts second

#### 3.4 Bounds Validation
- [ ] **Test**: Movement blocked at grid edge (North)
  - **Steps**: Robot at (5,9), call MoveInGrid(1) facing North
  - **Expected**: Movement blocked, robot stays at (5,9)

- [ ] **Test**: Movement blocked at grid edge (South)
  - **Steps**: Robot at (5,0), call MoveInGrid(1) facing South
  - **Expected**: Movement blocked, robot stays at (5,0)

- [ ] **Test**: Movement blocked at grid edge (East)
  - **Steps**: Robot at (9,5), call MoveInGrid(1) facing East
  - **Expected**: Movement blocked, robot stays at (9,5)

- [ ] **Test**: Movement blocked at grid edge (West)
  - **Steps**: Robot at (0,5), call MoveInGrid(1) facing West
  - **Expected**: Movement blocked, robot stays at (0,5)

- [ ] **Test**: Backward movement blocked at grid edge
  - **Steps**: Robot at (5,0) facing North, call MoveInGrid(-1)
  - **Expected**: Movement blocked, robot stays at (5,0)

---

### 4. Card Execution Integration

#### 4.1 ProcessNextRegister() Connection
- [ ] **Test**: Card Move1 calls MoveInGrid(1)
  - **Steps**: Execute card with ECardAction::Move1
  - **Expected**: Robot moves 1 tile forward

- [ ] **Test**: Card Move2 calls MoveInGrid(2)
  - **Steps**: Execute card with ECardAction::Move2
  - **Expected**: Robot moves 2 tiles forward

- [ ] **Test**: Card Move3 calls MoveInGrid(3)
  - **Steps**: Execute card with ECardAction::Move3
  - **Expected**: Robot moves 3 tiles forward

- [ ] **Test**: Card MoveBack calls MoveInGrid(-1)
  - **Steps**: Execute card with ECardAction::MoveBack
  - **Expected**: Robot moves 1 tile backward

- [ ] **Test**: Card RotateRight calls RotateInGrid(1)
  - **Steps**: Execute card with ECardAction::RotateRight
  - **Expected**: Robot rotates 90° clockwise

- [ ] **Test**: Card RotateLeft calls RotateInGrid(-1)
  - **Steps**: Execute card with ECardAction::RotateLeft
  - **Expected**: Robot rotates 90° counter-clockwise

- [ ] **Test**: Card UTurn calls RotateInGrid(2)
  - **Steps**: Execute card with ECardAction::UTurn
  - **Expected**: Robot rotates 180°

#### 4.2 Multi-Register Sequence
- [ ] **Test**: Execute 5-card sequence correctly
  - **Steps**: Program registers with [Move1, RotateRight, Move2, RotateLeft, Move1]
  - **Expected**: Robot executes all 5 cards in order

- [ ] **Test**: Registers execute in correct order (1→2→3→4→5)
  - **Steps**: Log each register execution, verify order
  - **Expected**: Console shows "Register 1", "Register 2", etc. in sequence

---

### 5. Grid Validation & Edge Cases

#### 5.1 Grid Manager Integration
- [ ] **Test**: MovementComponent queries GridManager for validation
  - **Steps**: Add log to MoveInGrid() showing IsValidGridPosition() result
  - **Expected**: Log shows validation check before movement

- [ ] **Test**: Invalid moves are rejected by GridManager
  - **Steps**: Attempt to move to (-1, 5)
  - **Expected**: GridManager returns false, movement blocked

#### 5.2 Edge Cases
- [ ] **Test**: Robot spawned outside grid is handled
  - **Steps**: Spawn robot at world position (-500, -500, 0)
  - **Expected**: Robot either clamped to grid or destroyed (define expected behavior)

- [ ] **Test**: Rapid successive movements don't break interpolation
  - **Steps**: Call MoveInGrid(1) 5 times rapidly
  - **Expected**: Robot queues movements or rejects new ones until current completes

- [ ] **Test**: Zero-distance movement is handled
  - **Steps**: Call MoveInGrid(0)
  - **Expected**: No movement occurs, no errors

---

## Testing Procedures

### Manual Testing Steps

1. **Setup**: Open RobotRally.uproject in UE5 Editor
2. **Open Level**: Content/RobotRally/Maps/MainMap
3. **Play in Editor (PIE)**: Press Play button or Alt+P
4. **Open Output Log**: Window → Developer Tools → Output Log
5. **Test Movement**: Use WASD keys to test grid movement
6. **Test Cards**: Press E to execute programmed card sequence
7. **Verify Visual**: Watch robot movement on screen
8. **Verify Console**: Check Output Log for validation messages

### Adding Debug Output

Add temporary logging to key functions for testing:

```cpp
// In URobotMovementComponent::MoveInGrid()
UE_LOG(LogTemp, Warning, TEXT("MoveInGrid(%d): %s → %s"),
    Distance, *CurrentGridPos.ToString(), *TargetGridPos.ToString());

// In AGridManager::IsValidGridPosition()
UE_LOG(LogTemp, Warning, TEXT("ValidatePos(%s): %s"),
    *GridPos.ToString(), IsValid ? TEXT("VALID") : TEXT("INVALID"));
```

### Automated Testing (Future Enhancement)

Consider adding C++ unit tests using Unreal's Automation Framework:
- `GridManager.spec.cpp` — Coordinate conversion tests
- `MovementComponent.spec.cpp` — Movement validation tests

---

## Success Criteria

Phase 1 testing is considered **COMPLETE** when:

- ✅ All 60+ test cases above pass
- ✅ Robot movement is smooth and visually correct
- ✅ Grid boundaries are respected
- ✅ Coordinate conversions are accurate
- ✅ Card execution triggers correct movement functions
- ✅ No crashes or errors in Output Log during testing
- ✅ Code compiles without warnings

---

## Known Issues & Limitations

- **Visual Debug Drawing** (Phase 1 incomplete item): Grid lines not visible in viewport
  - **Workaround**: Use tile meshes to visualize grid
  - **Future**: Add DrawDebugLine() calls in GridManager

---

## Next Steps After Phase 1 Testing

1. Fix any bugs found during testing
2. Add visual debug drawing for grid (optional Phase 1 item)
3. Proceed to Phase 2 testing (GameMode state machine and card logic)

---

## References

- [README.md](../../README.md) — Phase 1 implementation checklist
- [CLAUDE.md](../../CLAUDE.md) — Architecture and conventions
- Source Files:
  - `Source/RobotRally/RobotPawn.h/.cpp`
  - `Source/RobotRally/RobotMovementComponent.h/.cpp`
  - `Source/RobotRally/GridManager.h/.cpp`
  - `Source/RobotRally/RobotRallyGameMode.h/.cpp`
