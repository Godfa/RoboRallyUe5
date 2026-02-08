# Phase 2 Testing Plan — Logic Layer (GameMode & Cards)

**Created**: 2026-02-08
**Status**: Draft
**Complexity**: Medium

## Overview

This document outlines the testing procedures for Phase 2 of the RobotRally project, focusing on the game logic layer including the state machine, card system, deck management, and programming mechanics. Phase 2 builds on Phase 1's movement foundation to add strategic card-based gameplay.

## Phase 2 Components Under Test

- **ARobotRallyGameMode** — State machine (Programming → Executing → GameOver)
- **FRobotCard** — Card structure (ECardAction + Priority)
- **Card Deck** — 84-card generation and Fisher-Yates shuffle
- **Hand System** — Dealing cards based on robot damage
- **Register Programming** — 5 registers, card selection with 1-9 keys
- **Card Execution** — Sequential register processing

---

## Testing Checklist

### 1. Game State Machine (ARobotRallyGameMode)

#### 1.1 State Transitions
- [ ] **Test**: Game starts in Programming state
  - **Steps**: Launch game in PIE, check CurrentGameState
  - **Expected**: CurrentGameState == EGameState::Programming

- [ ] **Test**: Transition from Programming to Executing
  - **Steps**: Press E to execute programmed cards
  - **Expected**: CurrentGameState changes to EGameState::Executing

- [ ] **Test**: Transition from Executing back to Programming
  - **Steps**: Wait for all 5 registers to execute
  - **Expected**: CurrentGameState returns to EGameState::Programming

- [ ] **Test**: Transition to GameOver on robot death
  - **Steps**: Move robot into pit tile
  - **Expected**: CurrentGameState changes to EGameState::GameOver

- [ ] **Test**: Transition to GameOver on all checkpoints collected
  - **Steps**: Collect all checkpoints in order
  - **Expected**: CurrentGameState changes to EGameState::GameOver

- [ ] **Test**: Cannot execute cards in Executing state
  - **Steps**: Press E while already executing
  - **Expected**: Input ignored, no double-execution

- [ ] **Test**: Cannot execute cards in GameOver state
  - **Steps**: Press E after game ends
  - **Expected**: Input ignored

#### 1.2 State Persistence
- [ ] **Test**: State persists between frames
  - **Steps**: Set state to Executing, wait several frames
  - **Expected**: State remains Executing until registers complete

- [ ] **Test**: State visible in HUD
  - **Steps**: Check on-screen HUD display
  - **Expected**: HUD shows current game state text

---

### 2. Card Structure (FRobotCard)

#### 2.1 Card Properties
- [ ] **Test**: FRobotCard contains Action field
  - **Steps**: Create card, check Action property exists
  - **Expected**: Action is of type ECardAction

- [ ] **Test**: FRobotCard contains Priority field
  - **Steps**: Create card, check Priority property exists
  - **Expected**: Priority is integer type

- [ ] **Test**: All 7 card actions are defined
  - **Steps**: Check ECardAction enum
  - **Expected**: Contains Move1, Move2, Move3, MoveBack, RotateRight, RotateLeft, UTurn

#### 2.2 Card Comparison
- [ ] **Test**: Cards can be compared by priority
  - **Steps**: Create two cards with different priorities
  - **Expected**: Can sort or compare cards based on Priority field

---

### 3. Card Deck System

#### 3.1 Deck Generation
- [ ] **Test**: Deck contains 84 cards total
  - **Steps**: Generate deck in BeginPlay(), check CardDeck.Num()
  - **Expected**: CardDeck.Num() == 84

- [ ] **Test**: Deck contains correct distribution of actions
  - **Steps**: Count each action type in deck
  - **Expected**: Matches Robot Rally card distribution:
    - Move1: 18 cards
    - Move2: 12 cards
    - Move3: 6 cards
    - MoveBack: 6 cards
    - RotateRight: 18 cards
    - RotateLeft: 18 cards
    - UTurn: 6 cards

