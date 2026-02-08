# Phase 3 Testing Plan — Player Control

**Created**: 2026-02-08
**Status**: Draft
**Complexity**: Medium

## Overview

This document outlines the testing procedures for Phase 3 of the RobotRally project, focusing on player control systems including input handling, health/damage mechanics, checkpoint collection, and the lives/respawn system. Phase 3 builds on Phases 1-2 to add direct player interaction and survival mechanics.

## Phase 3 Components Under Test

- **ARobotController** — Player input handling (WASD, card selection, execution)
- **Health System** — Damage tracking, ApplyDamage(), death mechanics
- **OnDeath Delegate** — Event broadcasting on robot destruction
- **Checkpoint System** — Sequential collection (1→2→3...), win condition
- **Lives System** — 3 lives per robot, life counter tracking
- **Respawn System** — Respawn at last checkpoint, position restoration

---

## Testing Checklist

### 1. Player Controller (ARobotController)

#### 1.1 Controller Setup
- [ ] **Test**: Controller assigned to robot pawn
  - **Steps**: Launch PIE, check robot's Controller property
  - **Expected**: Controller is of type ARobotController

- [ ] **Test**: Controller only controls one robot
  - **Steps**: Spawn multiple robots, check control
  - **Expected**: Player controls only their assigned robot

- [ ] **Test**: Controller persists across state changes
  - **Steps**: Change game state (Programming → Executing → Programming)
  - **Expected**: Same controller remains assigned

#### 1.2 WASD Movement Input
- [ ] **Test**: W key moves robot forward
  - **Steps**: Press W during Programming
  - **Expected**: Robot moves 1 tile North (current facing direction)

- [ ] **Test**: A key rotates robot left
  - **Steps**: Press A during Programming
  - **Expected**: Robot rotates 90° counter-clockwise

- [ ] **Test**: S key moves robot backward
  - **Steps**: Press S during Programming
  - **Expected**: Robot moves 1 tile backward (opposite facing direction)

- [ ] **Test**: D key rotates robot right
  - **Steps**: Press D during Programming
  - **Expected**: Robot rotates 90° clockwise

- [ ] **Test**: WASD respects facing direction
  - **Steps**: Rotate robot to East, press W
  - **Expected**: Robot moves East (not North)

- [ ] **Test**: WASD only works in Programming state
  - **Steps**: Press WASD during Executing state
  - **Expected**: Input ignored, robot doesn't move

- [ ] **Test**: WASD only works in Programming state (GameOver)
  - **Steps**: Press WASD during GameOver state
  - **Expected**: Input ignored

- [ ] **Test**: WASD respects grid boundaries
  - **Steps**: Move robot to edge, press W to move out-of-bounds
  - **Expected**: Movement blocked, robot stays in bounds

#### 1.3 Card Selection Input (1-9 Keys)
- [ ] **Test**: 1 key selects first card in hand
  - **Steps**: Press 1 during Programming
  - **Expected**: Card[0] moves to first empty register

- [ ] **Test**: 9 key selects ninth card in hand
  - **Steps**: Press 9 during Programming
  - **Expected**: Card[8] moves to first empty register

- [ ] **Test**: Number keys only work in Programming state
  - **Steps**: Press 1-9 during Executing
  - **Expected**: Input ignored

- [ ] **Test**: Number key beyond hand size is ignored
  - **Steps**: With 4-card hand, press 7
  - **Expected**: No action, no error

- [ ] **Test**: Number keys fill registers sequentially
  - **Steps**: Press 1, 2, 3, 4, 5
  - **Expected**: Registers 1-5 filled in order

#### 1.4 Execution Input (E Key)
- [ ] **Test**: E key starts card execution
  - **Steps**: Program 5 cards, press E
  - **Expected**: State changes to Executing, cards execute

- [ ] **Test**: E key only works in Programming state
  - **Steps**: Press E during Executing
  - **Expected**: Input ignored

