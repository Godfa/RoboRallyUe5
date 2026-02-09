// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RobotRallyGameMode.h"
#include "RobotRallyPlayerState.generated.h"

class ARobotPawn;

UCLASS()
class ROBOTRALLY_API ARobotRallyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ARobotRallyPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// --- Replicated to owning client only (for HUD display) ---

	UPROPERTY(ReplicatedUsing = OnRep_HandCards, BlueprintReadOnly, Category = "Cards")
	TArray<FRobotCard> Rep_HandCards;

	UPROPERTY(ReplicatedUsing = OnRep_RegisterSlots, BlueprintReadOnly, Category = "Cards")
	TArray<int32> Rep_RegisterSlots;

	// --- Replicated to all (public info) ---

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
	ARobotPawn* Rep_Robot = nullptr;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
	bool bIsReady = false;

private:
	UFUNCTION()
	void OnRep_HandCards();

	UFUNCTION()
	void OnRep_RegisterSlots();
};