- [ ] **Test**: Each card has unique priority
  - **Steps**: Check all 84 priorities are different
  - **Expected**: No duplicate priority values (range 10-840, step 10)

- [ ] **Test**: Priority values are in valid range
  - **Steps**: Check min/max priority in deck
  - **Expected**: Min = 10, Max = 840, all multiples of 10

#### 3.2 Deck Shuffling
- [ ] **Test**: Fisher-Yates shuffle randomizes deck
  - **Steps**: Generate deck twice, compare order
  - **Expected**: Different card order each time (non-deterministic unless seeded)

- [ ] **Test**: Shuffle preserves all cards
  - **Steps**: Count cards before and after shuffle
  - **Expected**: Still 84 cards, no duplicates or missing cards

- [ ] **Test**: Shuffle doesn't modify card properties
  - **Steps**: Check card actions/priorities before and after shuffle
  - **Expected**: Same cards exist, just in different order

#### 3.3 Deck Depletion
- [ ] **Test**: Cards removed from deck when dealt
  - **Steps**: Deal 9 cards, check CardDeck.Num()
  - **Expected**: CardDeck.Num() == 75 (84 - 9)

- [ ] **Test**: Deck regenerates when empty
  - **Steps**: Deal all 84 cards, try to deal more
  - **Expected**: Deck regenerates and reshuffles (or error if not implemented)

---

### 4. Hand System

#### 4.1 Card Dealing
- [ ] **Test**: Hand dealt at start of Programming phase
  - **Steps**: Enter Programming state, check PlayerHand array
  - **Expected**: PlayerHand contains 9 cards

- [ ] **Test**: Hand size based on robot damage (0 damage = 9 cards)
  - **Steps**: Robot at full health (0 damage)
  - **Expected**: PlayerHand.Num() == 9

- [ ] **Test**: Hand size reduces with damage (5 damage = 4 cards)
  - **Steps**: Deal 5 damage to robot, enter Programming phase
  - **Expected**: PlayerHand.Num() == 4 (9 - 5)

- [ ] **Test**: Hand size minimum is 1 card (8+ damage)
  - **Steps**: Deal 8 damage to robot, enter Programming phase
  - **Expected**: PlayerHand.Num() == 1

- [ ] **Test**: Hand size never exceeds 9 cards
  - **Steps**: Heal robot to negative damage (if possible)
  - **Expected**: PlayerHand.Num() <= 9

- [ ] **Test**: Hand contains valid cards from deck
  - **Steps**: Check each card in PlayerHand has valid Action and Priority
  - **Expected**: All cards have defined ECardAction and positive Priority

#### 4.2 Hand Display
- [ ] **Test**: Hand visible in HUD during Programming
  - **Steps**: Enter Programming state, check HUD
  - **Expected**: 9 cards displayed on screen with actions and priorities

- [ ] **Test**: Hand updates after damage
  - **Steps**: Take damage, enter next Programming phase
  - **Expected**: Fewer cards shown in HUD

---

### 5. Register Programming

#### 5.1 Card Selection (1-9 Keys)
- [ ] **Test**: Pressing 1 selects first card in hand
  - **Steps**: Press 1 key during Programming
  - **Expected**: Card at PlayerHand[0] selected

- [ ] **Test**: Pressing 9 selects ninth card in hand
  - **Steps**: Press 9 key during Programming
  - **Expected**: Card at PlayerHand[8] selected

- [ ] **Test**: Pressing key beyond hand size is ignored
  - **Steps**: With 4-card hand, press 9
  - **Expected**: No selection, no error

- [ ] **Test**: Selection only works in Programming state
  - **Steps**: Press 1-9 during Executing or GameOver
  - **Expected**: Input ignored

#### 5.2 Register Filling
- [ ] **Test**: Selected card fills first empty register
  - **Steps**: Press 1, check Registers[0]
  - **Expected**: Registers[0] contains selected card

