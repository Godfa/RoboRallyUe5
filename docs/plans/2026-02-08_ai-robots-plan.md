# AI-Driven Robots Implementation Plan

## Context

RobotRally currently supports only **player-controlled robots**. The game spawns 2 robots but only the first is controllable via keyboard (WASD movement, 1-9 card selection, E to execute). The second robot sits idle.

**Goal:** Implement AI-controlled opponent robots that autonomously select movement cards and compete against the player. The AI should use pathfinding to navigate toward checkpoints, avoid hazards (pits, lasers), and automatically progress the game when ready.

**Why this matters:**
- Enables single-player gameplay against computer opponents
- Creates foundation for multi-difficulty AI (Easy/Medium/Hard)
- Demonstrates autonomous agent behavior in the game
- Required for Phase 5 multiplayer features

## Architecture Approach: Controller-Based AI

**Decision:** Use `AAIController` subclass (not component-based) because:
- Follows Unreal's Actor-Controller separation pattern
- Keeps `ARobotPawn` clean and controller-agnostic
- Enables easy switching between player and AI control
- Consistent with existing `ARobotController` (APlayerController) pattern
- Built-in AI debugging tools work natively

**Key insight:** Both player and AI controllers call the same public GameMode API:
- `GameMode->SelectCardFromHand(Robot, HandIndex)` - Select a card
- `GameMode->OnControllerReady(Controller)` - Signal programming complete (NEW)

## Implementation Plan

### Phase 1: Foundation - Controller Infrastructure

**1.1 Create Control Type Enum** (`RobotRallyGameMode.h`)
```cpp
UENUM(BlueprintType)
enum class ERobotControllerType : uint8
{
    Player,    // Human keyboard control
    AI_Easy,   // Random + safety checks
    AI_Medium, // Pathfinding + hazard avoidance
    AI_Hard    // Advanced prediction (future)
};
```

**1.2 Create Robot Spawn Configuration** (`RobotRallyGameMode.h`)
```cpp
USTRUCT(BlueprintType)
struct FRobotSpawnData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FIntVector StartPosition = FIntVector(0, 0, 0);

    UPROPERTY(EditAnywhere)
    EGridDirection StartFacing = EGridDirection::North;

    UPROPERTY(EditAnywhere)
    ERobotControllerType ControllerType = ERobotControllerType::Player;

    UPROPERTY(EditAnywhere)
    FLinearColor BodyColor = FLinearColor::White;
};
```

**1.3 Add GameMode Members** (`RobotRallyGameMode.h`)
```cpp
// Robot spawning configuration
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game|Setup")
TArray<FRobotSpawnData> RobotSpawnConfigs;

// Track which controllers have finished programming
TSet<AController*> ReadyControllers;

// Functions
void SpawnRobotsWithControllers();
TSubclassOf<AController> GetControllerClassForType(ERobotControllerType Type);
void OnControllerReady(AController* Controller);
bool AreAllRobotsReady() const;
```

**1.4 Implement Spawn System** (`RobotRallyGameMode.cpp`)
- Replace manual robot spawning in `SetupTestScene()` with `SpawnRobotsWithControllers()`
- For each `FRobotSpawnData` in config:
  - Spawn `ARobotPawn` at grid position
  - Spawn appropriate controller type (Player or AI)
  - Call `Controller->Possess(RobotPawn)`
  - Add to `Robots` and `RobotPrograms` arrays

**Files:** `RobotRallyGameMode.h`, `RobotRallyGameMode.cpp`

---

### Phase 2: AI Controller Skeleton

**2.1 Create RobotAIController Class**

