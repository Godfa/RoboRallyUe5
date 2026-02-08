# RobotRally API Reference

Quick reference for developers extending the RobotRally codebase.

---

## Core Classes

### ARobotPawn

```cpp
#include "RobotPawn.h"

// Movement execution
void ExecuteMoveCommand(int32 Distance);      // Move forward (+) or backward (-)
void ExecuteRotateCommand(int32 Steps);       // Rotate (1=right, -1=left, 2=180°)

// Health system
void ApplyDamage(int32 Amount);               // Reduce health, trigger death if ≤0
void Respawn();                               // Restore health, teleport to respawn point

// Checkpoint system
void ReachCheckpoint(int32 Number);           // Mark checkpoint as reached (validates order)

// Properties
URobotMovementComponent* RobotMovement;       // Movement component reference
int32 Health;                                 // Current health (0-10)
int32 Lives;                                  // Remaining lives (0-3)
int32 CurrentCheckpoint;                      // Last checkpoint reached (0-based)
bool bIsAlive;                                // Death state flag

// Delegates
FOnRobotDeath OnDeath;                        // Broadcast when health ≤ 0
FOnCheckpointReached OnCheckpointReached;     // Broadcast(int32 CheckpointNumber)
```

**Example: Damage robot on laser tile**
```cpp
ARobotPawn* Robot = GetRobot();
Robot->ApplyDamage(1);  // Laser = 1 damage

if (!Robot->bIsAlive) {
    UE_LOG(LogTemp, Warning, TEXT("Robot died!"));
}
```

---

### URobotMovementComponent

```cpp
#include "RobotMovementComponent.h"

// Grid movement
void MoveInGrid(int32 Distance);              // Set target N tiles in facing direction
void RotateInGrid(int32 Steps);               // Rotate N×90° (+ = CW, - = CCW)
void MoveToWorldPosition(FVector NewTarget);  // Direct world-space movement
void SetGridPosition(int32 NewX, int32 NewY); // Force-set coords without animation

// Initialization
void InitializeGridPosition(int32 InGridX, int32 InGridY, EGridDirection InFacing);

// Queries
int32 GetCurrentGridX() const;
int32 GetCurrentGridY() const;
EGridDirection GetFacingDirection() const;
bool IsMoving() const;                        // True during interpolation
bool IsRotating() const;                      // True during rotation

// Properties
float GridSize;                               // Tile size (default 100)
AGridManager* GridManager;                    // Grid reference for validation

// Delegates
FOnGridPositionChanged OnGridPositionChanged; // Broadcast(int32 NewX, int32 NewY)

// Static helpers
static void GetDirectionDelta(EGridDirection Dir, int32& OutDX, int32& OutDY);
```

**Example: Move robot 3 tiles forward**
```cpp
URobotMovementComponent* Movement = Robot->RobotMovement;
Movement->MoveInGrid(3);  // Sets target, begins interpolation

// Check when done
if (!Movement->IsMoving()) {
    UE_LOG(LogTemp, Log, TEXT("Movement complete!"));
}
```

**Example: Teleport robot (for conveyors)**
```cpp
FVector NewWorldPos = GridManager->GridToWorld(FIntVector(5, 3, 0));
Movement->MoveToWorldPosition(NewWorldPos);
Movement->SetGridPosition(5, 3);  // Update internal coords
```

---

### AGridManager

```cpp
#include "GridManager.h"

// Coordinate conversion
FVector GridToWorld(FIntVector Coords) const;    // Grid (x,y,0) → World FVector
FIntVector WorldToGrid(FVector Location) const;  // World FVector → Grid (x,y,0)

// Tile queries
ETileType GetTileType(FIntVector Coords) const;  // Returns tile type enum
FTileData GetTileData(FIntVector Coords) const;  // Returns full tile data
bool IsInBounds(int32 X, int32 Y) const;         // True if coords within grid
bool IsValidTile(FIntVector Coords) const;       // True if in-bounds and not pit

// Tile modification
void SetTileType(FIntVector Coords, const FTileData& Data);  // Update tile + visual
void RefreshAllTileVisuals();                    // Rebuild all tile meshes

// Checkpoint queries
int32 GetTotalCheckpoints() const;               // Count checkpoint tiles

// Properties
int32 Width;                                     // Grid width (default 10)
int32 Height;                                    // Grid height (default 10)
float TileSize;                                  // Tile size in units (100)
TMap<FIntVector, FTileData> GridMap;             // Tile data storage

// Static helpers
static FLinearColor GetTileColor(ETileType Type);
static bool IsConveyor(ETileType Type);
static float GetConveyorYaw(ETileType Type);     // Returns arrow rotation
```