- [ ] **Test**: Second selection fills second register
  - **Steps**: Press 1, then press 2
  - **Expected**: Registers[0] and Registers[1] filled

- [ ] **Test**: All 5 registers can be filled
  - **Steps**: Select 5 different cards
  - **Expected**: Registers[0] through Registers[4] all contain cards

- [ ] **Test**: Cannot select more than 5 cards
  - **Steps**: Fill 5 registers, press 6th card
  - **Expected**: Input ignored or replaces last register (define behavior)

- [ ] **Test**: Selected card removed from hand
  - **Steps**: Press 1, check PlayerHand
  - **Expected**: Card moved to register, PlayerHand.Num() decreases

#### 5.3 Card Undo (Backspace)
- [ ] **Test**: Backspace removes last programmed card
  - **Steps**: Select 3 cards, press Backspace
  - **Expected**: Registers[2] emptied

- [ ] **Test**: Backspace returns card to hand
  - **Steps**: Select card, press Backspace, check PlayerHand
  - **Expected**: Card returned to hand in same position

- [ ] **Test**: Multiple Backspace clears all registers
  - **Steps**: Fill 5 registers, press Backspace 5 times
  - **Expected**: All registers empty, all cards back in hand

- [ ] **Test**: Backspace on empty registers does nothing
  - **Steps**: Press Backspace with no cards selected
  - **Expected**: No error, no state change

- [ ] **Test**: Backspace only works in Programming state
  - **Steps**: Press Backspace during Executing
  - **Expected**: Input ignored

#### 5.4 Register Display
- [ ] **Test**: Registers visible in HUD
  - **Steps**: Program 3 cards, check HUD
  - **Expected**: 3 filled registers shown, 2 empty slots

- [ ] **Test**: Register shows card action and priority
  - **Steps**: Program Move1 (priority 50) into register
  - **Expected**: HUD displays "Move 1 [50]"

---

### 6. Card Execution System

#### 6.1 Execution Trigger (E Key)
- [ ] **Test**: E key starts execution
  - **Steps**: Program 5 cards, press E
  - **Expected**: CurrentGameState changes to Executing

- [ ] **Test**: E key requires at least 1 programmed card
  - **Steps**: Press E with empty registers
  - **Expected**: Input ignored or warning shown

- [ ] **Test**: E key only works in Programming state
  - **Steps**: Press E during Executing or GameOver
  - **Expected**: Input ignored

#### 6.2 Sequential Register Processing
- [ ] **Test**: Registers execute in order 1→2→3→4→5
  - **Steps**: Program 5 different cards, press E, observe
  - **Expected**: Cards execute in register order, not priority order

- [ ] **Test**: Empty registers are skipped
  - **Steps**: Program only registers 1, 3, 5 (skip 2 and 4)
  - **Expected**: Only 3 cards execute, empty registers ignored

- [ ] **Test**: Each register waits for movement completion
  - **Steps**: Program Move3, then RotateRight
  - **Expected**: Rotation waits until 3-tile move finishes

- [ ] **Test**: Register 1 executes first
  - **Steps**: Add logging, press E
  - **Expected**: Console shows "Executing Register 0" first

- [ ] **Test**: Register 5 executes last
  - **Steps**: Add logging, press E
  - **Expected**: Console shows "Executing Register 4" last

#### 6.3 Card Action Execution
- [ ] **Test**: Move1 card moves robot 1 tile
  - **Steps**: Program Move1 in register, press E
  - **Expected**: Robot moves forward 1 tile

- [ ] **Test**: Move2 card moves robot 2 tiles
  - **Steps**: Program Move2 in register, press E
  - **Expected**: Robot moves forward 2 tiles

- [ ] **Test**: Move3 card moves robot 3 tiles
  - **Steps**: Program Move3 in register, press E
  - **Expected**: Robot moves forward 3 tiles