- [ ] **Test**: E key requires programmed cards
  - **Steps**: Press E with empty registers
  - **Expected**: Input ignored or warning shown

#### 1.5 Undo Input (Backspace Key)
- [ ] **Test**: Backspace removes last programmed card
  - **Steps**: Program 3 cards, press Backspace
  - **Expected**: Register 3 cleared, card returns to hand

- [ ] **Test**: Backspace only works in Programming state
  - **Steps**: Press Backspace during Executing
  - **Expected**: Input ignored

- [ ] **Test**: Multiple Backspace clears all registers
  - **Steps**: Program 5 cards, press Backspace 5 times
  - **Expected**: All registers empty

- [ ] **Test**: Backspace on empty registers does nothing
  - **Steps**: Press Backspace with no cards programmed
  - **Expected**: No error, no state change

#### 1.6 Input Blocking
- [ ] **Test**: No input accepted in GameOver state
  - **Steps**: Reach GameOver, press WASD, 1-9, E, Backspace
  - **Expected**: All input ignored

- [ ] **Test**: Multiple simultaneous inputs handled correctly
  - **Steps**: Press W+A at same time
  - **Expected**: One input processed, other queued or ignored (define behavior)

---

### 2. Health System

#### 2.1 Health Initialization
- [ ] **Test**: Robot starts with 0 damage (full health)
  - **Steps**: Spawn robot, check Damage property
  - **Expected**: Damage == 0

- [ ] **Test**: Robot has maximum damage threshold (10 = death)
  - **Steps**: Check MaxDamage or death threshold constant
  - **Expected**: MaxDamage == 10 (or documented value)

#### 2.2 Damage Application
- [ ] **Test**: ApplyDamage() increases damage counter
  - **Steps**: Call ApplyDamage(3), check Damage
  - **Expected**: Damage == 3

- [ ] **Test**: Multiple ApplyDamage() calls accumulate
  - **Steps**: ApplyDamage(2), then ApplyDamage(3)
  - **Expected**: Damage == 5

- [ ] **Test**: Damage shown in HUD
  - **Steps**: Take damage, check on-screen health bar
  - **Expected**: Health bar decreases visually

- [ ] **Test**: Laser tile deals 1 damage
  - **Steps**: Move onto laser tile
  - **Expected**: Damage increases by 1

- [ ] **Test**: Damage affects hand size
  - **Steps**: Take 3 damage, enter Programming
  - **Expected**: Hand size = 6 cards (9 - 3)

#### 2.3 Death Mechanics
- [ ] **Test**: Robot dies at 10 damage
  - **Steps**: ApplyDamage(10)
  - **Expected**: OnDeath delegate fires, robot dies

- [ ] **Test**: Pit tile causes instant death
  - **Steps**: Move robot into pit
  - **Expected**: Robot dies immediately (full damage)

- [ ] **Test**: Death triggers OnDeath delegate
  - **Steps**: Add listener to OnDeath, kill robot
  - **Expected**: Delegate broadcasts event

- [ ] **Test**: Death decrements lives counter
  - **Steps**: Start with 3 lives, die once
  - **Expected**: Lives == 2

- [ ] **Test**: Death resets damage to 0 on respawn
  - **Steps**: Die with 10 damage, respawn
  - **Expected**: Damage == 0 after respawn

#### 2.4 Healing (if implemented)
- [ ] **Test**: Healing reduces damage
  - **Steps**: Take 5 damage, heal 2
  - **Expected**: Damage == 3

- [ ] **Test**: Healing cannot reduce damage below 0
  - **Steps**: At 0 damage, heal 5
  - **Expected**: Damage remains 0

---

### 3. Checkpoint System

#### 3.1 Checkpoint Setup
- [ ] **Test**: Checkpoints have sequential numbers
  - **Steps**: Check grid tiles marked as Checkpoint
  - **Expected**: Numbers 1, 2, 3, etc. (no duplicates)

