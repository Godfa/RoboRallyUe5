# Phase 4 Testing Plan — Field Hazards & Environment

**Created**: 2026-02-08
**Status**: Draft
**Complexity**: Medium

## Overview

This document outlines the testing procedures for Phase 4 of the RobotRally project, focusing on environmental hazards including conveyor belts, lasers, pits, tile effect processing, and win/lose conditions. Phase 4 adds strategic environmental challenges that affect robot survival and movement.

## Phase 4 Components Under Test

- **Pit Tiles** — Instant robot destruction on entry
- **Laser Tiles** — 1 damage per register execution
- **Conveyor Belts** — Directional movement (N/S/E/W), chaining up to 10 deep
- **Tile Effect Processing** — Triggered after each movement (WASD or card)
- **Win Condition** — All checkpoints collected
- **Lose Condition** — Robot death (0 lives remaining)

---

## Testing Checklist

### 1. Pit Tiles

#### 1.1 Basic Pit Behavior
- [ ] **Test**: Pit tile has correct type
  - **Steps**: Query GetTileType() on pit position
  - **Expected**: Returns ETileType::Pit

- [ ] **Test**: Pit tiles visible on grid
  - **Steps**: Play in PIE, look for pit tiles
  - **Expected**: Pits have distinct visual (darker color, hole appearance)

- [ ] **Test**: Moving onto pit causes instant death
  - **Steps**: Use WASD to move robot onto pit tile
  - **Expected**: Robot dies immediately (OnDeath fires)

- [ ] **Test**: Pit deals full damage (10)
  - **Steps**: Robot at 3 damage, move onto pit
  - **Expected**: Damage jumps to 10, death occurs

- [ ] **Test**: Pit during card execution causes death
  - **Steps**: Program Move2 that ends on pit, execute
  - **Expected**: Robot dies when reaching pit

- [ ] **Test**: Pit decrements lives
  - **Steps**: Start with 3 lives, fall into pit
  - **Expected**: Lives == 2, robot respawns

- [ ] **Test**: Pit triggers respawn
  - **Steps**: Collect checkpoint, fall into pit
  - **Expected**: Robot respawns at checkpoint position

#### 1.2 Pit Edge Cases
- [ ] **Test**: Moving through pit (Move2 over pit to normal tile)
  - **Steps**: Program Move2 where tile 1 is pit
  - **Expected**: Robot dies on pit, doesn't continue to tile 2

- [ ] **Test**: Out-of-bounds treated as pit
  - **Steps**: Move robot off grid edge
  - **Expected**: Treated as pit, robot dies

- [ ] **Test**: Respawning on pit (edge case)
  - **Steps**: Set respawn position to pit tile (if possible)
  - **Expected**: Error prevented or immediate re-death handled

---

### 2. Laser Tiles

#### 2.1 Basic Laser Behavior
- [ ] **Test**: Laser tile has correct type
  - **Steps**: Query GetTileType() on laser position
  - **Expected**: Returns ETileType::Laser

- [ ] **Test**: Laser tiles visible on grid
  - **Steps**: Play in PIE, look for laser tiles
  - **Expected**: Lasers have distinct visual (red color, beam effect)

- [ ] **Test**: Standing on laser deals 1 damage per register
  - **Steps**: Move onto laser, execute 5-card sequence
  - **Expected**: 5 damage dealt total (1 per register)

- [ ] **Test**: Laser damage happens after register execution
  - **Steps**: Move onto laser with WASD, execute 1 card
  - **Expected**: Damage applied after card completes

- [ ] **Test**: Moving off laser stops damage
  - **Steps**: Stand on laser for register 1, move off in register 2
  - **Expected**: Only 1 damage from register 1, none from registers 2-5

#### 2.2 Laser Damage Accumulation
- [ ] **Test**: Laser damage accumulates with existing damage
  - **Steps**: Robot at 3 damage, stand on laser for 2 registers
  - **Expected**: Damage increases to 5 (3 + 2)

- [ ] **Test**: Laser can kill robot
  - **Steps**: Robot at 6 damage, stand on laser for 5 registers
  - **Expected**: Robot dies after register 4 (6 + 4 = 10)

