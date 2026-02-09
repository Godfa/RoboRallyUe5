// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RobotController.generated.h"

class ARobotPawn;
class ARobotRallyGameMode;

/**
 * Player controller for Robot Rally.
 * Handles keyboard input for robot movement and card selection.
 * In network mode, sends RPCs to server instead of calling GameMode directly.
 */
UCLASS()
class ROBOTRALLY_API ARobotController : public APlayerController
{
	GENERATED_BODY()

public:
	ARobotController();

protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	// Movement input handlers
	void OnMoveForward();
	void OnMoveBackward();
	void OnMoveLeft();
	void OnMoveRight();

	// Card selection (number keys 1-9)
	void OnSelectCard1();
	void OnSelectCard2();
	void OnSelectCard3();
	void OnSelectCard4();
	void OnSelectCard5();
	void OnSelectCard6();
	void OnSelectCard7();
	void OnSelectCard8();
	void OnSelectCard9();

	// Execution and undo
	void OnExecuteProgram();
	void OnUndoSelection();

	// Helper to select card by index
	void SelectCard(int32 CardIndex);

	// --- Server RPCs ---

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSelectCard(int32 HandIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUndoSelection();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCommitProgram();

	// --- Client RPCs ---

	UFUNCTION(Client, Reliable)
	void ClientNotifyError(const FString& Message);

	// Cached references
	UPROPERTY()
	ARobotPawn* ControlledRobot;

	UPROPERTY()
	ARobotRallyGameMode* GameMode;
};