- [ ] **Test**: Checkpoints visible on grid
  - **Steps**: Play in PIE, look for checkpoint markers
  - **Expected**: Checkpoints have distinct visual (color, label)

- [ ] **Test**: Checkpoint tiles return correct type
  - **Steps**: Query GetTileType() on checkpoint position
  - **Expected**: Returns ETileType::Checkpoint

#### 3.2 Checkpoint Collection (Sequential Order)
- [ ] **Test**: Robot starts with 0 checkpoints collected
  - **Steps**: Spawn robot, check CurrentCheckpoint
  - **Expected**: CurrentCheckpoint == 0

- [ ] **Test**: Collecting checkpoint 1 increments counter
  - **Steps**: Move onto checkpoint 1
  - **Expected**: CurrentCheckpoint == 1

- [ ] **Test**: Checkpoints must be collected in order
  - **Steps**: Skip checkpoint 1, move to checkpoint 2
  - **Expected**: Checkpoint 2 not collected, warning shown

- [ ] **Test**: Collecting out-of-order checkpoint shows feedback
  - **Steps**: Collect checkpoint 3 before checkpoint 2
  - **Expected**: HUD shows "Wrong checkpoint!" or similar message

- [ ] **Test**: Sequential collection 1→2→3 works
  - **Steps**: Collect checkpoints 1, 2, 3 in order
  - **Expected**: CurrentCheckpoint == 3

- [ ] **Test**: Re-visiting collected checkpoint does nothing
  - **Steps**: Collect checkpoint 1, move onto it again
  - **Expected**: No effect, CurrentCheckpoint remains 1

- [ ] **Test**: Checkpoint counter shown in HUD
  - **Steps**: Collect checkpoints, check HUD
  - **Expected**: HUD shows "Checkpoints: 2/5" or similar

#### 3.3 Checkpoint Numbering
- [ ] **Test**: GetCheckpointNumber() returns correct value
  - **Steps**: Query checkpoint tile at position
  - **Expected**: Returns checkpoint's assigned number (1, 2, 3, etc.)

- [ ] **Test**: Non-checkpoint tiles return 0
  - **Steps**: Query GetCheckpointNumber() on normal tile
  - **Expected**: Returns 0

#### 3.4 Win Condition
- [ ] **Test**: Collecting all checkpoints triggers win
  - **Steps**: Collect all checkpoints in order
  - **Expected**: Game state changes to GameOver (win)

- [ ] **Test**: Win message shown in HUD
  - **Steps**: Collect all checkpoints
  - **Expected**: HUD shows "You Win!" or victory message

- [ ] **Test**: Cannot continue after winning
  - **Steps**: Win game, try to move robot
  - **Expected**: Input ignored, game stopped

---

### 4. Lives System

#### 4.1 Lives Initialization
- [ ] **Test**: Robot starts with 3 lives
  - **Steps**: Spawn robot, check Lives property
  - **Expected**: Lives == 3

- [ ] **Test**: Lives shown in HUD
  - **Steps**: Check on-screen display
  - **Expected**: HUD shows "Lives: 3" or heart icons

#### 4.2 Lives Depletion
- [ ] **Test**: Death decrements lives
  - **Steps**: Die once, check Lives
  - **Expected**: Lives == 2

- [ ] **Test**: Lives counter updates in HUD
  - **Steps**: Die, check HUD
  - **Expected**: HUD shows "Lives: 2"

- [ ] **Test**: Can die multiple times (3 total)
  - **Steps**: Die 3 times
  - **Expected**: Lives decrements to 0

- [ ] **Test**: Game over at 0 lives
  - **Steps**: Die 3 times (use all lives)
  - **Expected**: Game state changes to GameOver (lose)

- [ ] **Test**: Cannot continue after 0 lives
  - **Steps**: Lose all lives, try to move
  - **Expected**: Input ignored, game stopped