**Example: Check if tile is safe to move to**
```cpp
FIntVector DestCoords(5, 3, 0);

if (!GridManager->IsInBounds(DestCoords.X, DestCoords.Y)) {
    UE_LOG(LogTemp, Warning, TEXT("Out of bounds!"));
    return;
}

ETileType Type = GridManager->GetTileType(DestCoords);
if (Type == ETileType::Pit) {
    UE_LOG(LogTemp, Warning, TEXT("Cannot move onto pit!"));
    return;
}

// Safe to move
Robot->RobotMovement->MoveInGrid(1);
```

**Example: Create custom board layout**
```cpp
void AMyGameMode::SetupCustomBoard()
{
    // Create 3-tile laser corridor
    for (int32 X = 2; X <= 4; X++) {
        FTileData LaserData;
        LaserData.TileType = ETileType::Laser;
        GridManager->SetTileType(FIntVector(X, 5, 0), LaserData);
    }

    // Add checkpoint at end
    FTileData CheckpointData;
    CheckpointData.TileType = ETileType::Checkpoint;
    CheckpointData.CheckpointNumber = 1;
    GridManager->SetTileType(FIntVector(5, 5, 0), CheckpointData);
}
```

---

### ARobotRallyGameMode

```cpp
#include "RobotRallyGameMode.h"

// State management
void StartProgrammingPhase();                 // Reset registers, deal hand
void StartExecutionPhase();                   // Commit program, begin execution

// Card programming
void SelectCardFromHand(int32 HandIndex);     // Add card to next register
void UndoLastSelection();                     // Remove last card, return to hand
bool AreAllRegistersFilled() const;           // True if 5 cards programmed
bool IsCardInRegister(int32 HandIndex) const; // Check if hand card is used

// Tile effects (call after movement)
void ProcessTileEffects();                    // Handle pits, lasers, conveyors, checkpoints

// UI feedback
void ShowEventMessage(const FString& Text, FColor Color = FColor::White);

// Properties
EGameState CurrentState;                      // Programming | Executing | GameOver
TArray<FRobotCard> Deck;                      // 84-card shuffled deck
TArray<FRobotCard> HandCards;                 // Current hand (5-9 cards)
TArray<int32> RegisterSlots;                  // Indices into HandCards for 5 registers
TArray<FRobotCard> ProgrammedCards;           // Final committed program
AGridManager* GridManagerInstance;            // Grid reference
ARobotPawn* TestRobot;                        // Player robot

// Constants
static constexpr int32 NUM_REGISTERS = 5;     // Always 5 cards
static constexpr int32 DECK_SIZE = 84;        // Standard deck size
static constexpr int32 BASE_HAND_SIZE = 9;    // Base hand size (full health)
static constexpr int32 MIN_HAND_SIZE = 5;     // Minimum hand size (10 damage)

// Static helper
static FString GetCardActionName(ECardAction Action);  // Move1 → "Move1"
```

**Example: Custom card execution logic**
```cpp
void AMyGameMode::ExecuteCustomCard(ECardAction Action)
{
    switch (Action) {
        case ECardAction::Move1:
            TestRobot->ExecuteMoveCommand(1);
            break;
        case ECardAction::RotateRight:
            TestRobot->ExecuteRotateCommand(1);
            break;
        // ... handle other actions
    }

    // Wait for movement, then process tile effects
    GetWorldTimerManager().SetTimer(
        MovementCheckTimerHandle,
        this,
        &AMyGameMode::CheckMovementComplete,
        0.1f,
        true  // Loop until movement done
    );
}

void AMyGameMode::CheckMovementComplete()
{
    if (!TestRobot->RobotMovement->IsMoving() &&
        !TestRobot->RobotMovement->IsRotating()) {
        GetWorldTimerManager().ClearTimer(MovementCheckTimerHandle);
        ProcessTileEffects();  // Handle hazards/conveyors
    }
}
```

---

### ARobotController

```cpp
#include "RobotController.h"

// Input handlers (bound in SetupInputComponent)
void OnMoveForward();                         // W key
void OnMoveBackward();                        // S key
void OnMoveLeft();                            // A key (turn left)
void OnMoveRight();                           // D key (turn right)

void OnSelectCard1() ... OnSelectCard9();     // Number keys 1-9
void OnExecuteProgram();                      // E key
void OnUndoSelection();                       // Backspace key

// Properties
ARobotPawn* ControlledRobot;                  // Cached pawn reference
ARobotRallyGameMode* GameMode;                // Cached game mode reference
```

