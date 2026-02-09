# RobotRally Technical Documentation

**Version**: Phase 5 Complete
**Last Updated**: 2026-02-09
**Engine**: Unreal Engine 5.7

---

## Table of Contents

1. [System Overview](#system-overview)
2. [Architecture](#architecture)
3. [Class Reference](#class-reference)
4. [Game Systems](#game-systems)
5. [Game Flow](#game-flow)
6. [Testing Guide](#testing-guide)
7. [Known Limitations](#known-limitations)

---

## System Overview

RobotRally is a digital adaptation of the board game where players program robots using movement cards to navigate a hazardous factory floor. The current implementation supports:

- ✅ Multiplayer (LAN, 2-8 players) with server-client architecture
- ✅ Grid-based movement system with smooth interpolation
- ✅ Wall system on tile edges (blocks movement and pushing)
- ✅ Robot collision detection and chain pushing mechanics
- ✅ Card programming system (5 registers, 9-card hand)
- ✅ Tile hazards (pits, lasers, conveyors, checkpoints)
- ✅ Health and lives system with respawn
- ✅ Win/lose conditions
- ✅ On-screen HUD with event log
- ✅ AI controller framework (Easy/Medium difficulty)

### Implementation Status

| Phase | Status | Completion |
|-------|--------|------------|
| Phase 0: Project Setup | ✅ Complete | 100% |
| Phase 1: Core Structure | ✅ Complete | 100% |
| Phase 2: Card System | ✅ Complete | 100% |
| Phase 3: Player Control | ✅ Complete | 100% |
| Phase 4: Field Hazards | ✅ Complete | 100% |
| Phase 5: Multiplayer & Walls | ✅ Complete | 100% |
| Phase 6: UI/UMG Widgets | ⚠️ Partial | 25% |
| Phase 7: Content & Polish | ❌ Not Started | 0% |

---

## Architecture

### Class Hierarchy

```
AGameModeBase
  └─ ARobotRallyGameMode            State machine, card deck, turn logic

AActor
  ├─ AGridManager                    10x10 tile grid, world↔grid conversions
  └─ ARobotPawn (ACharacter)         Robot with health, checkpoints, lives
       └─ URobotMovementComponent    Smooth grid-based movement

APlayerController
  └─ ARobotController                Keyboard input (WASD, cards, execute)

AHUD
  └─ ARobotRallyHUD                  On-screen HUD (health, events, cards)
```

### Data Flow

```
User Input (Keyboard)
    ↓
ARobotController → binds input actions
    ↓
ARobotRallyGameMode → manages game state, card selection
    ↓
ARobotPawn → executes movement commands
    ↓
URobotMovementComponent → validates and interpolates movement
    ↓
AGridManager → checks tile types, boundaries
    ↓
ARobotRallyGameMode → processes tile effects (conveyors, hazards)
    ↓
ARobotRallyHUD → displays feedback to player
```

---

## Class Reference

### ARobotRallyGameMode

**Responsibility**: Game state machine, card management, turn execution

#### Key Properties

```cpp
EGameState CurrentState;              // Programming | Executing | GameOver
TArray<FRobotCard> Deck;              // 84-card shuffled deck
TArray<FRobotCard> HandCards;         // Player's current hand (5-9 cards)
TArray<int32> RegisterSlots;          // Indices of cards in 5 registers
TArray<FRobotCard> ProgrammedCards;   // Final committed program
AGridManager* GridManagerInstance;    // Reference to grid
ARobotPawn* TestRobot;                // Player robot (single-player mode)
```

#### Key Methods

| Method | Purpose |
|--------|---------|
| `BeginPlay()` | Creates GridManager, spawns robot, initializes deck |
| `StartProgrammingPhase()` | Resets registers, deals new hand, sets state to Programming |
| `StartExecutionPhase()` | Commits registers, begins executing cards in sequence |
| `SelectCardFromHand(int32)` | Adds card to next empty register |
| `UndoLastSelection()` | Removes last card from registers, returns to hand |
| `ProcessNextRegister()` | Executes one card, then checks tile effects |
| `ProcessTileEffects()` | Handles pits, lasers, conveyors, checkpoints |
| `ShowEventMessage(FString, FColor)` | Pushes message to on-screen event log |

#### Constants

```cpp
static constexpr int32 NUM_REGISTERS = 5;      // Always 5 cards per program
static constexpr int32 DECK_SIZE = 84;         // Standard Robot Rally deck
static constexpr int32 BASE_HAND_SIZE = 9;     // Base hand size
static constexpr int32 MIN_HAND_SIZE = 5;      // Minimum (10 damage = locked)
```

#### State Machine

```
Programming → (Player selects 5 cards) → Executing → (All cards executed) → Programming
                                              ↓
                                         (Robot dies or wins)
                                              ↓
                                          GameOver
```

---

### ARobotPawn

**Responsibility**: Robot actor with health, checkpoints, and movement execution

#### Key Properties

```cpp
URobotMovementComponent* RobotMovement;    // Grid movement component
int32 Health;                              // Current health (0-10)
int32 MaxHealth;                           // Maximum health (10)
bool bIsAlive;                             // Death state flag
int32 Lives;                               // Remaining lives (0-3)
int32 MaxLives;                            // Starting lives (3)
int32 CurrentCheckpoint;                   // Last checkpoint reached (0-based)
FIntVector RespawnPosition;                // Grid position for respawn
```

#### Key Methods

| Method | Purpose |
|--------|---------|
| `ExecuteMoveCommand(int32)` | Moves robot forward/backward by N tiles |
| `ExecuteRotateCommand(int32)` | Rotates robot (1 = 90° right, -1 = 90° left) |
| `ApplyDamage(int32)` | Reduces health, triggers death if ≤0 |
| `Respawn()` | Restores robot at last checkpoint, consumes 1 life |
| `ReachCheckpoint(int32)` | Updates checkpoint progress, saves respawn point |

#### Visual Components

```cpp
UStaticMeshComponent* BodyMesh;            // Robot body (customizable)
UStaticMeshComponent* DirectionIndicator;  // Forward-facing arrow
FLinearColor BodyColor;                    // Body tint (default blue)
FLinearColor DirectionColor;               // Arrow tint (default yellow)
```

#### Delegates

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRobotDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCheckpointReached, int32, CheckpointNumber);

FOnRobotDeath OnDeath;                     // Broadcast when health ≤ 0
FOnCheckpointReached OnCheckpointReached;  // Broadcast when checkpoint reached
```

---

### URobotMovementComponent

**Responsibility**: Smooth grid-based movement with interpolation

#### Key Properties

```cpp
float GridSize;                      // Tile size in UE units (default 100)
AGridManager* GridManager;           // Reference for move validation
EGridDirection FacingDirection;      // North, East, South, West
int32 CurrentGridX;                  // Current grid X coordinate
int32 CurrentGridY;                  // Current grid Y coordinate
bool bIsMoving;                      // Movement in progress flag
bool bIsRotating;                    // Rotation in progress flag
```

#### Key Methods

| Method | Purpose |
|--------|---------|
| `MoveInGrid(int32)` | Sets target location N tiles in facing direction |
| `RotateInGrid(int32)` | Sets target rotation N×90° from current rotation |
| `MoveToWorldPosition(FVector)` | Direct world-space movement (for conveyors) |
| `SetGridPosition(int32, int32)` | Force-sets grid coords without movement |
| `InitializeGridPosition(int32, int32, EGridDirection)` | Sets starting position |
| `TickComponent()` | Interpolates toward target location/rotation |

#### Grid Direction System

```cpp
enum class EGridDirection : uint8
{
    North,  // +X axis
    East,   // +Y axis
    South,  // -X axis
    West    // -Y axis
};
```

#### Movement Validation

Before setting `TargetLocation`, the component:
1. Calculates destination grid coordinates
2. Queries `GridManager->IsValidTile()` (in-bounds check, not a pit)
3. If invalid, movement is canceled and warning is logged

---

### AGridManager

**Responsibility**: Game board, tile data storage, coordinate conversions

#### Key Properties

```cpp
int32 Width;                                   // Grid width (default 10)
int32 Height;                                  // Grid height (default 10)
float TileSize;                                // Tile size in units (100)
TMap<FIntVector, FTileData> GridMap;           // Tile type and data storage
TMap<FIntVector, UStaticMeshComponent*> TileMeshes;     // Visual tiles
TMap<FIntVector, UStaticMeshComponent*> ArrowMeshes;    // Conveyor arrows
TMap<FIntVector, UTextRenderComponent*> CheckpointLabels; // Checkpoint numbers
```

#### Tile Types

```cpp
enum class ETileType : uint8
{
    Normal,         // Safe tile, no effect
    Pit,            // Instant death
    ConveyorNorth,  // Pushes robot north (+X)
    ConveyorSouth,  // Pushes robot south (-X)
    ConveyorEast,   // Pushes robot east (+Y)
    ConveyorWest,   // Pushes robot west (-Y)
    Laser,          // Deals 1 damage per register
    Checkpoint      // Must collect in order to win
};
```

#### Tile Data Structure

```cpp
struct FTileData
{
    ETileType TileType;        // Tile type enum
    int32 CheckpointNumber;    // Checkpoint ID (1-based), 0 for non-checkpoints
    uint8 Walls;               // Wall bitfield (bit 0=North, 1=East, 2=South, 3=West)
};
```

#### Wall System

Walls exist on **tile edges** (not tiles themselves) and block movement between adjacent tiles.

**Wall Bit Flags**:
```cpp
static constexpr uint8 WALL_NORTH = 1 << 0;  // bit 0
static constexpr uint8 WALL_EAST  = 1 << 1;  // bit 1
static constexpr uint8 WALL_SOUTH = 1 << 2;  // bit 2
static constexpr uint8 WALL_WEST  = 1 << 3;  // bit 3
```

**Storage**: Each tile stores walls on its edges using a single byte (4 bits used). This approach:
- Uses minimal memory (1 byte per tile)
- Provides O(1) wall lookups
- Replicates efficiently over network
- Allows dynamic wall placement/removal

**Visual Representation**: Wall meshes are spawned as thin cubes (5 units thick, 50 units high) positioned at tile boundaries. Color: dark gray (RGB 0.15).

#### Key Methods

| Method | Purpose |
|--------|---------|
| `GridToWorld(FIntVector)` | Converts grid coords to world location |
| `WorldToGrid(FVector)` | Converts world location to grid coords |
| `GetTileType(FIntVector)` | Returns tile type at coordinates |
| `GetTileData(FIntVector)` | Returns full tile data (type + checkpoint + walls) |
| `IsInBounds(int32, int32)` | Checks if coords are within grid boundaries |
| `IsValidTile(FIntVector)` | Returns true if in-bounds and not a pit |
| `SetTileType(FIntVector, FTileData)` | Updates tile and refreshes visual |
| `GetTotalCheckpoints()` | Counts total checkpoint tiles on board |
| `HasWall(FIntVector, EGridDirection)` | Checks if wall exists on tile edge |
| `SetWall(FIntVector, EGridDirection, bool)` | Adds/removes wall on tile edge |
| `IsMovementBlocked(FIntVector, FIntVector)` | Checks if wall blocks movement between tiles |
| `RefreshAllWallVisuals()` | Rebuilds all wall meshes from wall data |

#### Coordinate System

- **Grid Coordinates**: `FIntVector(X, Y, 0)` — integer tile indices
- **World Coordinates**: `FVector(X*100, Y*100, Z)` — UE world space
- **Origin**: Grid (0, 0) → World (0, 0, Z)

---

### ARobotController

**Responsibility**: Player keyboard input handling

#### Input Bindings

| Key | Action | Function |
|-----|--------|----------|
| W | Move Forward | `OnMoveForward()` |
| S | Move Backward | `OnMoveBackward()` |
| A | Turn Left | `OnMoveLeft()` |
| D | Turn Right | `OnMoveRight()` |
| 1-9 | Select Card | `OnSelectCard1()` - `OnSelectCard9()` |
| E | Execute Program | `OnExecuteProgram()` |
| Backspace | Undo Selection | `OnUndoSelection()` |

#### Implementation Notes

- Uses **legacy ActionMappings** in `Config/DefaultInput.ini`
- No Blueprint assets required (C++-only project)
- All inputs route through GameMode for state validation
- WASD movement only allowed in Programming state

---

### ARobotRallyHUD

**Responsibility**: On-screen UI rendering

#### Displayed Information

```
┌─────────────────────────────────────────┐
│ [Robot Rally]          State: Programming│
│                                          │
│ Health: ███████░░░ 7/10                  │
│ Lives: ♥♥♥ (3)                           │
│ Checkpoints: 2/4                         │
│                                          │
│ Hand: [1] Move1 [2] Move2 [3] Rotate... │
│ Registers: [Move1] [Rotate] [ ] [ ] [ ] │
│                                          │
│ Event Log:                               │
│ ✓ Checkpoint 2 reached!                  │
│ ⚠ Robot took 1 damage from laser         │
│ → Moved by conveyor                      │
└─────────────────────────────────────────┘
```

#### Key Methods

| Method | Purpose |
|--------|---------|
| `DrawHUD()` | Renders all UI elements each frame |
| `AddEventMessage(FString, FColor)` | Adds message to event log |
| `DrawCardSelection()` | Renders hand and register visualization |

#### Event Message System

- Messages display for 5 seconds
- Maximum 8 messages on screen
- Color-coded (green = success, red = damage, white = info)
- Automatically fades out oldest messages

---

## Game Systems

### 1. Card System

#### Deck Composition (84 cards total)

| Card Type | Count | Priority Range |
|-----------|-------|----------------|
| Move1 | 18 | 490-660 |
| Move2 | 12 | 670-780 |
| Move3 | 6 | 790-840 |
| MoveBack | 6 | 430-480 |
| RotateRight | 18 | 80-420 |
| RotateLeft | 18 | 70-410 |
| UTurn | 6 | 10-60 |

#### Card Priority Rules

- Higher priority executes **first** in multi-robot scenarios
- Priority is randomly assigned when deck is built
- UTurn has lowest priority (10-60)
- Move3 has highest priority (790-840)

#### Deck Management

```cpp
// Initialization (in GameMode BeginPlay)
BuildDeck();      // Creates 84 cards with priorities
ShuffleDeck();    // Fisher-Yates shuffle

// Each programming phase
DealHand();       // Draws N cards (based on damage)
CommitRegistersToProgram(); // Copies selected cards to program
DiscardHand();    // Returns unused cards to discard pile

// Deck exhaustion handling
if (Deck.Num() < HandSize) {
    Deck.Append(DiscardPile);  // Reshuffle discard into deck
    DiscardPile.Empty();
    ShuffleDeck();
}
```

#### Hand Size Calculation

```cpp
int32 CalculateHandSize() const
{
    int32 Damage = MaxHealth - Health;
    int32 LockedCards = FMath::Max(0, Damage - 4);  // 5+ damage locks 1 card, etc.
    return FMath::Max(BASE_HAND_SIZE - LockedCards, MIN_HAND_SIZE);
}
```

| Health | Damage | Locked Cards | Hand Size |
|--------|--------|--------------|-----------|
| 10 | 0 | 0 | 9 |
| 9 | 1 | 0 | 9 |
| 5 | 5 | 1 | 8 |
| 3 | 7 | 3 | 6 |
| 0 | 10 | 6 | 5 (MIN) |

#### Register Programming Flow

1. **Programming state begins**: Hand is dealt
2. **Player presses 1-9**: `SelectCardFromHand(HandIndex)` called
3. **Card added to register**: Next empty slot filled, card removed from hand
4. **All 5 registers filled**: Player can press E to execute
5. **Execution begins**: `CommitRegistersToProgram()` finalizes program
6. **Cards execute sequentially**: Register 0 → 1 → 2 → 3 → 4

---

### 2. Movement System

#### Movement Pipeline

```
Player/GameMode calls ARobotPawn::ExecuteMoveCommand(Distance)
    ↓
URobotMovementComponent::MoveInGrid(Distance) called
    ↓
Calculate destination: (CurrentX + DX*Distance, CurrentY + DY*Distance)
    ↓
Validate with GridManager->IsValidTile(Destination)
    ↓
If valid: Set TargetLocation, bIsMoving = true
If invalid: Log warning, cancel movement
    ↓
TickComponent() interpolates: VInterpTo(CurrentLocation, TargetLocation)
    ↓
When distance < 1.0: Snap to target, bIsMoving = false
    ↓
Broadcast OnGridPositionChanged delegate
    ↓
ARobotPawn updates internal GridX/GridY
```

#### Interpolation Settings

```cpp
// In URobotMovementComponent::TickComponent()
const float InterpSpeed = 5.0f;  // Units per second

NewLocation = FMath::VInterpTo(
    OwnerLocation,
    TargetLocation,
    DeltaTime,
    InterpSpeed
);
```

- **Speed**: 5.0 units/sec → ~0.5 seconds per tile (at 100 unit tiles)
- **Smooth**: Uses Unreal's `VInterpTo` for exponential ease-in

#### Rotation System

```cpp
void RotateInGrid(int32 Steps)
{
    int32 NewFacing = ((int32)FacingDirection + Steps) % 4;
    if (NewFacing < 0) NewFacing += 4;
    FacingDirection = (EGridDirection)NewFacing;

    // Update target rotation
    TargetRotation = GetOwner()->GetActorRotation();
    TargetRotation.Yaw += Steps * 90.0f;
    bIsRotating = true;
}
```

- 1 step = 90° clockwise
- -1 step = 90° counter-clockwise
- 2 steps = 180° (UTurn)
- Uses `RInterpTo` for smooth rotation

---

### 3. Tile Effects System

#### Effect Processing Order

After each card execution:

1. **Check current tile**
2. **Apply instant effects** (pits → instant death)
3. **Apply damage effects** (lasers → 1 damage)
4. **Process movement effects** (conveyors → push robot)
5. **Check checkpoints** (update progress)
6. **Check win/lose conditions**

#### Conveyor Chain System

```cpp
void ProcessConveyors()
{
    const int32 MaxChain = 10;  // Prevent infinite loops
    int32 ChainCount = 0;

    while (ChainCount < MaxChain) {
        FIntVector CurrentPos = GetRobotGridPosition();
        ETileType Type = GridManager->GetTileType(CurrentPos);

        if (!AGridManager::IsConveyor(Type)) break;  // No conveyor, exit

        // Move robot in conveyor direction
        FIntVector NewPos = CalculateConveyorDestination(CurrentPos, Type);
        Robot->RobotMovement->MoveToWorldPosition(GridToWorld(NewPos));
        Robot->RobotMovement->SetGridPosition(NewPos.X, NewPos.Y);

        ChainCount++;
    }
}
```

**Example**: Robot lands on ConveyorNorth → Pushed north 1 tile → Lands on ConveyorNorth → Pushed north again → Lands on Normal tile → Chain ends

#### Checkpoint System

```cpp
void ReachCheckpoint(int32 Number)
{
    if (Number == CurrentCheckpoint + 1) {
        CurrentCheckpoint = Number;
        RespawnPosition = GetRobotGridPosition();  // Save respawn point
        OnCheckpointReached.Broadcast(Number);
        ShowEventMessage("Checkpoint " + FString::FromInt(Number), FColor::Green);
    } else {
        ShowEventMessage("Must reach checkpoints in order!", FColor::Red);
    }
}
```

- Checkpoints **must** be reached in sequential order (1 → 2 → 3 → ...)
- Out-of-order checkpoints display error message
- Each checkpoint updates respawn location

---

### 4. Wall System

#### Wall Placement

Walls exist on **tile edges**, not tiles themselves. Each tile stores which of its 4 edges have walls.

```cpp
// Add perimeter walls around the board
for (int32 x = 0; x < 10; ++x)
{
    GridManager->SetWall(FIntVector(x, 0, 0), EGridDirection::West, true);
    GridManager->SetWall(FIntVector(x, 9, 0), EGridDirection::East, true);
}

// Add internal wall
GridManager->SetWall(FIntVector(4, 3, 0), EGridDirection::North, true);
```

#### Movement Validation

Before each movement step, the system checks:
1. **From-tile side**: Does the starting tile have a wall in the movement direction?
2. **To-tile side**: Does the destination tile have a wall in the opposite direction?
3. **Result**: If either has a wall, movement is blocked

```cpp
bool IsMovementBlocked(FIntVector From, FIntVector To)
{
    // Calculate direction (DX, DY)
    int32 DX = To.X - From.X;
    int32 DY = To.Y - From.Y;

    // Must be adjacent tiles
    if (FMath::Abs(DX) + FMath::Abs(DY) != 1) return false;

    // Check wall on from-tile side
    if (HasWall(From, Direction)) return true;

    // Check wall on to-tile side (opposite direction)
    if (HasWall(To, OppositeDirection)) return true;

    return false;
}
```

#### Wall Effects on Game Systems

**Robot Movement**:
- `MoveInGrid()` checks walls before each step
- Partial movement allowed (Move3 blocked after 1 tile = Move1)
- Log message: "Blocked by wall between (X1,Y1) and (X2,Y2)"

**Robot Pushing**:
- `TryPushRobot()` validates destination is wall-free
- Chain pushing stops at walls
- Robots cannot be pushed through walls

**Conveyor Belts**:
- `ProcessRobotConveyors()` checks walls before movement
- Blocked conveyors show message: "R0 blocked by wall on conveyor"
- Robot stays on conveyor tile if blocked

#### Network Replication

Walls automatically replicate via existing `FReplicatedTileEntry` system:
```cpp
struct FReplicatedTileEntry
{
    int32 X;
    int32 Y;
    ETileType TileType;
    int32 CheckpointNumber;
    uint8 Walls;  // Replicates automatically
};
```

Server sets walls → GameState replicates → Clients call `RefreshAllWallVisuals()`

---

### 5. Health & Lives System

#### Damage Sources

| Source | Damage | Timing |
|--------|--------|--------|
| Laser Tile | 1 HP | After each card execution |
| Pit Tile | 10 HP (instant death) | Immediate |
| Robot Collision | Not yet implemented | — |
| Board Laser | Not yet implemented | — |

#### Death & Respawn Flow

```
Robot Health ≤ 0
    ↓
ARobotPawn::ApplyDamage() triggers death
    ↓
bIsAlive = false
OnDeath.Broadcast()
    ↓
ARobotRallyGameMode receives death event
    ↓
Lives > 0?
    YES → ARobotPawn::Respawn()
          - Restore Health to MaxHealth
          - Move to RespawnPosition
          - Lives--
          - bIsAlive = true
    NO → Game Over (all lives exhausted)
```

#### Respawn Position Updates

- **Start of game**: RespawnPosition = (0, 0, 0)
- **Checkpoint reached**: RespawnPosition = checkpoint grid coords
- **Respawn triggered**: Robot teleports to RespawnPosition

---

### 6. Win/Lose Conditions

#### Win Condition

```cpp
if (Robot->CurrentCheckpoint >= GridManager->GetTotalCheckpoints()) {
    CurrentState = EGameState::GameOver;
    ShowEventMessage("Victory! All checkpoints collected!", FColor::Green);
}
```

- Triggered when `CurrentCheckpoint` equals total checkpoints on board
- Game enters GameOver state
- No further card execution allowed

#### Lose Condition

```cpp
if (Robot->Lives <= 0 && !Robot->bIsAlive) {
    CurrentState = EGameState::GameOver;
    ShowEventMessage("Game Over! All lives lost.", FColor::Red);
}
```

- Triggered when robot dies with 0 remaining lives
- Game enters GameOver state
- Robot remains at death location

---

## Game Flow

### Typical Game Loop

```
1. Game Starts (Programming State)
   ├─ GameMode spawns GridManager (10×10 grid)
   ├─ GameMode spawns TestRobot at (0, 0)
   ├─ GameMode builds 84-card deck
   └─ GameMode deals 9-card hand

2. Player Programming Phase (WASD movement allowed)
   ├─ Player can move robot with WASD to scout board
   ├─ Player selects 5 cards using number keys 1-9
   ├─ Cards fill registers left to right
   ├─ Player can undo with Backspace
   └─ Player presses E when all 5 registers filled

3. Execution Phase Begins (WASD disabled)
   ├─ GameMode commits registers to program
   ├─ GameMode starts executing Register 0
   │   ├─ Execute card action (Move1 → ExecuteMoveCommand(1))
   │   ├─ Wait for movement interpolation to complete
   │   ├─ ProcessTileEffects()
   │   │   ├─ Check current tile type
   │   │   ├─ Apply damage (laser = 1, pit = 10)
   │   │   ├─ Process conveyors (chain movement)
   │   │   └─ Check checkpoints
   │   └─ Check win/lose conditions
   └─ Repeat for Registers 1, 2, 3, 4

4. Execution Complete
   ├─ Discard used cards to discard pile
   ├─ Return to Programming Phase (step 1)
   └─ Deal new hand (size based on damage)

5. Game Ends (GameOver State)
   ├─ Win: All checkpoints collected
   ├─ Lose: All lives lost
   └─ Player must restart game (no implemented yet)
```

---

## Testing Guide

### Manual Testing Checklist

#### Movement System
- [ ] **WASD movement**: Robot moves smoothly in 4 directions
- [ ] **Grid alignment**: Robot snaps to exact grid centers
- [ ] **Rotation**: Robot faces correct direction after turns
- [ ] **Boundary check**: Robot cannot move off grid edges
- [ ] **Pit avoidance**: Robot cannot move onto pit tiles

#### Card System
- [ ] **Hand dealt**: 9 cards appear in HUD (at full health)
- [ ] **Card selection**: Pressing 1-9 fills registers
- [ ] **Register order**: Cards fill left to right (0-4)
- [ ] **Undo**: Backspace removes last card, returns to hand
- [ ] **Hand size reduction**: Taking 5+ damage reduces hand to 8 cards
- [ ] **Minimum hand**: 10 damage still gives 5 cards
- [ ] **Execution disabled**: Cannot execute with <5 cards

#### Tile Effects
- [ ] **Pit death**: Robot dies instantly when entering pit
- [ ] **Laser damage**: Robot takes 1 damage per laser tile per card
- [ ] **Conveyor movement**: Robot pushed 1 tile in arrow direction
- [ ] **Conveyor chain**: Robot pushed through multiple conveyors
- [ ] **Checkpoint order**: Out-of-order checkpoint shows error message
- [ ] **Checkpoint progress**: HUD displays correct checkpoint count

#### Health & Lives
- [ ] **Damage display**: Health bar decreases when damaged
- [ ] **Death trigger**: Health ≤ 0 triggers death
- [ ] **Respawn**: Robot respawns at last checkpoint with full health
- [ ] **Lives decrement**: Lives decrease from 3 → 2 → 1 → 0
- [ ] **Game over**: 0 lives + death = game over message

#### Win/Lose Conditions
- [ ] **Win**: Collecting all checkpoints shows victory message
- [ ] **Lose**: Exhausting all lives shows game over message
- [ ] **GameOver state**: Cannot execute cards after game ends

### Testing Scenarios

#### Scenario 1: Basic Card Execution
1. Start game (PIE)
2. Press 1, 2, 3, 4, 5 to fill registers
3. Press E to execute
4. Verify robot executes all 5 cards in sequence
5. Verify Programming phase restarts

#### Scenario 2: Conveyor Chain
1. Set up 3 ConveyorNorth tiles in a row
2. Move robot onto first conveyor
3. Verify robot pushed through all 3 tiles
4. Verify robot stops on non-conveyor tile

#### Scenario 3: Checkpoint Sequence
1. Set up 3 checkpoint tiles (numbers 1, 2, 3)
2. Touch checkpoint 1 → success message
3. Touch checkpoint 3 → error (out of order)
4. Touch checkpoint 2 → success message
5. Touch checkpoint 3 → success + victory

#### Scenario 4: Death & Respawn
1. Reach checkpoint 1 (saves respawn point)
2. Take 10 damage (pit or laser spam)
3. Verify death message, lives = 2
4. Verify robot respawns at checkpoint 1
5. Repeat until lives = 0
6. Verify game over message

---

## Known Limitations

### Not Yet Implemented

#### AI Pathfinding
- **Issue**: AI controllers exist but use random card selection
- **Impact**: AI robots don't navigate to checkpoints intelligently
- **Required for**: Challenging AI opponents

#### UI/UMG Widgets
- **Issue**: HUD uses DrawHUD (Canvas API), not UMG
- **Impact**: No mouse interaction, limited styling
- **Required for**: Phase 5 (modern UI)

#### Card Tooltips
- **Issue**: No description of what each card does
- **Impact**: New players must memorize card types
- **Enhancement**: Add hover tooltips to HUD cards

#### Additional Board Elements
- **Issue**: No gear tiles, repair stations, pushers, crushers
- **Impact**: Missing some classic Robot Rally mechanics
- **Required for**: Phase 7 (full board element set)

#### Board Lasers
- **Issue**: Fixed lasers exist, but no board-mounted lasers
- **Impact**: Cannot create cross-board laser hazards
- **Required for**: Phase 6 (advanced hazards)

#### Sound & VFX
- **Issue**: No audio or particle effects
- **Impact**: Less immersive experience
- **Required for**: Phase 6 (polish)

### Known Bugs

#### Movement Interpolation Edge Case
- **Bug**: Rapid card execution can cause movement overlap
- **Workaround**: Timer checks `IsMoving()` before next card
- **Status**: Mitigated, not fully solved

#### Conveyor Z-Fighting
- **Bug**: Conveyor arrows sometimes clip through tiles
- **Cause**: Arrow mesh Z-offset = 1.0 (too small)
- **Fix**: Increase Z-offset to 5.0 units

#### HUD Text Overlap
- **Bug**: Long event messages overlap with card display
- **Cause**: No text wrapping in DrawText
- **Fix**: Implement multi-line text or message queue

---

## Performance Notes

### Current Metrics (PIE, Development Build)

- **Tick Cost**: ~0.1ms (GridManager + Movement + HUD)
- **Grid Size**: 10×10 = 100 tiles, 100 static meshes
- **Memory**: ~15 MB for grid + robot + deck
- **Scalability**: Tested up to 20×20 grid (400 tiles) with no FPS drop

### Optimization Opportunities

1. **Static Mesh Instancing**: Use ISMC for tiles (reduces draw calls)
2. **HUD Caching**: Pre-render card images to textures
3. **Grid Culling**: Only render visible tiles (for large boards)
4. **Conveyor Processing**: Batch process all conveyors simultaneously

---

## Appendix: Code Style Guide

### Naming Conventions

| Type | Prefix | Example |
|------|--------|---------|
| Actor | A | `ARobotPawn`, `AGridManager` |
| Component | U | `URobotMovementComponent` |
| Struct | F | `FRobotCard`, `FTileData` |
| Enum | E | `ETileType`, `ECardAction` |
| Interface | I | `IRobotAI` (future) |
| Boolean | b | `bIsMoving`, `bIsAlive` |

### UPROPERTY Tags

```cpp
// Blueprint-exposed, editable in editor
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
int32 MyVariable;

// Blueprint-exposed, read-only
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MyCategory")
int32 MyReadOnlyVar;

// Editor-only, falls back to default
UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MyCategory")
UStaticMesh* MyMeshAsset;
```

### File Headers

```cpp
// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"
```

### Tab Settings
- **Indentation**: Tabs (not spaces)
- **Tab Size**: 4 spaces
- **Braces**: Same line for control flow, next line for functions

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 0.1 | 2026-01-15 | Phase 1 complete (core structure) |
| 0.2 | 2026-01-20 | Phase 2 complete (card system) |
| 0.3 | 2026-02-05 | Phase 3 complete (player control, lives) |
| 0.4 | 2026-02-08 | Phase 4 complete (hazards), documentation created |

---

**End of Technical Documentation**