- [ ] **Test**: Lose message shown in HUD
  - **Steps**: Lose all lives
  - **Expected**: HUD shows "Game Over" or defeat message

---

### 5. Respawn System

#### 5.1 Respawn Position Tracking
- [ ] **Test**: Initial respawn position is spawn point
  - **Steps**: Check RespawnPosition at game start
  - **Expected**: RespawnPosition == robot's starting grid position

- [ ] **Test**: Respawn position updates on checkpoint collection
  - **Steps**: Collect checkpoint 1 at position (5,5)
  - **Expected**: RespawnPosition == (5,5)

- [ ] **Test**: Respawn position updates only on checkpoint
  - **Steps**: Move to various tiles, check RespawnPosition
  - **Expected**: RespawnPosition only changes when checkpoint collected

- [ ] **Test**: Respawn position uses latest checkpoint
  - **Steps**: Collect checkpoint 1, then checkpoint 2
  - **Expected**: RespawnPosition == checkpoint 2 position

#### 5.2 Respawn Mechanics
- [ ] **Test**: Death triggers respawn
  - **Steps**: Die by falling in pit
  - **Expected**: Robot respawns at RespawnPosition

- [ ] **Test**: Robot respawns at last checkpoint
  - **Steps**: Collect checkpoint 2, die, check position
  - **Expected**: Robot at checkpoint 2 position

- [ ] **Test**: Robot respawns at spawn if no checkpoints collected
  - **Steps**: Die before collecting any checkpoints
  - **Expected**: Robot at original spawn position

- [ ] **Test**: Respawn restores robot facing direction
  - **Steps**: Collect checkpoint facing North, rotate, die
  - **Expected**: Robot respawns facing same direction as when checkpoint collected

- [ ] **Test**: Respawn resets damage to 0
  - **Steps**: Take 8 damage, die, check Damage after respawn
  - **Expected**: Damage == 0

- [ ] **Test**: Respawn clears programmed registers
  - **Steps**: Program 3 cards, die, check registers
  - **Expected**: All registers empty (or define if they persist)

- [ ] **Test**: Respawn deals new hand
  - **Steps**: Die, check PlayerHand
  - **Expected**: New 9-card hand dealt

- [ ] **Test**: Respawn preserves checkpoint progress
  - **Steps**: Collect checkpoints 1-2, die, check CurrentCheckpoint
  - **Expected**: CurrentCheckpoint == 2 (progress preserved)

- [ ] **Test**: Respawn animation/effect plays
  - **Steps**: Die, observe respawn
  - **Expected**: Visual effect or teleport animation (if implemented)

#### 5.3 Respawn State
- [ ] **Test**: Respawn returns to Programming state
  - **Steps**: Die during Executing, check state after respawn
  - **Expected**: CurrentGameState == Programming

- [ ] **Test**: Can move immediately after respawn
  - **Steps**: Respawn, press WASD
  - **Expected**: Robot responds to input

---

### 6. Integration Tests (Combined Systems)

#### 6.1 Full Gameplay Loop
- [ ] **Test**: Complete turn with WASD + cards
  - **Steps**: Move with WASD, program 5 cards, press E
  - **Expected**: Both movement types work correctly

- [ ] **Test**: Damage affects hand, respawn resets it
  - **Steps**: Take 5 damage (hand=4), die, respawn
  - **Expected**: Hand size returns to 9 after respawn

- [ ] **Test**: Collect checkpoints, die, respawn, continue collecting
  - **Steps**: Collect checkpoint 1, die, respawn, collect checkpoint 2
  - **Expected**: Checkpoint progress preserved, can continue from 2

- [ ] **Test**: Use all lives, game ends
  - **Steps**: Die 3 times
  - **Expected**: Game state = GameOver, no respawn on 3rd death

#### 6.2 Checkpoint + Lives Interaction
- [ ] **Test**: Checkpoint sets new respawn, die multiple times
  - **Steps**: Collect checkpoint 2, die twice
  - **Expected**: Both respawns at checkpoint 2 position, Lives == 1