- [ ] **Test**: MoveBack card moves robot backward
  - **Steps**: Program MoveBack in register, press E
  - **Expected**: Robot moves backward 1 tile

- [ ] **Test**: RotateRight card rotates robot 90° CW
  - **Steps**: Program RotateRight in register, press E
  - **Expected**: Robot rotates 90° clockwise

- [ ] **Test**: RotateLeft card rotates robot 90° CCW
  - **Steps**: Program RotateLeft in register, press E
  - **Expected**: Robot rotates 90° counter-clockwise

- [ ] **Test**: UTurn card rotates robot 180°
  - **Steps**: Program UTurn in register, press E
  - **Expected**: Robot rotates 180°

#### 6.4 Execution Completion
- [ ] **Test**: After all registers execute, state returns to Programming
  - **Steps**: Execute 5-card sequence, wait for completion
  - **Expected**: CurrentGameState changes back to Programming

- [ ] **Test**: Registers cleared after execution
  - **Steps**: Execute sequence, check Registers array
  - **Expected**: All registers empty (or define if they persist)

- [ ] **Test**: New hand dealt after execution completes
  - **Steps**: Execute sequence, check PlayerHand
  - **Expected**: New 9 cards dealt from deck

---

### 7. Priority System (Future: Multi-Robot)

#### 7.1 Priority Awareness (Single Robot)
- [ ] **Test**: Each card has priority value
  - **Steps**: Check Priority field on cards in hand
  - **Expected**: All cards have priority between 10-840

- [ ] **Test**: Priority displayed in HUD
  - **Steps**: Check card display in hand
  - **Expected**: Each card shows priority number

#### 7.2 Priority-Based Execution (Not Yet Implemented)
- [ ] **Test**: Single robot ignores priority (sequential only)
  - **Steps**: Program low-priority card first, high-priority second
  - **Expected**: Executes in register order (1→2), not priority order

- [ ] **Note**: Priority-based execution requires multi-robot support (future Phase)

---

### 8. Edge Cases & Error Handling

#### 8.1 Invalid States
- [ ] **Test**: Cannot deal cards in Executing state
  - **Steps**: Trigger deal during execution
  - **Expected**: Operation blocked or queued

- [ ] **Test**: Cannot deal cards in GameOver state
  - **Steps**: Trigger deal after game ends
  - **Expected**: Operation blocked

#### 8.2 Input Validation
- [ ] **Test**: Invalid register index handled gracefully
  - **Steps**: Manually call ProcessNextRegister() with index 10
  - **Expected**: No crash, error logged

- [ ] **Test**: Null card in register handled gracefully
  - **Steps**: Execute with nullptr in Registers[2]
  - **Expected**: Register skipped, no crash

#### 8.3 Deck Edge Cases
- [ ] **Test**: Dealing more cards than deck size
  - **Steps**: Deal 85 cards (more than 84 in deck)
  - **Expected**: Deck reshuffles or error handled

- [ ] **Test**: Concurrent deal operations
  - **Steps**: Trigger DealCards() twice rapidly
  - **Expected**: Second call ignored or queued

#### 8.4 Hand/Register Edge Cases
- [ ] **Test**: Hand size with extreme damage
  - **Steps**: Set damage to 100
  - **Expected**: Hand size = 1 (minimum enforced)

- [ ] **Test**: Hand size with negative damage
  - **Steps**: Set damage to -10 (healing beyond max)
  - **Expected**: Hand size = 9 (maximum enforced)

- [ ] **Test**: Programming partial registers
  - **Steps**: Fill only 2 registers, press E
  - **Expected**: Only 2 cards execute, state returns to Programming

---

## Testing Procedures

### Manual Testing Steps

1. **Setup**: Open RobotRally.uproject in UE5 Editor
2. **Open Level**: Content/RobotRally/Maps/MainMap
3. **Play in Editor (PIE)**: Press Play button or Alt+P
4. **Open Output Log**: Window → Developer Tools → Output Log

