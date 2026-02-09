// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RobotRallyGameMode.h"
#include "RobotAIController.generated.h"

class ARobotPawn;
class AGridManager;

UCLASS()
class ROBOTRALLY_API ARobotAIController : public AAIController
{
	GENERATED_BODY()

public:
	ARobotAIController();

	// Called by GameMode at the start of each programming phase
	void StartCardSelection();

	// Difficulty level set by GameMode after spawning
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	ERobotControllerType DifficultyLevel = ERobotControllerType::AI_Easy;

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	// Card selection strategies per difficulty
	void SelectCardsEasy();
	void SelectCardsMedium();

	// Evaluate a card's score: how much it helps reach the target
	// Higher score = better card. Simulates the card action and measures distance improvement.
	float ScoreCard(ECardAction Action, FIntVector CurrentPos, EGridDirection CurrentFacing,
		FIntVector TargetPos, AGridManager* Grid) const;

	// Simulate a card action and return resulting position + facing
	void SimulateCardAction(ECardAction Action, FIntVector InPos, EGridDirection InFacing,
		FIntVector& OutPos, EGridDirection& OutFacing) const;

	// Find the grid position of the next checkpoint this robot needs
	FIntVector FindNextCheckpointPosition() const;

	// Rotate a direction by steps (1 = 90 deg clockwise, -1 = CCW)
	static EGridDirection RotateDirection(EGridDirection Dir, int32 Steps);

	// Get forward delta for a direction
	static void GetDirectionDelta(EGridDirection Dir, int32& DX, int32& DY);

	// Cached references
	UPROPERTY()
	ARobotPawn* ControlledRobot;

	UPROPERTY()
	ARobotRallyGameMode* GameMode;
};