- [ ] **Test**: Reach last checkpoint with 1 life remaining
  - **Steps**: Die twice, collect all checkpoints
  - **Expected**: Win condition triggers despite low lives

#### 6.3 Health + Respawn Interaction
- [ ] **Test**: Die from accumulated damage (not pit)
  - **Steps**: Take laser damage until death (10 damage)
  - **Expected**: OnDeath fires, respawn occurs

- [ ] **Test**: Die from pit during card execution
  - **Steps**: Program Move3 into pit, execute
  - **Expected**: Death during Executing state, respawn to Programming

---

### 7. Edge Cases & Error Handling

#### 7.1 Input Edge Cases
- [ ] **Test**: Mash WASD rapidly
  - **Steps**: Press WASD keys very quickly
  - **Expected**: Inputs queued or ignored, no crashes

- [ ] **Test**: Hold down W key
  - **Steps**: Hold W continuously
  - **Expected**: Robot moves repeatedly or input rate-limited

- [ ] **Test**: Press all keys simultaneously
  - **Steps**: Press WASD + 1-9 + E + Backspace at once
  - **Expected**: No crash, inputs processed safely

#### 7.2 Health Edge Cases
- [ ] **Test**: Apply negative damage
  - **Steps**: ApplyDamage(-5)
  - **Expected**: Damage doesn't go below 0, or error logged

- [ ] **Test**: Apply damage > 10 in one hit
  - **Steps**: ApplyDamage(100)
  - **Expected**: Robot dies (clamped to max damage)

- [ ] **Test**: Die during respawn
  - **Steps**: Respawn on pit tile (if possible)
  - **Expected**: Immediate re-death or error prevented

#### 7.3 Checkpoint Edge Cases
- [ ] **Test**: Multiple checkpoints on same tile
  - **Steps**: Set two checkpoint numbers on one tile
  - **Expected**: Error prevented or higher number used

- [ ] **Test**: Checkpoint number gaps (1, 3, 5 - no 2, 4)
  - **Steps**: Try to collect checkpoint 3 after checkpoint 1
  - **Expected**: Validation fails (expects sequential)

- [ ] **Test**: Checkpoint number 0
  - **Steps**: Create checkpoint with number 0
  - **Expected**: Ignored or error logged

#### 7.4 Lives Edge Cases
- [ ] **Test**: Set lives to negative
  - **Steps**: Manually set Lives = -1
  - **Expected**: Clamped to 0, game over triggered

- [ ] **Test**: Set lives to very high number
  - **Steps**: Set Lives = 100
  - **Expected**: Can die 100 times (no overflow)

#### 7.5 Respawn Edge Cases
- [ ] **Test**: Respawn position out of bounds
  - **Steps**: Set RespawnPosition to (-10, -10)
  - **Expected**: Clamped to valid grid position or error

- [ ] **Test**: Respawn on occupied tile (if multi-robot)
  - **Steps**: Respawn where another robot is
  - **Expected**: Collision handled or offset applied

---

## Testing Procedures

### Manual Testing Steps

1. **Setup**: Open RobotRally.uproject in UE5 Editor
2. **Open Level**: Content/RobotRally/Maps/MainMap
3. **Play in Editor (PIE)**: Press Play button or Alt+P
4. **Open Output Log**: Window → Developer Tools → Output Log

### Testing Workflow

#### Test Player Input
1. Press WASD → Verify robot moves and rotates
2. Press 1-9 → Verify cards select into registers
3. Press Backspace → Verify last card removed
4. Press E → Verify execution starts
5. Change state → Verify input blocking works

#### Test Health System
1. Move onto laser tile → Verify 1 damage taken
2. Check HUD → Verify health bar decreases
3. Take 10 damage → Verify death occurs
4. Check lives counter → Verify decremented