### Testing Workflow

#### Test Card Selection
1. Verify 9 cards shown in HUD during Programming
2. Press 1 → First card should move to Register 1
3. Press 2 → Second card should move to Register 2
4. Continue until 5 registers filled
5. Press Backspace → Last register should clear
6. Verify card returns to hand

#### Test Card Execution
1. Program 5 cards into registers
2. Press E → State should change to Executing
3. Watch robot execute each card in sequence
4. Verify smooth interpolation between moves
5. After 5th card, state should return to Programming
6. Verify new hand dealt

#### Test Damage Impact on Hand Size
1. Find laser tile or pit
2. Take 3 damage (hand should be 6 cards)
3. Enter Programming, count cards in HUD
4. Take 5 more damage (hand should be 1 card)
5. Verify minimum enforced

### Debug Output Examples

```cpp
// In ARobotRallyGameMode::DealCards()
UE_LOG(LogTemp, Warning, TEXT("Dealing %d cards (Damage=%d)"),
    HandSize, RobotPawn->GetDamage());

// In ARobotRallyGameMode::ProcessNextRegister()
UE_LOG(LogTemp, Warning, TEXT("Executing Register %d: %s [Priority=%d]"),
    CurrentRegister, *GetActionName(Card.Action), Card.Priority);

// In ARobotRallyGameMode::SetGameState()
UE_LOG(LogTemp, Warning, TEXT("GameState: %s → %s"),
    *GetStateName(OldState), *GetStateName(NewState));
```

### Automated Testing (Future Enhancement)

Consider adding C++ unit tests:
- `CardDeck.spec.cpp` — Deck generation, shuffling, distribution
- `GameMode.spec.cpp` — State machine transitions
- `RegisterSystem.spec.cpp` — Card selection and register logic

---

## Success Criteria

Phase 2 testing is considered **COMPLETE** when:

- ✅ All 100+ test cases above pass
- ✅ State machine transitions correctly
- ✅ Deck contains 84 cards with correct distribution
- ✅ Hand size scales with damage (9 → 1)
- ✅ Card selection (1-9 keys) fills registers correctly
- ✅ Backspace undo works properly
- ✅ E key executes all 5 registers in sequence
- ✅ All 7 card actions trigger correct robot movement
- ✅ New hand dealt after execution completes
- ✅ No crashes or errors in Output Log during testing
- ✅ HUD displays cards, registers, and game state correctly

---

## Known Issues & Limitations

- **Priority-Based Execution** (Phase 2 incomplete item): Currently not implemented, requires multi-robot support
  - **Current**: Registers execute sequentially (1→5) regardless of priority
  - **Future**: In multi-robot game, all robots' Register 1 cards execute by priority, then Register 2, etc.

- **UCardDataAsset** (Phase 2 optional item): Data-driven card approach not implemented
  - **Current**: Cards hardcoded in GameMode
  - **Future**: Consider DataAsset for modding/expansion support

---

## Next Steps After Phase 2 Testing

1. Fix any bugs found during testing
2. Consider implementing visual card UI (currently text-based HUD)
3. Proceed to Phase 3 testing (Player Control and Health System)
4. Future: Implement priority-based execution when multi-robot support added

---

## References

- [README.md](../../README.md) — Phase 2 implementation checklist
- [CLAUDE.md](../../CLAUDE.md) — Architecture and conventions
- [Phase 1 Testing Plan](./2026-02-08_phase1-testing-plan.md) — Movement system tests
- Source Files:
  - `Source/RobotRally/RobotRallyGameMode.h/.cpp`
  - `Source/RobotRally/RobotController.h/.cpp`
  - `Source/RobotRally/RobotRallyHUD.h/.cpp`
  - `Config/DefaultInput.ini` — Key bindings (1-9, E, Backspace)
