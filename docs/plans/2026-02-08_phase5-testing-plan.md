# Phase 5 Testing Plan — UI & Interface

**Created**: 2026-02-08
**Status**: Draft
**Complexity**: Medium

## Overview

This document outlines the testing procedures for Phase 5 of the RobotRally project, focusing on user interface widgets including the programming deck, card slots, and main HUD. Phase 5 replaces or enhances the existing C++ HUD with rich Blueprint-based UMG widgets for improved visual feedback and usability.

## Phase 5 Components Under Test

- **WBP_ProgrammingDeck** — Card selection view showing hand (up to 9 cards) and registers (5 slots)
- **WBP_CardSlot** — Single card widget displaying action type, priority, and visual styling
- **WBP_HUD** — Main heads-up display showing health, lives, game phase, and checkpoint progress

---

## Testing Checklist

### 1. WBP_CardSlot (Individual Card Widget)

#### 1.1 Card Appearance
- [ ] **Test**: Card displays action type text
  - **Steps**: Create card with ECardAction::Move1
  - **Expected**: Card shows "Move 1" or "→" icon

- [ ] **Test**: All 7 card actions have distinct visuals
  - **Steps**: Create one card of each type (Move1/2/3, MoveBack, RotateRight/Left, UTurn)
  - **Expected**: Each card visually distinguishable (text or icon)

- [ ] **Test**: Card displays priority number
  - **Steps**: Create card with Priority=350
  - **Expected**: Card shows "350" in corner or designated area

- [ ] **Test**: Card has background/border
  - **Steps**: View card widget
  - **Expected**: Visible card outline, colored background

- [ ] **Test**: Card size is reasonable
  - **Steps**: Check card dimensions in viewport
  - **Expected**: Card fits in hand/register UI, readable text

#### 1.2 Card States
- [ ] **Test**: Card has "in hand" visual state
  - **Steps**: Place card in hand area
  - **Expected**: Default appearance (white/normal color)

- [ ] **Test**: Card has "in register" visual state
  - **Steps**: Move card to register slot
  - **Expected**: Different appearance (highlighted/locked appearance)

- [ ] **Test**: Card has "hover" visual state (if mouse interaction)
  - **Steps**: Hover mouse over card
  - **Expected**: Highlight or glow effect

- [ ] **Test**: Card has "selected" visual state (if mouse interaction)
  - **Steps**: Click card
  - **Expected**: Border highlight or color change

- [ ] **Test**: Card has "disabled" visual state
  - **Steps**: Card in register during Executing state
  - **Expected**: Grayed out or locked appearance

#### 1.3 Card Animations (Optional)
- [ ] **Test**: Card has appear animation
  - **Steps**: Deal new hand
  - **Expected**: Cards fade in or slide in

- [ ] **Test**: Card has selection animation
  - **Steps**: Select card with 1-9 key
  - **Expected**: Card slides/moves to register

- [ ] **Test**: Card has execution animation
  - **Steps**: Execute cards
  - **Expected**: Cards highlight or glow as they execute

#### 1.4 Card Readability
- [ ] **Test**: Text readable at normal resolution (1920x1080)
  - **Steps**: View cards at 100% UI scale
  - **Expected**: Action and priority text clearly readable

- [ ] **Test**: Text readable at low resolution (1280x720)
  - **Steps**: Change resolution, view cards
  - **Expected**: Text still legible

- [ ] **Test**: Cards distinguishable in colorblind modes (if implemented)
  - **Steps**: Enable colorblind filter, view cards
  - **Expected**: Cards use shapes/icons, not just color

---

### 2. WBP_ProgrammingDeck (Card Selection View)

#### 2.1 Deck Layout
- [ ] **Test**: Deck widget visible during Programming state
  - **Steps**: Enter Programming state
  - **Expected**: Programming deck widget appears on screen

- [ ] **Test**: Deck widget hidden during Executing state
  - **Steps**: Press E to execute
  - **Expected**: Programming deck hidden or minimized

- [ ] **Test**: Deck widget hidden during GameOver state
  - **Steps**: Win or lose game
  - **Expected**: Programming deck hidden

- [ ] **Test**: Hand area displays up to 9 cards
  - **Steps**: Deal full hand (9 cards)
  - **Expected**: 9 card slots visible in hand area