**Example: Add custom input binding**
```cpp
// In ARobotController::SetupInputComponent()
InputComponent->BindAction("CustomAction", IE_Pressed, this, &ARobotController::OnCustomAction);

void ARobotController::OnCustomAction()
{
    if (GameMode->CurrentState != EGameState::Programming) return;

    // Custom logic here
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Custom action!"));
}
```

---

### ARobotRallyHUD

```cpp
#include "RobotRallyHUD.h"

// Event log
void AddEventMessage(const FString& Text, FColor Color = FColor::White);

// Drawing (override for custom rendering)
virtual void DrawHUD() override;

// Constants
static constexpr float MESSAGE_DURATION = 5.0f;  // Message fade time
static constexpr int32 MAX_MESSAGES = 8;         // Max simultaneous messages
```

**Example: Show custom feedback**
```cpp
ARobotRallyHUD* HUD = Cast<ARobotRallyHUD>(PlayerController->GetHUD());
if (HUD) {
    HUD->AddEventMessage("Power-up collected!", FColor::Yellow);
}
```

---

## Enums & Structs

### ECardAction

```cpp
enum class ECardAction : uint8
{
    Move1,       // Move 1 tile forward
    Move2,       // Move 2 tiles forward
    Move3,       // Move 3 tiles forward
    MoveBack,    // Move 1 tile backward
    RotateRight, // Turn 90° clockwise
    RotateLeft,  // Turn 90° counter-clockwise
    UTurn        // Turn 180°
};
```

### FRobotCard

```cpp
USTRUCT(BlueprintType)
struct FRobotCard
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECardAction Action = ECardAction::Move1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Priority = 0;  // Higher = executes first
};
```

**Example: Create custom card**
```cpp
FRobotCard MyCard;
MyCard.Action = ECardAction::Move3;
MyCard.Priority = 800;  // High priority

Deck.Add(MyCard);
```

### ETileType

```cpp
enum class ETileType : uint8
{
    Normal,        // Safe tile
    Pit,           // Instant death
    ConveyorNorth, // Push +X
    ConveyorSouth, // Push -X
    ConveyorEast,  // Push +Y
    ConveyorWest,  // Push -Y
    Laser,         // 1 damage
    Checkpoint     // Collect to win
};
```

### FTileData

```cpp
USTRUCT(BlueprintType)
struct FTileData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ETileType TileType = ETileType::Normal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CheckpointNumber = 0;  // 1-based, 0 for non-checkpoints
};
```

### EGridDirection

```cpp
enum class EGridDirection : uint8
{
    North,  // +X axis (forward = increasing X)
    East,   // +Y axis (forward = increasing Y)
    South,  // -X axis (forward = decreasing X)
    West    // -Y axis (forward = decreasing Y)
};
```

**Example: Get direction delta**
```cpp
int32 DX, DY;
URobotMovementComponent::GetDirectionDelta(EGridDirection::North, DX, DY);
// Result: DX = 1, DY = 0
```

### EGameState

```cpp
enum class EGameState : uint8
{
    Programming,  // Player selects cards
    Executing,    // Robot executes program
    GameOver      // Game ended (win/lose)
};
```

---

## Common Patterns

### Adding a New Tile Type

1. **Add enum value**
```cpp
// In GridManager.h
enum class ETileType : uint8
{
    // ... existing types
    Repair  // New type
};
```

2. **Add color mapping**
```cpp
// In GridManager.cpp
FLinearColor AGridManager::GetTileColor(ETileType Type)
{
    switch (Type) {
        // ... existing cases
        case ETileType::Repair: return FLinearColor::Green;
    }
}
```

3. **Add tile effect logic**
```cpp
// In RobotRallyGameMode.cpp
void ARobotRallyGameMode::ProcessTileEffects()
{
    ETileType Type = GridManager->GetTileType(CurrentPos);

    if (Type == ETileType::Repair) {
        TestRobot->Health = FMath::Min(TestRobot->Health + 1, TestRobot->MaxHealth);
        ShowEventMessage("Repaired +1 HP", FColor::Green);
    }
}
```

### Creating a Custom Game Mode

```cpp
// MyCustomGameMode.h
#include "RobotRallyGameMode.h"
#include "MyCustomGameMode.generated.h"

UCLASS()
class AMyCustomGameMode : public ARobotRallyGameMode
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    void SetupCustomBoard();
};

// MyCustomGameMode.cpp
void AMyCustomGameMode::BeginPlay()
{
    Super::BeginPlay();  // Creates grid and robot
    SetupCustomBoard();
}

void AMyCustomGameMode::SetupCustomBoard()
{
    // Custom board layout
    FTileData LaserTile;
    LaserTile.TileType = ETileType::Laser;

    for (int32 X = 0; X < 10; X++) {
        GridManager->SetTileType(FIntVector(X, 5, 0), LaserTile);
    }
}
```