**File:** `Source/RobotRally/RobotAIController.h`
```cpp
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RobotAIController.generated.h"

class ARobotPawn;
class ARobotRallyGameMode;
class AGridManager;

UCLASS()
class ROBOTRALLY_API ARobotAIController : public AAIController
{
    GENERATED_BODY()

public:
    ARobotAIController();

    virtual void OnPossess(APawn* InPawn) override;

    // Called by GameMode when programming phase starts
    void StartCardSelection();

    // AI decision-making
    void MakeCardSelections();

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float ThinkingDelay = 1.0f;  // Delay before AI selects cards

protected:
    // Cached references
    UPROPERTY()
    ARobotPawn* ControlledRobot;

    UPROPERTY()
    ARobotRallyGameMode* GameMode;

    UPROPERTY()
    AGridManager* GridManager;

    // State
    bool bHasProgrammedCards = false;
    FTimerHandle ThinkingTimerHandle;

    // Pathfinding (Phase 4)
    TArray<FIntVector> FindPathToCheckpoint(int32 CheckpointNum);
    float Heuristic(const FIntVector& A, const FIntVector& B) const;

    // Card conversion (Phase 5)
    TArray<FRobotCard> ConvertPathToCards(const TArray<FIntVector>& Path);

    // Card selection (Phase 6)
    void SelectBestCardsFromHand(const TArray<FRobotCard>& DesiredCards);
    float EvaluateCardMatch(const FRobotCard& HandCard, const FRobotCard& DesiredCard) const;
    bool WillCardCauseDeath(const FRobotCard& Card) const;
};
```

**File:** `Source/RobotRally/RobotAIController.cpp`
- `OnPossess()` - Cache references to Robot, GameMode, GridManager
- `StartCardSelection()` - Set timer for `ThinkingDelay`, then call `MakeCardSelections()`
- `MakeCardSelections()` - **Stub: Select 5 random valid cards**, then call `GameMode->OnControllerReady(this)`

**Files:** `RobotAIController.h`, `RobotAIController.cpp`

---

### Phase 3: Auto-Execution System

**3.1 Modify StartProgrammingPhase** (`RobotRallyGameMode.cpp`)
```cpp
void ARobotRallyGameMode::StartProgrammingPhase()
{
    CurrentState = EGameState::Programming;
    CurrentRegister = 0;
    DiscardHand();
    DealHandsToAllRobots();

    // Reset ready tracking
    ReadyControllers.Empty();

    // Notify all AI controllers to start thinking
    for (ARobotPawn* Robot : Robots)
    {
        if (!Robot || !Robot->bIsAlive) continue;

        AController* Controller = Robot->GetController();
        if (ARobotAIController* AIController = Cast<ARobotAIController>(Controller))
        {
            AIController->StartCardSelection();
        }
    }

    ShowEventMessage(TEXT("Programming phase. Select 5 cards."), FColor::Cyan);
}
```

**3.2 Implement OnControllerReady** (`RobotRallyGameMode.cpp`)
```cpp
void ARobotRallyGameMode::OnControllerReady(AController* Controller)
{
    if (!Controller) return;

    ReadyControllers.Add(Controller);

    // Check if all alive robots' controllers are ready
    if (AreAllRobotsReady())
    {
        ShowEventMessage(TEXT("All robots programmed. Starting execution..."), FColor::Green);

        // Small delay for visual feedback
        FTimerHandle DelayHandle;
        GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this]()
        {
            StartExecutionPhase();
        }, 0.5f, false);
    }
}

bool ARobotRallyGameMode::AreAllRobotsReady() const
{
    for (ARobotPawn* Robot : Robots)
    {
        if (!Robot || !Robot->bIsAlive) continue;

        AController* Controller = Robot->GetController();
        if (!Controller) continue;

        if (!ReadyControllers.Contains(Controller))
            return false;
    }
    return true;
}
```

**3.3 Update Player Controller** (`RobotController.cpp`)
- In `SelectCard()`: When 5th register is filled, call `GameMode->OnControllerReady(this)`
- In `OnUndoSelection()`: If registers drop below 5, remove `this` from `GameMode->ReadyControllers`

**Files:** `RobotRallyGameMode.cpp`, `RobotController.cpp`

---

### Phase 4: Pathfinding Implementation (A* with Hazards)

**4.1 Implement A* Pathfinding** (`RobotAIController.cpp`)