- [ ] **Test**: Death from laser during execution
  - **Steps**: Robot at 9 damage, stand on laser, execute
  - **Expected**: Robot dies after register 1, remaining registers not executed

- [ ] **Test**: Laser damage shown in HUD
  - **Steps**: Stand on laser for 3 registers, check health bar
  - **Expected**: Health bar decreases visually

#### 2.3 Laser Timing
- [ ] **Test**: WASD move onto laser deals damage immediately
  - **Steps**: Use W to move onto laser
  - **Expected**: 1 damage dealt after movement completes

- [ ] **Test**: Card move onto laser deals damage after card
  - **Steps**: Program Move1 onto laser, execute
  - **Expected**: 1 damage dealt after Move1 completes

- [ ] **Test**: Multiple moves on/off laser in one turn
  - **Steps**: Move onto laser (register 1), off laser (register 2), onto laser (register 3)
  - **Expected**: 2 damage total (registers 1 and 3)

- [ ] **Test**: Laser damage NOT dealt during movement interpolation
  - **Steps**: Stand on laser, observe during smooth movement
  - **Expected**: Damage applied after interpolation completes, not continuously

#### 2.4 Laser + Health Interaction
- [ ] **Test**: Laser reduces hand size
  - **Steps**: Stand on laser for 3 registers (3 damage), enter Programming
  - **Expected**: Hand size = 6 cards (9 - 3)

- [ ] **Test**: Laser damage reset on respawn
  - **Steps**: Take laser damage, die, respawn
  - **Expected**: Damage reset to 0

---

### 3. Conveyor Belts

#### 3.1 Basic Conveyor Behavior
- [ ] **Test**: ConveyorNorth tile has correct type
  - **Steps**: Query GetTileType() on conveyor position
  - **Expected**: Returns ETileType::ConveyorNorth

- [ ] **Test**: All 4 conveyor directions exist
  - **Steps**: Check for ConveyorNorth, ConveyorSouth, ConveyorEast, ConveyorWest
  - **Expected**: All 4 types defined in ETileType enum

- [ ] **Test**: Conveyors visible on grid with direction arrows
  - **Steps**: Play in PIE, look for conveyor tiles
  - **Expected**: Conveyors show directional arrows pointing N/S/E/W

#### 3.2 Conveyor Movement Timing
- [ ] **Test**: Conveyor moves robot AFTER register completes
  - **Steps**: Stand on ConveyorNorth, execute 1 card
  - **Expected**: Card executes first, then conveyor pushes North

- [ ] **Test**: Conveyor does NOT activate during WASD movement
  - **Steps**: Use WASD to move onto conveyor
  - **Expected**: Robot moves onto conveyor, conveyor activates immediately after

- [ ] **Test**: Conveyor activates after WASD movement
  - **Steps**: Use W to move onto ConveyorNorth
  - **Expected**: Robot pushed North by conveyor after move completes

- [ ] **Test**: Conveyor activates once per register
  - **Steps**: Start on ConveyorNorth, execute Move1 (stay on conveyor)
  - **Expected**: Conveyor pushes North once, not continuously

#### 3.3 Conveyor Directions
- [ ] **Test**: ConveyorNorth pushes robot North
  - **Steps**: Move onto ConveyorNorth at (5,5)
  - **Expected**: Robot pushed to (5,6)

- [ ] **Test**: ConveyorSouth pushes robot South
  - **Steps**: Move onto ConveyorSouth at (5,5)
  - **Expected**: Robot pushed to (5,4)

- [ ] **Test**: ConveyorEast pushes robot East
  - **Steps**: Move onto ConveyorEast at (5,5)
  - **Expected**: Robot pushed to (6,5)

- [ ] **Test**: ConveyorWest pushes robot West
  - **Steps**: Move onto ConveyorWest at (5,5)
  - **Expected**: Robot pushed to (4,5)

- [ ] **Test**: Conveyor ignores robot facing direction
  - **Steps**: Robot facing South on ConveyorNorth
  - **Expected**: Still pushed North (not backward relative to robot)