- [ ] **Test**: Register area displays 5 slots
  - **Steps**: View programming deck
  - **Expected**: 5 register slots visible and labeled (1, 2, 3, 4, 5)

#### 2.2 Hand Display
- [ ] **Test**: Cards appear in hand at Programming start
  - **Steps**: Enter Programming state, check hand area
  - **Expected**: 9 cards visible with actions and priorities

- [ ] **Test**: Hand shows correct number based on damage
  - **Steps**: Take 3 damage, enter Programming
  - **Expected**: 6 cards shown in hand area

- [ ] **Test**: Hand shows minimum 1 card at high damage
  - **Steps**: Take 8+ damage, enter Programming
  - **Expected**: 1 card shown in hand area

- [ ] **Test**: Empty hand slots are visible (if damage reduces hand)
  - **Steps**: Take 5 damage (4 cards)
  - **Expected**: 4 cards shown, 5 empty/grayed slots

- [ ] **Test**: Hand cards sorted or ordered consistently
  - **Steps**: Deal multiple hands, check order
  - **Expected**: Cards in consistent order (by priority? or dealt order?)

#### 2.3 Register Display
- [ ] **Test**: Registers start empty at Programming start
  - **Steps**: Enter new Programming phase
  - **Expected**: All 5 register slots empty/gray

- [ ] **Test**: Selecting card (key 1) fills register 1
  - **Steps**: Press 1 key
  - **Expected**: Card appears in register slot 1

- [ ] **Test**: Selecting 5 cards fills all registers
  - **Steps**: Press 1, 2, 3, 4, 5 keys
  - **Expected**: All 5 register slots filled with cards

- [ ] **Test**: Register slots numbered/labeled
  - **Steps**: View register area
  - **Expected**: Slots labeled "Register 1", "Register 2", etc. (or numbered 1-5)

- [ ] **Test**: Cards in registers show action and priority
  - **Steps**: Fill registers, check display
  - **Expected**: Each card shows action type and priority number

#### 2.4 Card Selection Visual Feedback
- [ ] **Test**: Selected card disappears from hand
  - **Steps**: Press 1 key
  - **Expected**: First card removed from hand area

- [ ] **Test**: Selected card appears in register
  - **Steps**: Press 1 key
  - **Expected**: Card appears in first empty register slot

- [ ] **Test**: Card movement is smooth (animation)
  - **Steps**: Press 1 key, watch transition
  - **Expected**: Card slides/animates from hand to register

- [ ] **Test**: Hand updates after card selection
  - **Steps**: Select 3 cards, check hand
  - **Expected**: 6 cards remaining in hand (or 3 empty slots)

#### 2.5 Undo (Backspace) Visual Feedback
- [ ] **Test**: Backspace removes card from register
  - **Steps**: Fill 3 registers, press Backspace
  - **Expected**: Register 3 becomes empty

- [ ] **Test**: Backspace returns card to hand
  - **Steps**: Press Backspace, check hand
  - **Expected**: Card reappears in hand area

- [ ] **Test**: Multiple Backspace clears all registers
  - **Steps**: Fill 5 registers, press Backspace 5 times
  - **Expected**: All registers empty, all cards back in hand

- [ ] **Test**: Backspace animation is smooth
  - **Steps**: Press Backspace, watch transition
  - **Expected**: Card slides/animates from register back to hand

#### 2.6 Execution Visual Feedback
- [ ] **Test**: Current register highlighted during execution
  - **Steps**: Execute 5 cards, watch registers
  - **Expected**: Register 1 highlights during register 1, then 2, etc.

- [ ] **Test**: Executed cards marked or removed
  - **Steps**: Execute 5 cards
  - **Expected**: Cards gray out, fade, or disappear as they execute

- [ ] **Test**: Registers clear after execution completes
  - **Steps**: Execute all 5 registers
  - **Expected**: All registers empty when returning to Programming

#### 2.7 Layout and Positioning
- [ ] **Test**: Deck doesn't block robot view
  - **Steps**: View robot on grid with deck visible
  - **Expected**: Deck positioned at bottom/side, robot clearly visible

- [ ] **Test**: Deck scales with screen resolution
  - **Steps**: Change resolution (1280x720, 1920x1080, 2560x1440)
  - **Expected**: Deck scales appropriately, maintains layout

- [ ] **Test**: Deck readable on small screens
  - **Steps**: Test at 1280x720
  - **Expected**: Cards and text still legible

