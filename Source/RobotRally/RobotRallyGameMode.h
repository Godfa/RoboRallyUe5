// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RobotMovementComponent.h"
#include "RobotRallyGameMode.generated.h"

class AGridManager;
class ARobotPawn;
class ARobotRallyHUD;
class AController;
class ARobotAIController;

UENUM(BlueprintType)
enum class ECardAction : uint8
{
	Move1,
	Move2,
	Move3,
	MoveBack,
	RotateRight,
	RotateLeft,
	UTurn
};

USTRUCT(BlueprintType)
struct FRobotCard
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardAction Action = ECardAction::Move1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;
};

USTRUCT(BlueprintType)
struct FRobotProgram
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ARobotPawn* Robot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FRobotCard> HandCards;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<int32> RegisterSlots;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FRobotCard> CommittedProgram;
};

USTRUCT(BlueprintType)
struct FExecutionQueueEntry
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ARobotPawn* Robot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRobotCard Card;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 RegisterNumber = 0;

	bool operator<(const FExecutionQueueEntry& Other) const
	{
		return Card.Priority > Other.Card.Priority;  // Higher priority first
	}
};

UENUM(BlueprintType)
enum class EGameState : uint8
{
	Programming,
	Executing,
	GameOver
};

UENUM(BlueprintType)
enum class ERobotControllerType : uint8
{
	Player,    // Human keyboard control
	AI_Easy,   // Random + safety checks
	AI_Medium, // Pathfinding + hazard avoidance
	AI_Hard    // Advanced prediction (future)
};

USTRUCT(BlueprintType)
struct FRobotSpawnData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game|Setup")
	FIntVector StartPosition = FIntVector(0, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game|Setup")
	EGridDirection StartFacing = EGridDirection::North;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game|Setup")
	ERobotControllerType ControllerType = ERobotControllerType::Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game|Setup")
	FLinearColor BodyColor = FLinearColor::White;
};

UCLASS()
class ROBOTRALLY_API ARobotRallyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARobotRallyGameMode();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	EGameState CurrentState;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartExecutionPhase();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartProgrammingPhase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|TestScene")
	AGridManager* GridManagerInstance;

	// Robot spawning configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game|Setup")
	TArray<FRobotSpawnData> RobotSpawnConfigs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|Robots")
	TArray<ARobotPawn*> Robots;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|Cards")
	TArray<FRobotProgram> RobotPrograms;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|Execution")
	TArray<FExecutionQueueEntry> ExecutionQueue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|Execution")
	int32 CurrentExecutionIndex = 0;

	UPROPERTY()
	TSet<ARobotPawn*> MovingRobots;

	static constexpr int32 NUM_REGISTERS = 5;
	static constexpr int32 DECK_SIZE = 84;
	static constexpr int32 BASE_HAND_SIZE = 9;
	static constexpr int32 MIN_HAND_SIZE = 5;

	// Deck and hand system
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|Cards")
	TArray<FRobotCard> Deck;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|Cards")
	TArray<FRobotCard> DiscardPile;

	// Card selection functions
	UFUNCTION(BlueprintCallable, Category = "Game|Cards")
	void SelectCardFromHand(ARobotPawn* Robot, int32 HandIndex);

	UFUNCTION(BlueprintCallable, Category = "Game|Cards")
	void UndoLastSelection();

	bool IsCardInRegister(const FRobotProgram* Program, int32 HandIndex) const;

	static FString GetCardActionName(ECardAction Action);

	// Tile hazard processing (public so RobotPawn can trigger after manual moves)
	UFUNCTION(BlueprintCallable, Category = "Game")
	void ProcessTileEffects();

	// True while tile effects/conveyors are being processed (prevents re-triggering)
	bool bProcessingTileEffects = false;

	// Called by RobotPawn after WASD input to start polling for movement completion
	void StartManualMoveTick();

	// Push a message to the on-screen event log
	void ShowEventMessage(const FString& Text, FColor Color = FColor::White);

	// AI controller ready tracking - public so controllers can signal when ready
	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnControllerReady(AController* Controller);

private:
	void SetupTestScene();
	void ExecuteCardAction(ARobotPawn* Robot, ECardAction Action);

	void BuildDeck();
	void ShuffleDeck();
	void DealHandsToAllRobots();
	void CommitAllRobotPrograms();
	void DiscardHand();

	void BuildExecutionQueue(int32 RegisterIndex);
	void ProcessExecutionQueue();
	void CheckParallelMovementComplete();
	void ProcessAllRobotTileEffects();
	void ProcessRobotTileEffects(ARobotPawn* Robot);
	void ProcessAllConveyors();
	void ProcessRobotConveyors(ARobotPawn* Robot);
	void CheckWinLoseConditions();
	void OnTileEffectsComplete();

	// AI controller tracking
	TSet<AController*> ReadyControllers;
	bool AreAllRobotsReady() const;
	void SpawnRobotsWithControllers();
	TSubclassOf<AController> GetControllerClassForType(ERobotControllerType Type);

	int32 CurrentRegister = 0;
	FTimerHandle MovementCheckTimerHandle;
	FTimerHandle TileEffectTimerHandle;
	FTimerHandle ManualMoveTimerHandle;

	void CheckManualMoveComplete();
};