#### 3.4 Conveyor Chaining
- [ ] **Test**: Conveyor chains to next conveyor
  - **Steps**: ConveyorNorth at (5,5) pushes to (5,6) which is also ConveyorNorth
  - **Expected**: Robot pushed to (5,7) by second conveyor

- [ ] **Test**: Conveyor chain of 3 tiles
  - **Steps**: Set up 3 ConveyorNorth tiles in a row
  - **Expected**: Robot pushed 3 tiles North

- [ ] **Test**: Conveyor chain changes direction
  - **Steps**: ConveyorNorth → ConveyorEast → ConveyorSouth
  - **Expected**: Robot pushed North, then East, then South

- [ ] **Test**: Conveyor chain maximum depth (10)
  - **Steps**: Set up 10 ConveyorNorth tiles in a row
  - **Expected**: Robot pushed 10 tiles North

- [ ] **Test**: Conveyor chain stops at 10 (safety limit)
  - **Steps**: Set up 15 ConveyorNorth tiles in a row
  - **Expected**: Robot pushed only 10 tiles, then stops (prevents infinite loops)

- [ ] **Test**: Conveyor chain stops at Normal tile
  - **Steps**: ConveyorNorth (3 tiles) → Normal tile
  - **Expected**: Robot pushed 3 tiles, stops on Normal

- [ ] **Test**: Conveyor chain logged in console
  - **Steps**: Add logging, create 5-tile chain
  - **Expected**: Console shows "Conveyor chain depth: 5"

#### 3.5 Conveyor Edge Cases
- [ ] **Test**: Conveyor at grid edge blocked
  - **Steps**: ConveyorNorth at (5,9) tries to push off-grid
  - **Expected**: Push blocked, robot stays at (5,9)

- [ ] **Test**: Conveyor pushes into pit
  - **Steps**: ConveyorNorth pushes robot onto pit tile
  - **Expected**: Robot dies from pit

- [ ] **Test**: Conveyor pushes onto laser
  - **Steps**: ConveyorNorth pushes robot onto laser tile
  - **Expected**: Robot takes 1 damage from laser after conveyor movement

- [ ] **Test**: Conveyor pushes onto checkpoint
  - **Steps**: ConveyorNorth pushes robot onto checkpoint 1
  - **Expected**: Checkpoint collected

- [ ] **Test**: Conveyor circular loop (ConveyorNorth → East → South → West → North)
  - **Steps**: Create 4-tile conveyor loop
  - **Expected**: Robot pushed 4 tiles around loop, stops at depth limit (if it loops back)

- [ ] **Test**: Conveyor pushes robot off their turn
  - **Steps**: Stand on conveyor, execute card
  - **Expected**: Conveyor activates after card, before next register

#### 3.6 Conveyor + Movement Interaction
- [ ] **Test**: Move onto conveyor, conveyor pushes, tile effects applied
  - **Steps**: Move onto ConveyorNorth that pushes onto laser
  - **Expected**: Conveyor activates, laser damage dealt

- [ ] **Test**: Conveyor doesn't rotate robot
  - **Steps**: Robot facing East on ConveyorNorth
  - **Expected**: Robot pushed North but still facing East

---

### 4. Tile Effect Processing

#### 4.1 Processing Timing
- [ ] **Test**: ProcessTileEffects() called after WASD move
  - **Steps**: Add logging, press W
  - **Expected**: Console shows "ProcessTileEffects" after movement

- [ ] **Test**: ProcessTileEffects() called after card execution
  - **Steps**: Add logging, execute Move1 card
  - **Expected**: Console shows "ProcessTileEffects" after card

- [ ] **Test**: ProcessTileEffects() called after each register
  - **Steps**: Execute 5-card sequence, add logging
  - **Expected**: ProcessTileEffects called 5 times (once per register)

- [ ] **Test**: ProcessTileEffects() NOT called during rotation
  - **Steps**: Use A to rotate (no position change)
  - **Expected**: ProcessTileEffects not called (or no effect since position unchanged)

#### 4.2 Effect Order
- [ ] **Test**: Effects processed in order: Pit → Laser → Conveyor → Checkpoint
  - **Steps**: Move onto tile with multiple effects (if possible)
  - **Expected**: Effects processed in defined order