---

### 3. WBP_HUD (Main Heads-Up Display)

#### 3.1 Health Display
- [ ] **Test**: Health bar visible on screen
  - **Steps**: Play in PIE, check HUD
  - **Expected**: Health bar displayed (position: top-left recommended)

- [ ] **Test**: Health bar shows current damage visually
  - **Steps**: Take 3 damage, check bar
  - **Expected**: Bar 70% full (7/10 health remaining)

- [ ] **Test**: Health bar updates in real-time
  - **Steps**: Stand on laser for 5 registers, watch bar
  - **Expected**: Bar decreases visually as damage taken

- [ ] **Test**: Health bar color indicates danger
  - **Steps**: Take 8 damage, check bar
  - **Expected**: Bar turns red/orange at low health

- [ ] **Test**: Health bar shows numeric value
  - **Steps**: Check health display
  - **Expected**: Shows "7/10" or "70%" or "Damage: 3"

- [ ] **Test**: Health bar resets on respawn
  - **Steps**: Die and respawn, check bar
  - **Expected**: Bar returns to 100% (0 damage)

#### 3.2 Lives Display
- [ ] **Test**: Lives counter visible on screen
  - **Steps**: Play in PIE, check HUD
  - **Expected**: Lives displayed (position: near health recommended)

- [ ] **Test**: Lives shows starting value (3)
  - **Steps**: Check lives at game start
  - **Expected**: Shows "Lives: 3" or 3 heart icons

- [ ] **Test**: Lives decrements on death
  - **Steps**: Die once, check lives
  - **Expected**: Shows "Lives: 2" or 2 heart icons

- [ ] **Test**: Lives reaches 0 on game over
  - **Steps**: Die 3 times, check lives
  - **Expected**: Shows "Lives: 0" or 0 heart icons

- [ ] **Test**: Lives uses icons (hearts/robots) or text
  - **Steps**: View lives display
  - **Expected**: Clear visual representation (icons preferred)

#### 3.3 Game Phase Display
- [ ] **Test**: Game phase shown on screen
  - **Steps**: Check HUD during Programming
  - **Expected**: Shows "Programming" or "Programming Phase"

- [ ] **Test**: Phase updates to Executing
  - **Steps**: Press E, check HUD
  - **Expected**: Shows "Executing" or "Executing Phase"

- [ ] **Test**: Phase updates to GameOver
  - **Steps**: Win or lose, check HUD
  - **Expected**: Shows "Game Over" or "Victory"/"Defeat"

- [ ] **Test**: Phase text has distinct visual style
  - **Steps**: View phase display
  - **Expected**: Large, readable text, possibly color-coded

- [ ] **Test**: Phase changes are animated (optional)
  - **Steps**: Press E, watch transition
  - **Expected**: Text fades or slides when changing

#### 3.4 Checkpoint Progress Display
- [ ] **Test**: Checkpoint counter visible on screen
  - **Steps**: Play in PIE, check HUD
  - **Expected**: Shows "Checkpoints: 0/5" (or total count)

- [ ] **Test**: Counter updates on checkpoint collection
  - **Steps**: Collect checkpoint 1, check HUD
  - **Expected**: Shows "Checkpoints: 1/5"

- [ ] **Test**: Counter shows all checkpoints collected
  - **Steps**: Collect all checkpoints, check HUD
  - **Expected**: Shows "Checkpoints: 5/5" or "Complete"

- [ ] **Test**: Checkpoint icons/visual indicators (optional)
  - **Steps**: View checkpoint display
  - **Expected**: Icons or flags show collected vs remaining

- [ ] **Test**: Counter positioned clearly
  - **Steps**: View HUD
  - **Expected**: Counter visible but not blocking view

#### 3.5 Event Log / Feedback Messages
- [ ] **Test**: Event log displays messages
  - **Steps**: Take damage, check log
  - **Expected**: Message appears: "Laser damage: 1"

- [ ] **Test**: Log shows wrong checkpoint feedback
  - **Steps**: Try to collect checkpoint out of order
  - **Expected**: Message appears: "Wrong checkpoint! Collect checkpoint 1 first"

- [ ] **Test**: Log shows death messages
  - **Steps**: Die, check log
  - **Expected**: Message appears: "Robot destroyed!" or "Respawning..."