### Adding a Robot Ability

```cpp
// In RobotPawn.h
UFUNCTION(BlueprintCallable, Category = "Robot|Abilities")
void ActivateShield(float Duration);

// In RobotPawn.cpp
void ARobotPawn::ActivateShield(float Duration)
{
    bHasShield = true;

    FTimerHandle ShieldTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        ShieldTimerHandle,
        [this]() { bHasShield = false; },
        Duration,
        false
    );
}

// Modified damage function
void ARobotPawn::ApplyDamage(int32 Amount)
{
    if (bHasShield) {
        ShowEventMessage("Shield absorbed damage!", FColor::Cyan);
        return;
    }

    // Normal damage logic
    Health -= Amount;
    // ...
}
```

---

## Debugging Helpers

### Enable Visual Debugging

```cpp
// In GridManager.cpp (or custom GameMode)
void AGridManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Draw grid debug lines
    for (int32 X = 0; X <= Width; X++) {
        FVector Start = GridToWorld(FIntVector(X, 0, 0));
        FVector End = GridToWorld(FIntVector(X, Height, 0));
        DrawDebugLine(GetWorld(), Start, End, FColor::White, false, -1, 0, 2.0f);
    }

    for (int32 Y = 0; Y <= Height; Y++) {
        FVector Start = GridToWorld(FIntVector(0, Y, 0));
        FVector End = GridToWorld(FIntVector(Width, Y, 0));
        DrawDebugLine(GetWorld(), Start, End, FColor::White, false, -1, 0, 2.0f);
    }
}
```

### Log Robot State

```cpp
void ARobotPawn::DebugLogState()
{
    UE_LOG(LogTemp, Log, TEXT("Robot State:"));
    UE_LOG(LogTemp, Log, TEXT("  Position: (%d, %d)"), GridX, GridY);
    UE_LOG(LogTemp, Log, TEXT("  Facing: %d"), (int32)RobotMovement->GetFacingDirection());
    UE_LOG(LogTemp, Log, TEXT("  Health: %d/%d"), Health, MaxHealth);
    UE_LOG(LogTemp, Log, TEXT("  Lives: %d"), Lives);
    UE_LOG(LogTemp, Log, TEXT("  Checkpoint: %d"), CurrentCheckpoint);
}
```

### Print Card Deck

```cpp
void ARobotRallyGameMode::DebugPrintDeck()
{
    UE_LOG(LogTemp, Log, TEXT("=== Deck (%d cards) ==="), Deck.Num());

    TMap<ECardAction, int32> Counts;
    for (const FRobotCard& Card : Deck) {
        Counts.FindOrAdd(Card.Action)++;
    }

    for (auto& Pair : Counts) {
        FString ActionName = GetCardActionName(Pair.Key);
        UE_LOG(LogTemp, Log, TEXT("  %s: %d"), *ActionName, Pair.Value);
    }
}
```

---

## Performance Tips

1. **Batch tile updates**: Use `SetTileType()` + `RefreshAllTileVisuals()` instead of individual updates
2. **Cache component references**: Store `RobotMovement`, `GridManager` in BeginPlay()
3. **Use timers for polling**: Prefer `SetTimer()` over Tick() for movement checks
4. **Minimize string operations**: Use `FName` for lookups, `FText` for UI
5. **Disable tick when idle**: Set `PrimaryActorTick.bCanEverTick = false` if not needed

---

## Blueprint Integration

All major classes are **BlueprintType** and expose functions via **UFUNCTION(BlueprintCallable)**.

### Create Blueprint Child Class

1. In Content Browser: **Right-click → Blueprint Class**
2. Select **ARobotPawn** (or other C++ class)
3. Name it `BP_RobotPawn`
4. Open and customize in Blueprint editor

### Override Functions

```cpp
// Mark function as BlueprintNativeEvent in C++
UFUNCTION(BlueprintNativeEvent, Category = "Robot")
void OnCheckpointReachedEvent(int32 CheckpointNumber);

// Implementation
void ARobotPawn::OnCheckpointReachedEvent_Implementation(int32 CheckpointNumber)
{
    // C++ default behavior
    UE_LOG(LogTemp, Log, TEXT("Checkpoint %d reached"), CheckpointNumber);
}

// Blueprint can now override "On Checkpoint Reached Event"
```

---

**For more details, see [Technical Documentation](TECHNICAL_DOCUMENTATION.md)**