- [ ] **Test**: Pit immediately stops further processing
  - **Steps**: Pit tile (if could have multiple effects)
  - **Expected**: Death occurs, no further effects processed

- [ ] **Test**: Conveyor triggers tile effects on destination
  - **Steps**: Stand on ConveyorNorth that pushes onto laser
  - **Expected**: Laser effect processed after conveyor movement

#### 4.3 Chain Processing
- [ ] **Test**: Conveyor chain processes effects at final tile only
  - **Steps**: 3-tile conveyor chain ending on laser
  - **Expected**: Only final laser tile deals damage (not intermediate tiles)

- [ ] **Test**: Conveyor chain stops at pit
  - **Steps**: ConveyorNorth (2 tiles) → Pit
  - **Expected**: Robot pushed 2 tiles, dies on pit, chain stops

---

### 5. Win Condition

#### 5.1 Win Trigger
- [ ] **Test**: Collecting all checkpoints triggers win
  - **Steps**: Collect all checkpoints sequentially
  - **Expected**: CurrentGameState == GameOver (win)

- [ ] **Test**: Win message shown in HUD
  - **Steps**: Collect all checkpoints
  - **Expected**: HUD displays "Victory!" or win message

- [ ] **Test**: Win freezes game state
  - **Steps**: Win game, try to move robot
  - **Expected**: All input ignored

- [ ] **Test**: Win with 1 life remaining
  - **Steps**: Die twice (1 life left), collect all checkpoints
  - **Expected**: Win condition triggers despite low lives

#### 5.2 Win Condition Edge Cases
- [ ] **Test**: Checkpoint collected via conveyor triggers win
  - **Steps**: Use conveyor to push onto final checkpoint
  - **Expected**: Win condition triggers

- [ ] **Test**: Collecting last checkpoint while damaged
  - **Steps**: At 9 damage, collect last checkpoint
  - **Expected**: Win occurs, robot doesn't die

- [ ] **Test**: Cannot continue after winning
  - **Steps**: Win, press E to execute cards
  - **Expected**: Input ignored

---

### 6. Lose Condition

#### 6.1 Lose Trigger
- [ ] **Test**: Running out of lives triggers lose
  - **Steps**: Die 3 times (use all lives)
  - **Expected**: CurrentGameState == GameOver (lose)

- [ ] **Test**: Lose message shown in HUD
  - **Steps**: Use all lives
  - **Expected**: HUD displays "Game Over" or lose message

- [ ] **Test**: Lose freezes game state
  - **Steps**: Lose game, try to move robot
  - **Expected**: All input ignored

- [ ] **Test**: Lose prevents respawn
  - **Steps**: Die with 0 lives
  - **Expected**: No respawn, game ends

#### 6.2 Lose Condition Variations
- [ ] **Test**: Lose from pit (instant death)
  - **Steps**: 1 life remaining, fall into pit
  - **Expected**: Lose condition triggers

- [ ] **Test**: Lose from laser (accumulated damage)
  - **Steps**: 1 life remaining, 6 damage, stand on laser for 5 registers
  - **Expected**: Lose condition triggers during execution

- [ ] **Test**: Lose during card execution
  - **Steps**: 1 life, program Move3 into pit, execute
  - **Expected**: Robot dies mid-execution, lose condition triggers

#### 6.3 Win vs Lose Priority
- [ ] **Test**: Collecting last checkpoint with 0 lives (edge case)
  - **Steps**: Artificially set 0 lives, collect last checkpoint
  - **Expected**: Define behavior (win takes priority? or lose?)

- [ ] **Test**: Die on last checkpoint (simultaneous)
  - **Steps**: 1 life, move onto last checkpoint + pit (if possible)
  - **Expected**: Define behavior (checkpoint before pit? or pit first?)

---

### 7. Combined Hazard Scenarios

#### 7.1 Multi-Hazard Sequences
- [ ] **Test**: Conveyor → Laser → Checkpoint
  - **Steps**: Create sequence, move onto start
  - **Expected**: Pushed by conveyor, take laser damage, collect checkpoint