- [ ] **Test**: Log shows win/lose messages
  - **Steps**: Win or lose, check log
  - **Expected**: Message appears: "Victory!" or "Game Over"

- [ ] **Test**: Log messages fade or scroll
  - **Steps**: Trigger multiple messages
  - **Expected**: Old messages fade/scroll away, new messages appear

- [ ] **Test**: Log doesn't block important UI
  - **Steps**: View log with many messages
  - **Expected**: Log positioned to side/bottom, doesn't cover cards

#### 3.6 HUD Layout and Positioning
- [ ] **Test**: All HUD elements visible simultaneously
  - **Steps**: Play game with all systems active
  - **Expected**: Health, lives, phase, checkpoints, log all visible

- [ ] **Test**: HUD elements don't overlap
  - **Steps**: Check all UI positions
  - **Expected**: No overlapping elements, clear layout

- [ ] **Test**: HUD scales with resolution
  - **Steps**: Change resolution
  - **Expected**: HUD elements maintain proportions and readability

- [ ] **Test**: HUD readable at all resolutions
  - **Steps**: Test at 1280x720, 1920x1080, 2560x1440
  - **Expected**: Text and icons remain legible

- [ ] **Test**: HUD doesn't block gameplay view
  - **Steps**: Play full turn, check visibility
  - **Expected**: Robot, grid, and hazards clearly visible

#### 3.7 HUD Visual Polish
- [ ] **Test**: HUD has consistent visual style
  - **Steps**: View all HUD elements
  - **Expected**: Consistent fonts, colors, borders

- [ ] **Test**: HUD uses game's color scheme
  - **Steps**: Compare HUD to game visuals
  - **Expected**: HUD matches game's art style

- [ ] **Test**: HUD elements have backgrounds/borders
  - **Steps**: View HUD elements
  - **Expected**: Panels have visible backgrounds for readability

- [ ] **Test**: HUD text has contrast/readability
  - **Steps**: View HUD on light and dark backgrounds
  - **Expected**: Text readable in all situations (drop shadow/outline)

---

### 4. Mouse Interaction (Optional Feature)

#### 4.1 Mouse Selection (if implemented)
- [ ] **Test**: Click card in hand to select
  - **Steps**: Click card with mouse
  - **Expected**: Card moves to first empty register

- [ ] **Test**: Hover highlights card
  - **Steps**: Hover mouse over card
  - **Expected**: Card highlights or glows

- [ ] **Test**: Click register to remove card
  - **Steps**: Click filled register
  - **Expected**: Card returns to hand (undo)

#### 4.2 Drag-and-Drop (if implemented)
- [ ] **Test**: Drag card from hand to register
  - **Steps**: Click and drag card
  - **Expected**: Card follows mouse, snaps to register

- [ ] **Test**: Drag card from register to hand (undo)
  - **Steps**: Drag card from register
  - **Expected**: Card returns to hand

- [ ] **Test**: Drag card between registers (reorder)
  - **Steps**: Drag card from register 1 to register 3
  - **Expected**: Card swaps positions or shifts registers

- [ ] **Test**: Drop card outside valid area cancels drag
  - **Steps**: Drag card, release outside UI
  - **Expected**: Card returns to original position

- [ ] **Test**: Visual feedback during drag
  - **Steps**: Drag card
  - **Expected**: Card follows cursor, registers highlight when valid drop target

---

### 5. Integration with C++ Systems

#### 5.1 Data Binding
- [ ] **Test**: WBP_HUD receives damage updates from ARobotPawn
  - **Steps**: Take damage, check HUD updates
  - **Expected**: Health bar updates in real-time

- [ ] **Test**: WBP_HUD receives lives updates from ARobotPawn
  - **Steps**: Die, check lives counter
  - **Expected**: Lives counter decrements immediately

- [ ] **Test**: WBP_HUD receives game state from ARobotRallyGameMode
  - **Steps**: Change state (Programming → Executing)
  - **Expected**: Phase display updates immediately

- [ ] **Test**: WBP_HUD receives checkpoint updates from ARobotPawn
  - **Steps**: Collect checkpoint, check HUD
  - **Expected**: Checkpoint counter updates immediately

- [ ] **Test**: WBP_ProgrammingDeck receives hand data from GameMode
  - **Steps**: Enter Programming, check cards
  - **Expected**: Cards match PlayerHand array in GameMode

