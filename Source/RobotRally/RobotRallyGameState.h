// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GridManager.h"
#include "RobotRallyGameMode.h"
#include "RobotRallyGameState.generated.h"

class ARobotPawn;

// Replicated tile entry for non-Normal tiles (avoids TMap replication issues)
USTRUCT(BlueprintType)
struct FReplicatedTileEntry
{
	GENERATED_BODY()

	UPROPERTY()
	int32 X = 0;

	UPROPERTY()
	int32 Y = 0;

	UPROPERTY()
	ETileType TileType = ETileType::Normal;

	UPROPERTY()
	int32 CheckpointNumber = 0;
};

UCLASS()
class ROBOTRALLY_API ARobotRallyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ARobotRallyGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// --- Replicated game state ---

	UPROPERTY(ReplicatedUsing = OnRep_CurrentGameState, BlueprintReadOnly, Category = "Game")
	EGameState Rep_CurrentGameState = EGameState::Programming;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
	int32 Rep_CurrentRegister = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
	TArray<ARobotPawn*> AllRobots;

	// --- Grid config (replicated to clients) ---

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Grid")
	int32 Rep_GridWidth = 10;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Grid")
	int32 Rep_GridHeight = 10;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Grid")
	TArray<FReplicatedTileEntry> Rep_TileOverrides;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Grid")
	int32 Rep_TotalCheckpoints = 0;

	// --- Multicast event messages ---

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastShowEventMessage(const FString& Text, FColor Color);

private:
	UFUNCTION()
	void OnRep_CurrentGameState();
};