- [ ] **Test**: Conveyor → Conveyor → Pit
  - **Steps**: 2-tile conveyor chain ending in pit
  - **Expected**: Robot pushed 2 tiles, dies on pit

- [ ] **Test**: Laser → Conveyor → Laser
  - **Steps**: Stand on laser, conveyor pushes to another laser
  - **Expected**: Damage from first laser, pushed, damage from second laser

- [ ] **Test**: Move3 through mixed hazards
  - **Steps**: Program Move3 through laser → conveyor → normal
  - **Expected**: All effects processed correctly

#### 7.2 Complex Chaining
- [ ] **Test**: Conveyor chain pushes onto another conveyor chain
  - **Steps**: 3-tile North chain → 3-tile East chain
  - **Expected**: Robot pushed 6 tiles total (3 North, 3 East), max 10 enforced

- [ ] **Test**: Circular conveyor loop with depth limit
  - **Steps**: 4-tile loop (N→E→S→W→N)
  - **Expected**: Robot circles 2.5 times, stops at depth 10

- [ ] **Test**: Conveyor pushes onto checkpoint → conveyor
  - **Steps**: ConveyorNorth → Checkpoint (ConveyorEast) → Normal
  - **Expected**: Checkpoint collected, conveyor continues pushing East

#### 7.3 Hazard + Lives Interaction
- [ ] **Test**: Multiple hazard deaths deplete lives
  - **Steps**: Die on pit (life 1), die on laser (life 2), die on pit (life 3)
  - **Expected**: All 3 death types work, game over after 3rd

- [ ] **Test**: Respawn onto conveyor
  - **Steps**: Set checkpoint on ConveyorNorth, die, respawn
  - **Expected**: Robot respawns, conveyor immediately activates (or waits for next move)

---

### 8. Edge Cases & Error Handling

#### 8.1 Boundary Cases
- [ ] **Test**: Conveyor at every edge position
  - **Steps**: Place ConveyorNorth at (5,9), (0,9), (9,9)
  - **Expected**: Push blocked at boundary, robot doesn't fall off

- [ ] **Test**: Laser at grid corners
  - **Steps**: Place laser at (0,0), (9,9)
  - **Expected**: Functions normally

#### 8.2 Invalid Tile Combinations
- [ ] **Test**: Tile with both Pit and Laser (if possible)
  - **Steps**: Set tile type to both (or prioritize one)
  - **Expected**: Pit takes priority (instant death), or error prevented

- [ ] **Test**: Checkpoint on pit tile
  - **Steps**: Set tile as both Checkpoint and Pit
  - **Expected**: Define behavior (checkpoint before pit? or prevent combination?)

#### 8.3 Conveyor Edge Cases
- [ ] **Test**: Conveyor depth exactly 10 (no warning)
  - **Steps**: Create 10-tile chain
  - **Expected**: Works correctly, no error

- [ ] **Test**: Conveyor depth 11+ (warning logged)
  - **Steps**: Create 12-tile chain
  - **Expected**: Stops at 10, warning logged

- [ ] **Test**: Conveyor self-loop (ConveyorNorth on same tile)
  - **Steps**: Create ConveyorNorth at (5,5) pushing to (5,6) which is ConveyorSouth pushing back to (5,5)
  - **Expected**: Stops at depth 10, doesn't infinite loop

#### 8.4 Death During Execution
- [ ] **Test**: Die on register 3 of 5
  - **Steps**: Program 5 cards, die on pit in register 3
  - **Expected**: Registers 4-5 not executed, state transitions correctly

- [ ] **Test**: Death from laser mid-sequence
  - **Steps**: 8 damage, stand on laser, execute 5 cards
  - **Expected**: Die after register 2, remaining registers skipped

---

## Testing Procedures

### Manual Testing Steps

1. **Setup**: Open RobotRally.uproject in UE5 Editor
2. **Open Level**: Content/RobotRally/Maps/MainMap
3. **Play in Editor (PIE)**: Press Play button or Alt+P
4. **Open Output Log**: Window → Developer Tools → Output Log

### Testing Workflow

#### Test Pits
1. Move robot onto pit tile with WASD
2. Verify instant death (OnDeath fires)
3. Verify lives decremented
4. Verify respawn occurs

