// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RobotRallyGameMode.generated.h"

class AGridManager;
class ARobotPawn;
class ARobotRallyHUD;

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

UENUM(BlueprintType)
enum class EGameState : uint8
{
	Programming,
	Executing,
	GameOver
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|TestScene")
	ARobotPawn* TestRobot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|Cards")
	TArray<FRobotCard> ProgrammedCards;

	static constexpr int32 NUM_REGISTERS = 5;
	static constexpr int32 DECK_SIZE = 84;
	static constexpr int32 BASE_HAND_SIZE = 9;
	static constexpr int32 MIN_HAND_SIZE = 5;

	// Deck and hand system
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|Cards")
	TArray<FRobotCard> Deck;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|Cards")
	TArray<FRobotCard> DiscardPile;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|Cards")
	TArray<FRobotCard> HandCards;

	// Indices into HandCards for each register slot (-1 = empty)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|Cards")
	TArray<int32> RegisterSlots;

	// Card selection functions
	UFUNCTION(BlueprintCallable, Category = "Game|Cards")
	void SelectCardFromHand(int32 HandIndex);

	UFUNCTION(BlueprintCallable, Category = "Game|Cards")
	void UndoLastSelection();

	UFUNCTION(BlueprintCallable, Category = "Game|Cards")
	bool AreAllRegistersFilled() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Cards")
	bool IsCardInRegister(int32 HandIndex) const;

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

private:
	void ProcessNextRegister();
	void SetupTestScene();
	void ExecuteCardAction(ECardAction Action);
	void CheckMovementComplete();

	void BuildDeck();
	void ShuffleDeck();
	void DealHand();
	void CommitRegistersToProgram();
	void DiscardHand();
	int32 CalculateHandSize() const;

	void ProcessConveyors();
	void CheckWinLoseConditions();
	void OnTileEffectsComplete();

	int32 CurrentRegister = 0;
	FTimerHandle MovementCheckTimerHandle;
	FTimerHandle TileEffectTimerHandle;
	FTimerHandle ManualMoveTimerHandle;

	void CheckManualMoveComplete();
};