- [ ] **Test**: WBP_ProgrammingDeck receives register data from GameMode
  - **Steps**: Program cards, check display
  - **Expected**: Register display matches Registers array in GameMode

#### 5.2 Input Integration
- [ ] **Test**: Keyboard input (1-9) still works with UI
  - **Steps**: Press 1-9 keys with UI active
  - **Expected**: Cards select into registers, UI updates

- [ ] **Test**: E key execution works with UI
  - **Steps**: Press E with UI active
  - **Expected**: Execution starts, deck hides

- [ ] **Test**: Backspace undo works with UI
  - **Steps**: Press Backspace with UI active
  - **Expected**: Card returns to hand, UI updates

- [ ] **Test**: Mouse input doesn't interfere with keyboard
  - **Steps**: Use keyboard and mouse together
  - **Expected**: Both input methods work correctly

#### 5.3 State Synchronization
- [ ] **Test**: UI shows correct state after respawn
  - **Steps**: Die and respawn, check UI
  - **Expected**: Health full, new hand dealt, registers empty

- [ ] **Test**: UI updates during card execution
  - **Steps**: Execute 5 cards, watch UI
  - **Expected**: Current register highlights as each executes

- [ ] **Test**: UI hidden/shown at correct times
  - **Steps**: Cycle through states (Programming → Executing → Programming)
  - **Expected**: Deck visible in Programming only, HUD always visible

---

### 6. Performance and Optimization

#### 6.1 Widget Performance
- [ ] **Test**: Dealing 9 cards doesn't cause lag
  - **Steps**: Enter Programming, measure framerate
  - **Expected**: No noticeable framerate drop

- [ ] **Test**: Card selection is responsive
  - **Steps**: Press 1-9 keys rapidly
  - **Expected**: Cards select immediately, no input lag

- [ ] **Test**: HUD updates don't cause lag
  - **Steps**: Take damage rapidly, watch framerate
  - **Expected**: Smooth performance, no stuttering

- [ ] **Test**: UI doesn't leak memory
  - **Steps**: Play for 10+ turns, check memory usage
  - **Expected**: Memory usage stable, no continuous growth

#### 6.2 Visual Performance
- [ ] **Test**: Animations are smooth (60 FPS)
  - **Steps**: Watch card selection animations
  - **Expected**: Smooth transitions, no jittering

- [ ] **Test**: Many widgets on screen perform well
  - **Steps**: Show deck (14 cards) + HUD elements
  - **Expected**: Maintains 60 FPS

---

### 7. Edge Cases and Error Handling

#### 7.1 UI Edge Cases
- [ ] **Test**: UI with 1-card hand (high damage)
  - **Steps**: Take 8 damage, enter Programming
  - **Expected**: UI shows 1 card, 8 empty slots, no errors

- [ ] **Test**: UI with empty registers
  - **Steps**: Don't program any cards
  - **Expected**: 5 empty register slots shown, no errors

- [ ] **Test**: UI with partial registers (3/5 filled)
  - **Steps**: Program 3 cards only
  - **Expected**: 3 filled, 2 empty slots shown

- [ ] **Test**: UI during rapid state changes
  - **Steps**: Quickly cycle states (if possible)
  - **Expected**: UI updates correctly, no visual glitches

- [ ] **Test**: UI with very high priority numbers
  - **Steps**: Display card with priority 840
  - **Expected**: Number fits in display area

#### 7.2 Resolution Edge Cases
- [ ] **Test**: UI at minimum resolution (1280x720)
  - **Steps**: Set resolution to 720p
  - **Expected**: All elements visible and readable

- [ ] **Test**: UI at 4K resolution (3840x2160)
  - **Steps**: Set resolution to 4K
  - **Expected**: UI scales correctly, not tiny

- [ ] **Test**: UI at ultrawide resolution (2560x1080)
  - **Steps**: Set ultrawide resolution
  - **Expected**: UI elements positioned correctly, no stretching

#### 7.3 State Edge Cases
- [ ] **Test**: UI during death/respawn
  - **Steps**: Die, watch UI during respawn
  - **Expected**: Smooth transition, no visual errors

- [ ] **Test**: UI during win screen
  - **Steps**: Win game, check UI
  - **Expected**: Victory message clear, deck hidden

