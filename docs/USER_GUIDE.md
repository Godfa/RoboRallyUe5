# RobotRally User Guide

**Version**: 0.4 (Phase 3 Complete)
**Last Updated**: 2026-02-08

---

## Table of Contents

1. [Getting Started](#getting-started)
2. [Game Rules](#game-rules)
3. [Controls](#controls)
4. [Understanding the HUD](#understanding-the-hud)
5. [Card Types](#card-types)
6. [Tile Types](#tile-types)
7. [Tips & Strategies](#tips--strategies)
8. [Troubleshooting](#troubleshooting)

---

## Getting Started

### Installation

1. Clone or download the project to your computer
2. Double-click `RobotRally.uproject` to open in Unreal Engine 5.7
3. Wait for the editor to compile C++ code (first launch takes 2-3 minutes)
4. Press **Play (Alt+P)** to start the game

### First Game

When you press Play, you'll see:
- A 10×10 colored grid (the game board)
- A blue robot with a yellow arrow (your robot)
- An on-screen HUD showing your health, lives, and available cards

**Your goal**: Reach all checkpoints in order (1 → 2 → 3...) without running out of lives!

---

## Game Rules

### Win Condition
✅ **Collect all checkpoints in sequential order**

### Lose Condition
❌ **Run out of all 3 lives**

### How to Play

The game alternates between two phases:

#### 1. Programming Phase 🎯
- You have a hand of 9 cards (or fewer if damaged)
- Select 5 cards using number keys (1-9) to fill your registers
- Press **E** to execute your program

#### 2. Execution Phase ⚙️
- Your robot executes all 5 cards in order
- After each card, tile effects activate (conveyors, lasers, etc.)
- You cannot control the robot during execution
- Once all cards execute, you return to Programming Phase

### Health & Damage
- You start with **10 HP** and **3 lives**
- Taking damage reduces your hand size:
  - **10 HP** → 9 cards
  - **5 HP** → 8 cards (1 locked)
  - **0 HP** → Death, respawn with 5 cards minimum
- When you die, you respawn at the last checkpoint reached
- Each death consumes 1 life

### Checkpoints
- Checkpoints are **purple tiles with numbers**
- **Must** be reached in order (1 → 2 → 3...)
- Touching checkpoint out of order shows error message
- Each checkpoint becomes your new respawn point

---

## Controls

### Movement (Programming Phase Only)

| Key | Action |
|-----|--------|
| **W** | Move forward 1 tile |
| **S** | Move backward 1 tile |
| **A** | Turn left 90° |
| **D** | Turn right 90° |

*Note*: WASD is for scouting during Programming phase. These moves do NOT consume cards.

### Card Programming

| Key | Action |
|-----|--------|
| **1-9** | Select card from hand → add to next register |
| **Backspace** | Undo last card selection |
| **E** | Execute program (only when 5 registers filled) |

### Example Programming Session

```
Hand: [1]Move1 [2]Move2 [3]RotateR [4]RotateL [5]Move1 [6]Move3 [7]UTurn [8]Back [9]RotateR

Player presses: 1 → 3 → 1 → 1 → 6

Registers: [Move1] [RotateR] [Move1] [Move1] [Move3]
           ^reg0   ^reg1     ^reg2   ^reg3   ^reg4

Player presses: E

Robot executes:
  1. Move forward 1 tile
  2. Turn right 90°
  3. Move forward 1 tile
  4. Move forward 1 tile
  5. Move forward 3 tiles
```

---

## Understanding the HUD

### Top Bar
```
[Robot Rally]          State: Programming
```
- **State**: Current game phase (Programming / Executing / GameOver)

### Health Display
```
Health: ███████░░░ 7/10
Lives: ♥♥♥ (3)
Checkpoints: 2/4
```
- **Health Bar**: Visual HP indicator
- **Lives**: Remaining respawns (♥ = 1 life)
- **Checkpoints**: Progress toward victory (current/total)

### Card Display
```
Hand: [1] Move1-540 [2] Move2-710 [3] RotateR-230 ...
Registers: [Move1-540] [RotateR-230] [ ] [ ] [ ]
```
- **Hand**: Available cards with number keys
- **Registers**: Your programmed sequence (0-4)
- **Numbers after dash**: Card priority (higher = executes first in multi-robot games)

### Event Log
```
✓ Checkpoint 2 reached!
⚠ Robot took 1 damage from laser
→ Moved by conveyor
```
- **Green text**: Positive events (checkpoints, wins)
- **Red text**: Damage, death, errors
- **White text**: General info (movement, conveyors)
- Messages fade after 5 seconds

---

## Card Types

### Movement Cards

| Card | Action | Effect | Priority Range |
|------|--------|--------|----------------|
| **Move1** | ![→](arrow) | Move 1 tile forward | 490-660 |
| **Move2** | ![→→](arrows) | Move 2 tiles forward | 670-780 |
| **Move3** | ![→→→](arrows) | Move 3 tiles forward | 790-840 (HIGHEST) |
| **Back** | ![←](arrow) | Move 1 tile backward | 430-480 |

### Rotation Cards

| Card | Action | Effect | Priority Range |
|------|--------|--------|----------------|
| **RotateRight** | ↻ | Turn 90° clockwise | 80-420 |
| **RotateLeft** | ↺ | Turn 90° counter-clockwise | 70-410 |
| **UTurn** | ⟲ | Turn 180° | 10-60 (LOWEST) |

### Card Priority Explained

In multiplayer (not yet implemented), **higher priority cards execute first** each register.

Example:
```
Robot A: Move1-540
Robot B: Move1-490

Execution order: A moves, then B moves
```

*Currently*, priority doesn't matter since there's only one robot.

---

## Tile Types

### Safe Tiles

#### Normal Tile (Gray)
- No effect
- Safe to land on

### Hazard Tiles

#### Pit (Black)
- **Effect**: Instant death (10 damage)
- **When**: Immediately upon entering
- **Result**: Robot dies, respawns at last checkpoint
- **Tip**: Cannot be moved onto (movement is blocked)

#### Laser (Orange)
- **Effect**: 1 damage
- **When**: After each card execution if standing on it
- **Result**: Health decreases by 1
- **Tip**: Avoid staying on lasers for multiple turns

### Movement Tiles

#### Conveyor Belt (Blue with arrow)
- **Effect**: Pushes robot 1 tile in arrow direction
- **When**: After each card execution
- **Chain**: Can push through multiple conveyors
- **Tip**: Use conveyors as shortcuts or traps

##### Conveyor Directions
- **North** (↑): Pushes toward +X
- **South** (↓): Pushes toward -X
- **East** (→): Pushes toward +Y
- **West** (←): Pushes toward -Y

### Objective Tiles

#### Checkpoint (Purple with number)
- **Effect**: Registers checkpoint progress
- **When**: Immediately upon touching
- **Rule**: Must touch in order (1 → 2 → 3...)
- **Result**: Saves respawn point, updates HUD
- **Victory**: Touching last checkpoint wins the game

---

## Tips & Strategies

### Beginner Tips

1. **Scout first**: Use WASD to explore the board before programming
2. **Plan your route**: Look for checkpoint locations and hazards
3. **Count tiles**: Make sure your Move2/Move3 cards don't overshoot
4. **Use conveyors**: They move you for free after each card
5. **Save lives**: Each checkpoint is a safety net—reach them early

### Advanced Strategies

#### Conveyor Chaining
```
Example board:
[Start] → [ConveyorN] → [ConveyorN] → [Checkpoint]

Strategy:
- Program: [Move1] (step onto first conveyor)
- Result: Conveyor pushes you 2 tiles → reach checkpoint!
- Saved 2 cards!
```

#### Damage Management
```
Health: 7/10 → Hand size = 8 cards

Strategy:
- Avoid lasers when possible
- Each 1 damage reduces hand by 1 card at 5+ damage
- Death resets health but costs 1 life
- Sometimes dying is faster than healing (not yet implemented)
```

#### Register Planning
```
Bad program:
[Move1] [Move1] [RotateR] [Move1] [Move1]
→ Moves 2 tiles, turns, moves 2 tiles
→ Might hit hazard between turns!

Good program:
[Move1] [RotateR] [Move2] [ ... ]
→ Turn early, then move in safe direction
→ Fewer chances to hit hazards mid-sequence
```

---

## Troubleshooting

### "I pressed E but nothing happens"
- **Cause**: Not all 5 registers are filled
- **Solution**: Select 5 cards (press number keys 1-9 until all registers full)

### "WASD doesn't work during execution"
- **Expected behavior**: Manual movement only allowed in Programming phase
- **Wait** for all 5 cards to execute, then you can move again

### "I touched the checkpoint but nothing happened"
- **Cause**: You touched checkpoints out of order
- **Solution**: Check event log for error message, touch checkpoints in sequence

### "My robot fell off the map"
- **Cause**: Tried to move off grid edge
- **Solution**: Movement is blocked at edges—you should see "invalid move" warning

### "Game froze during execution"
- **Possible cause**: Movement interpolation stuck
- **Solution**: Restart PIE session (Escape → Play again)
- **Report**: This is a known bug—please report in GitHub issues

### "Cards don't appear in HUD"
- **Cause**: HUD not initializing properly
- **Solution**: Check that `RobotRallyHUD` is set as HUD class in GameMode

### "Robot moves through walls/pits"
- **Expected**: Pits block movement (shows warning)
- **Walls not implemented**: Collision system coming in Phase 6

---

## Keyboard Reference Card

```
┌─────────────────────────────────────┐
│      ROBOTRALLY CONTROLS            │
├─────────────────────────────────────┤
│  MOVEMENT (Programming Phase Only)  │
│    W        Move Forward            │
│   A S D     Turn Left/Back/Right    │
│                                     │
│  CARD PROGRAMMING                   │
│   1-9       Select Card             │
│   Backspace Undo Selection          │
│   E         Execute Program         │
│                                     │
│  GAME CONTROLS                      │
│   Escape    Pause / Stop Play       │
│   Alt+P     Play (in editor)        │
│   F11       Fullscreen              │
└─────────────────────────────────────┘
```

---

## What's Next?

### Coming in Future Phases

- **Phase 5**: UMG widget-based UI (mouse-driven card selection)
- **Phase 6**: More tile types (walls, crushers, repair stations)
- **Multiplayer**: 2-8 robots executing simultaneously
- **AI opponents**: Programmed robots to race against
- **Custom boards**: Level editor and board designer
- **Sound & VFX**: Audio feedback and particle effects

### Give Feedback

Found a bug or have a suggestion?
- GitHub Issues: [github.com/YourRepo/RobotRallyUE5/issues](https://github.com)
- Discord: (coming soon)

---

## Credits

**RobotRally Board Game**: Original game by Richard Garfield (Wizards of the Coast)
**UE5 Implementation**: Robot Rally Team (2026)
**Engine**: Unreal Engine 5.7 (Epic Games)

---

**Happy Robot Programming!** 🤖✨