#### Test Checkpoint System
1. Move to checkpoint 2 first → Verify rejection
2. Move to checkpoint 1 → Verify collection
3. Move to checkpoint 2 → Verify collection
4. Check HUD → Verify counter shows 2/N
5. Collect all → Verify win condition

#### Test Respawn System
1. Collect checkpoint at (5,5)
2. Move away to (8,8)
3. Die (pit or 10 damage)
4. Verify respawn at (5,5)
5. Verify damage reset to 0
6. Verify new hand dealt

### Debug Output Examples

```cpp
// In ARobotController::SetupInputComponent()
UE_LOG(LogTemp, Warning, TEXT("RobotController input bound successfully"));

// In ARobotPawn::ApplyDamage()
UE_LOG(LogTemp, Warning, TEXT("Damage applied: %d → %d (Lives: %d)"),
    OldDamage, Damage, Lives);

// In ARobotPawn::OnCheckpointReached()
UE_LOG(LogTemp, Warning, TEXT("Checkpoint %d collected (Expected: %d)"),
    CheckpointNumber, CurrentCheckpoint + 1);

// In ARobotPawn::Respawn()
UE_LOG(LogTemp, Warning, TEXT("Respawning at %s (Lives: %d, Checkpoints: %d)"),
    *RespawnPosition.ToString(), Lives, CurrentCheckpoint);
```

### Performance Testing

- [ ] **Test**: Rapid input doesn't cause lag
  - **Steps**: Mash keys for 30 seconds
  - **Expected**: Framerate stable, no memory leaks

- [ ] **Test**: Multiple deaths don't cause lag
  - **Steps**: Die and respawn 20 times rapidly
  - **Expected**: Performance remains consistent

---

## Success Criteria

Phase 3 testing is considered **COMPLETE** when:

- ✅ All 120+ test cases above pass
- ✅ WASD movement works correctly in Programming state only
- ✅ Card selection (1-9), execution (E), and undo (Backspace) work correctly
- ✅ Input properly blocked in wrong game states
- ✅ Health system tracks damage and triggers death at 10
- ✅ Damage shown in HUD, affects hand size
- ✅ OnDeath delegate fires correctly
- ✅ Checkpoints must be collected in sequential order
- ✅ Wrong-order checkpoints show feedback message
- ✅ Lives counter starts at 3, decrements on death
- ✅ Game over occurs at 0 lives
- ✅ Respawn occurs at last checkpoint collected
- ✅ Respawn resets damage to 0
- ✅ Checkpoint progress preserved across deaths
- ✅ Win condition triggers on all checkpoints collected
- ✅ No crashes or errors in Output Log during testing

---

## Known Issues & Limitations

No known limitations for Phase 3 — all items marked as COMPLETE in README.

### Future Enhancements
- Multiple players / networked multiplayer
- Different robot models / customization
- Checkpoint animations / sound effects
- Respawn invulnerability period

---

## Next Steps After Phase 3 Testing

1. Fix any bugs found during testing
2. Proceed to Phase 4 testing (Field Hazards & Environment)
3. Consider adding visual polish (particle effects on respawn, checkpoint animations)

---

## References

- [README.md](../../README.md) — Phase 3 implementation checklist
- [CLAUDE.md](../../CLAUDE.md) — Architecture and conventions
- [Phase 1 Testing Plan](./2026-02-08_phase1-testing-plan.md) — Movement system tests
- [Phase 2 Testing Plan](./2026-02-08_phase2-testing-plan.md) — Card system tests
- Source Files:
  - `Source/RobotRally/RobotController.h/.cpp` — Player input handling
  - `Source/RobotRally/RobotPawn.h/.cpp` — Health, lives, checkpoints, respawn
  - `Source/RobotRally/RobotRallyHUD.h/.cpp` — Health bar, lives counter, checkpoint display
  - `Config/DefaultInput.ini` — WASD, 1-9, E, Backspace bindings