```cpp
TArray<FIntVector> ARobotAIController::FindPathToCheckpoint(int32 CheckpointNum)
{
    // Get target checkpoint position from GridManager
    FIntVector Goal = GridManager->GetCheckpointPosition(CheckpointNum);
    FIntVector Start = ControlledRobot->GetGridPosition();

    // A* algorithm
    TMap<FIntVector, float> GScore;  // Cost from start
    TMap<FIntVector, float> FScore;  // Estimated total cost
    TMap<FIntVector, FIntVector> CameFrom;  // Parent map
    TArray<FIntVector> OpenSet;

    GScore.Add(Start, 0.0f);
    FScore.Add(Start, Heuristic(Start, Goal));
    OpenSet.Add(Start);

    while (OpenSet.Num() > 0)
    {
        // Get node with lowest FScore
        FIntVector Current = GetLowestFScore(OpenSet, FScore);

        if (Current == Goal)
            return ReconstructPath(CameFrom, Current);

        OpenSet.Remove(Current);

        // Check 4 neighbors (North, East, South, West)
        for (EGridDirection Dir : {North, East, South, West})
        {
            FIntVector Neighbor = GetNeighborInDirection(Current, Dir);

            if (!GridManager->IsValidGridPosition(Neighbor))
                continue;

            ETileType TileType = GridManager->GetTileType(Neighbor);

            // Calculate movement cost with hazard penalties
            float MoveCost = 1.0f;
            if (TileType == ETileType::Pit)
                continue;  // Never path through pits
            if (TileType == ETileType::Laser)
                MoveCost += 3.0f;  // Avoid lasers (high penalty)
            if (IsConveyorTile(TileType))
                MoveCost += 1.5f;  // Conveyors are unpredictable

            float TentativeGScore = GScore[Current] + MoveCost;

            if (!GScore.Contains(Neighbor) || TentativeGScore < GScore[Neighbor])
            {
                CameFrom.Add(Neighbor, Current);
                GScore.Add(Neighbor, TentativeGScore);
                FScore.Add(Neighbor, TentativeGScore + Heuristic(Neighbor, Goal));

                if (!OpenSet.Contains(Neighbor))
                    OpenSet.Add(Neighbor);
            }
        }
    }

    // No path found - return empty or fallback path
    return TArray<FIntVector>();
}

float ARobotAIController::Heuristic(const FIntVector& A, const FIntVector& B) const
{
    // Manhattan distance
    return FMath::Abs(A.X - B.X) + FMath::Abs(A.Y - B.Y);
}
```

**Helper functions to implement:**
- `GetLowestFScore()` - Find node with minimum F score in open set
- `ReconstructPath()` - Build path from parent map
- `GetNeighborInDirection()` - Get adjacent grid position
- `IsConveyorTile()` - Check if tile is conveyor type

**Files:** `RobotAIController.cpp`

---

### Phase 5: Card Conversion (Path → Card Sequence)

**5.1 Convert Grid Path to Movement Cards** (`RobotAIController.cpp`)

```cpp
TArray<FRobotCard> ARobotAIController::ConvertPathToCards(const TArray<FIntVector>& Path)
{
    TArray<FRobotCard> DesiredCards;
    EGridDirection CurrentFacing = ControlledRobot->GetFacingDirection();
    FIntVector CurrentPos = Path[0];

    for (int32 i = 1; i < Path.Num() && DesiredCards.Num() < 5; ++i)
    {
        FIntVector NextPos = Path[i];
        FIntVector Delta = NextPos - CurrentPos;

        // Convert delta to target direction
        EGridDirection TargetDir = DeltaToDirection(Delta);

        // Calculate rotation needed (steps: -1=left, +1=right, 2=U-turn)
        int32 RotationSteps = GetRotationSteps(CurrentFacing, TargetDir);

        // Add rotation card if needed
        if (RotationSteps == 1)
            DesiredCards.Add(FRobotCard{ECardAction::RotateRight, 0});
        else if (RotationSteps == -1 || RotationSteps == 3)
            DesiredCards.Add(FRobotCard{ECardAction::RotateLeft, 0});
        else if (RotationSteps == 2)
            DesiredCards.Add(FRobotCard{ECardAction::UTurn, 0});

        if (DesiredCards.Num() >= 5) break;

        // Add move card
        DesiredCards.Add(FRobotCard{ECardAction::Move1, 0});

        CurrentFacing = TargetDir;
        CurrentPos = NextPos;
    }

    // Pad with safe moves if path shorter than 5 cards
    while (DesiredCards.Num() < 5)
    {
        DesiredCards.Add(FRobotCard{ECardAction::Move1, 0});
    }

    return DesiredCards;
}
```