#### Test Lasers
1. Move onto laser tile
2. Execute 5-card sequence
3. Check damage after each register (should increase by 1)
4. Verify HUD health bar decreases
5. Stand on laser until death (10 damage)

#### Test Conveyors
1. Move onto ConveyorNorth
2. Verify robot pushed North after movement
3. Create 3-tile conveyor chain
4. Verify robot pushed 3 tiles
5. Create conveyor → pit sequence
6. Verify death occurs

#### Test Win/Lose
1. Collect all checkpoints → verify win message
2. Die 3 times → verify lose message
3. Try to move after win/lose → verify input blocked

### Debug Output Examples

```cpp
// In AGridManager::ProcessTileEffects()
UE_LOG(LogTemp, Warning, TEXT("ProcessTileEffects at %s: Type=%s"),
    *GridPosition.ToString(), *GetTileTypeName(TileType));

// In ARobotPawn::OnConveyorMove()
UE_LOG(LogTemp, Warning, TEXT("Conveyor push: %s → %s (Depth=%d)"),
    *CurrentPos.ToString(), *NewPos.ToString(), ConveyorChainDepth);

// In ARobotRallyGameMode::CheckWinCondition()
UE_LOG(LogTemp, Warning, TEXT("Checkpoints: %d/%d (Win=%s)"),
    CurrentCheckpoint, TotalCheckpoints, HasWon ? TEXT("YES") : TEXT("NO"));
```

### Visual Testing Checklist

- [ ] Pit tiles have distinct visual (dark/hole appearance)
- [ ] Laser tiles have distinct visual (red/beam effect)
- [ ] Conveyor tiles show directional arrows (N/S/E/W)
- [ ] Conveyor arrows visible and correctly oriented
- [ ] Health bar decreases smoothly with damage
- [ ] Win/lose messages clearly visible

---

## Success Criteria

Phase 4 testing is considered **COMPLETE** when:

- ✅ All 140+ test cases above pass
- ✅ Pits cause instant death and respawn
- ✅ Lasers deal 1 damage per register (timing correct)
- ✅ Conveyors push robot in correct direction (N/S/E/W)
- ✅ Conveyor chaining works up to depth 10
- ✅ Conveyor chains stop at 10 (safety limit enforced)
- ✅ Tile effects processed after every movement (WASD and cards)
- ✅ Conveyor → Pit, Conveyor → Laser, Conveyor → Checkpoint combos work
- ✅ Win condition triggers on all checkpoints collected
- ✅ Lose condition triggers on 0 lives
- ✅ Death during execution handled correctly (remaining registers skipped)
- ✅ No crashes or errors in Output Log during testing
- ✅ Visual feedback clear (pit/laser/conveyor appearance)

---

## Known Issues & Limitations

- **Robot-Robot Collision** (Phase 4 incomplete item): Collision checks between robots not implemented
  - **Current**: Single robot gameplay only
  - **Future**: Multi-robot support with pushing mechanics

---

## Next Steps After Phase 4 Testing

1. Fix any bugs found during testing
2. Tune conveyor chain depth limit if needed (currently 10)
3. Proceed to Phase 5 testing (UI & Interface - WBP widgets)
4. Future: Implement robot-robot collision for multiplayer

---

## References

- [README.md](../../README.md) — Phase 4 implementation checklist
- [CLAUDE.md](../../CLAUDE.md) — Architecture and conventions
- [Phase 1 Testing Plan](./2026-02-08_phase1-testing-plan.md) — Movement system tests
- [Phase 2 Testing Plan](./2026-02-08_phase2-testing-plan.md) — Card system tests
- [Phase 3 Testing Plan](./2026-02-08_phase3-testing-plan.md) — Player control tests
- Source Files:
  - `Source/RobotRally/GridManager.h/.cpp` — Tile types, ProcessTileEffects()
  - `Source/RobotRally/RobotPawn.h/.cpp` — Damage, death, conveyor response
  - `Source/RobotRally/RobotRallyGameMode.h/.cpp` — Win/lose conditions, state machine
  - `Source/RobotRally/RobotMovementComponent.h/.cpp` — MoveToWorldPosition() for conveyors