- [ ] **Test**: UI during lose screen
  - **Steps**: Lose game, check UI
  - **Expected**: Game over message clear, deck hidden

---

## Testing Procedures

### Manual Testing Steps

1. **Setup**: Open RobotRally.uproject in UE5 Editor
2. **Create Widgets**: Create WBP_CardSlot, WBP_ProgrammingDeck, WBP_HUD in Content/RobotRally/UI/
3. **Configure GameMode**: Set HUD class to use WBP_HUD
4. **Play in Editor (PIE)**: Press Play button or Alt+P
5. **Test Interaction**: Use keyboard (WASD, 1-9, E, Backspace) and mouse

### Testing Workflow

#### Test Card Widgets
1. Deal hand (9 cards)
2. Verify all cards show action + priority
3. Verify cards visually distinct
4. Verify card size appropriate

#### Test Programming Deck
1. Enter Programming state
2. Verify deck appears
3. Press 1-5 to select cards
4. Verify cards move from hand to registers
5. Press Backspace to undo
6. Verify cards return to hand
7. Press E to execute
8. Verify deck hides during execution

#### Test HUD
1. Check health bar at 100%
2. Take damage → verify bar decreases
3. Die → verify lives decrement
4. Collect checkpoint → verify counter increases
5. Change state → verify phase display updates
6. Check event log for messages

### Visual Acceptance Criteria

- [ ] All UI elements follow consistent design language
- [ ] Text is readable on all backgrounds
- [ ] Colors are distinct and accessible
- [ ] Animations are smooth and non-distracting
- [ ] Layout is clear and uncluttered
- [ ] Important information (health, lives) is prominent

---

## Success Criteria

Phase 5 testing is considered **COMPLETE** when:

- ✅ All 120+ test cases above pass
- ✅ WBP_CardSlot displays all 7 card actions clearly
- ✅ WBP_ProgrammingDeck shows hand (up to 9) and registers (5)
- ✅ Card selection with 1-9 keys updates UI correctly
- ✅ Backspace undo returns cards to hand visually
- ✅ WBP_HUD displays health, lives, game phase, checkpoints
- ✅ Health bar updates in real-time with damage
- ✅ Lives counter decrements on death
- ✅ Checkpoint counter increments on collection
- ✅ Event log shows feedback messages
- ✅ UI visible/hidden in correct game states
- ✅ UI scales correctly across resolutions
- ✅ UI doesn't block gameplay view
- ✅ Performance remains smooth (60 FPS) with UI active
- ✅ UI integrates correctly with C++ backend systems

---

## Known Issues & Limitations

No known limitations for Phase 5 — all items are pending implementation.

### Design Decisions to Make

- **Card visual style**: Icons vs text? Color coding?
- **Drag-and-drop**: Implement or keyboard-only?
- **Mouse interaction**: Click to select or keyboard-only?
- **Animation complexity**: Simple fades or complex transitions?
- **Layout**: Deck at bottom or side? HUD layout arrangement?

---

## Next Steps After Phase 5 Testing

1. Fix any bugs found during testing
2. Polish UI animations and transitions
3. Proceed to Phase 6 (Content & Polish - levels, sound, VFX)
4. Consider accessibility features (colorblind mode, UI scale options)

---

## References

- [README.md](../../README.md) — Phase 5 implementation checklist
- [CLAUDE.md](../../CLAUDE.md) — Architecture and conventions
- [Phase 1 Testing Plan](./2026-02-08_phase1-testing-plan.md) — Movement system tests
- [Phase 2 Testing Plan](./2026-02-08_phase2-testing-plan.md) — Card system tests
- [Phase 3 Testing Plan](./2026-02-08_phase3-testing-plan.md) — Player control tests
- [Phase 4 Testing Plan](./2026-02-08_phase4-testing-plan.md) — Hazards tests
- Unreal Documentation:
  - UMG UI Designer: https://docs.unrealengine.com/5.7/en-US/umg-ui-designer-for-unreal-engine/
  - Widget Blueprints: https://docs.unrealengine.com/5.7/en-US/creating-widgets-in-unreal-engine/
- To Create:
  - `/Content/RobotRally/UI/WBP_CardSlot.uasset`
  - `/Content/RobotRally/UI/WBP_ProgrammingDeck.uasset`
  - `/Content/RobotRally/UI/WBP_HUD.uasset`