**Helper functions:**
- `DeltaToDirection()` - Convert (dx, dy) to EGridDirection
- `GetRotationSteps()` - Calculate rotation delta between directions

**Files:** `RobotAIController.cpp`

---

### Phase 6: Card Selection (Match Desired → Hand)

**6.1 Select Best Available Cards** (`RobotAIController.cpp`)

```cpp
void ARobotAIController::SelectBestCardsFromHand(const TArray<FRobotCard>& DesiredCards)
{
    FRobotProgram* MyProgram = GetMyRobotProgram();
    if (!MyProgram) return;

    for (int32 RegisterIndex = 0; RegisterIndex < 5; ++RegisterIndex)
    {
        FRobotCard DesiredCard = DesiredCards[RegisterIndex];

        // Find best match in hand
        int32 BestHandIndex = -1;
        float BestScore = -1.0f;

        for (int32 HandIndex = 0; HandIndex < MyProgram->HandCards.Num(); ++HandIndex)
        {
            // Skip already-selected cards
            if (GameMode->IsCardInRegister(MyProgram, HandIndex))
                continue;

            // Safety check: reject cards that cause death
            if (WillCardCauseDeath(MyProgram->HandCards[HandIndex]))
                continue;

            float Score = EvaluateCardMatch(MyProgram->HandCards[HandIndex], DesiredCard);

            if (Score > BestScore)
            {
                BestScore = Score;
                BestHandIndex = HandIndex;
            }
        }

        // Select the best available card
        if (BestHandIndex != -1)
        {
            GameMode->SelectCardFromHand(ControlledRobot, BestHandIndex);
        }
    }

    bHasProgrammedCards = true;
    GameMode->OnControllerReady(this);
}

float ARobotAIController::EvaluateCardMatch(const FRobotCard& HandCard, const FRobotCard& DesiredCard) const
{
    // Perfect match
    if (HandCard.Action == DesiredCard.Action)
        return 100.0f;

    // Partial match (e.g., Move2 when wanted Move1)
    if (IsSameActionType(HandCard.Action, DesiredCard.Action))
        return 50.0f;

    // Fallback: any safe card
    return 10.0f;
}

bool ARobotAIController::WillCardCauseDeath(const FRobotCard& Card) const
{
    // Simulate card execution, check if result is a pit
    FIntVector SimPos = ControlledRobot->GetGridPosition();
    EGridDirection SimDir = ControlledRobot->GetFacingDirection();

    // Apply card to simulation
    ApplyCardToSimulation(SimPos, SimDir, Card);

    // Check tile type
    ETileType TileType = GridManager->GetTileType(SimPos);
    return (TileType == ETileType::Pit);
}
```

**Files:** `RobotAIController.cpp`

---

### Phase 7: Integration & Polish

**7.1 Update MakeCardSelections** (`RobotAIController.cpp`)
```cpp
void ARobotAIController::MakeCardSelections()
{
    // Get next checkpoint to reach
    int32 NextCheckpoint = ControlledRobot->CurrentCheckpoint + 1;

    // Find path using A*
    TArray<FIntVector> Path = FindPathToCheckpoint(NextCheckpoint);

    // Convert path to desired card sequence
    TArray<FRobotCard> DesiredCards = ConvertPathToCards(Path);

    // Select best matching cards from hand
    SelectBestCardsFromHand(DesiredCards);
}
```

**7.2 Add Visual Feedback**
- When AI starts thinking: `GameMode->ShowEventMessage("AI thinking...", FColor::Yellow)`
- When AI finishes: `GameMode->ShowEventMessage("AI ready!", FColor::Green)`

**7.3 Update Build Configuration** (`RobotRally.Build.cs`)
- Add `"AIModule"` to `PublicDependencyModuleNames` (for AAIController)

**Files:** `RobotAIController.cpp`, `RobotRally.Build.cs`

---

## Critical Files

| File | Purpose | Phase |
|------|---------|-------|
| `RobotRallyGameMode.h` | Add enums, structs, spawn config, ready tracking | 1, 3 |
| `RobotRallyGameMode.cpp` | Implement spawn system, auto-execution | 1, 3 |
| `RobotAIController.h` | NEW - AI controller header | 2 |
| `RobotAIController.cpp` | NEW - AI logic, pathfinding, card selection | 2, 4, 5, 6, 7 |
| `RobotController.cpp` | Add ready notification on 5th card | 3 |
| `RobotRally.Build.cs` | Add AIModule dependency | 7 |
| `GridManager.h/cpp` | Add `GetCheckpointPosition()` helper | 4 |

## Verification Steps

### Test 1: Basic AI (Random Cards)
**Setup:** Spawn 1 Player + 1 AI (random selection)
1. Start PIE, observe AI selects 5 random cards after delay
2. Player selects 5 cards
3. Game auto-starts execution (no E press needed)
4. Both robots execute cards by priority

**Expected:** Game progresses with mixed human/AI control

### Test 2: AI Pathfinding
**Setup:** Spawn AI at (1,1), checkpoint at (5,5), no obstacles
1. Start PIE, observe AI thinks for 1 second
2. AI selects cards (rotations + moves toward checkpoint)
3. AI executes and moves toward (5,5)

**Expected:** AI navigates toward checkpoint

### Test 3: Hazard Avoidance
**Setup:** Create pit at (3,1), AI at (1,1), checkpoint at (5,1)
1. Start PIE
2. AI should path around pit (detour north or south)
3. AI should NOT select cards that walk into pit

**Expected:** AI avoids pits and lasers

### Test 4: Multi-AI Game
**Setup:** Spawn 3 AI robots at different positions
1. All AIs select cards simultaneously (staggered visual delays)
2. Game auto-executes when all ready
3. Cards execute by priority order

**Expected:** Multiple AIs compete, game flows smoothly

### Test 5: Player Timeout (Edge Case)
**Setup:** 1 Player + 1 AI
1. AI finishes programming in 1 second
2. Player hasn't selected any cards yet
3. Game should wait for player (NOT auto-execute)
4. Player selects 5 cards
5. Game auto-executes

**Expected:** Game waits for human player, doesn't force execution

---

## Implementation Checklist

- [ ] **Phase 1:** Create enums, spawn config, spawn system
- [ ] **Phase 2:** Create RobotAIController with random card selection
- [ ] **Phase 3:** Implement auto-execution system
- [ ] **Phase 4:** Implement A* pathfinding with hazard penalties
- [ ] **Phase 5:** Implement card conversion (path → cards)
- [ ] **Phase 6:** Implement smart card selection (match hand to desired)
- [ ] **Phase 7:** Add visual feedback and polish
- [ ] **Testing:** Verify all 5 test scenarios pass
- [ ] **Build:** Ensure project compiles without warnings
- [ ] **Documentation:** Update CLAUDE.md and README.md with AI features

---

## Future Enhancements (Not in Scope)

- **Easy Difficulty:** Pure random selection with pit safety
- **Hard Difficulty:** Predict other robots' moves, optimize priority
- **Blueprint AI Config:** Expose difficulty/personality via DataAssets
- **Debug Visualization:** Draw A* path and card decisions on screen
- **Multi-turn Planning:** Path to all checkpoints, not just next one
- **Conveyor Prediction:** Simulate conveyor chains in pathfinding
