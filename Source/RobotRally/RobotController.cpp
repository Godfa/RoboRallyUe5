// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotController.h"
#include "RobotPawn.h"
#include "RobotRallyGameMode.h"
#include "RobotMovementComponent.h"
#include "Engine/World.h"

ARobotController::ARobotController()
{
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableTouchEvents = false;
}

void ARobotController::BeginPlay()
{
	Super::BeginPlay();

	// Cache references
	ControlledRobot = Cast<ARobotPawn>(GetPawn());
	GameMode = Cast<ARobotRallyGameMode>(GetWorld()->GetAuthGameMode());
}

void ARobotController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!InputComponent) return;

	// Movement controls (WASD)
	InputComponent->BindAction("MoveForward", IE_Pressed, this, &ARobotController::OnMoveForward);
	InputComponent->BindAction("MoveBackward", IE_Pressed, this, &ARobotController::OnMoveBackward);
	InputComponent->BindAction("MoveLeft", IE_Pressed, this, &ARobotController::OnMoveLeft);
	InputComponent->BindAction("MoveRight", IE_Pressed, this, &ARobotController::OnMoveRight);

	// Card selection (1-9)
	InputComponent->BindAction("SelectCard1", IE_Pressed, this, &ARobotController::OnSelectCard1);
	InputComponent->BindAction("SelectCard2", IE_Pressed, this, &ARobotController::OnSelectCard2);
	InputComponent->BindAction("SelectCard3", IE_Pressed, this, &ARobotController::OnSelectCard3);
	InputComponent->BindAction("SelectCard4", IE_Pressed, this, &ARobotController::OnSelectCard4);
	InputComponent->BindAction("SelectCard5", IE_Pressed, this, &ARobotController::OnSelectCard5);
	InputComponent->BindAction("SelectCard6", IE_Pressed, this, &ARobotController::OnSelectCard6);
	InputComponent->BindAction("SelectCard7", IE_Pressed, this, &ARobotController::OnSelectCard7);
	InputComponent->BindAction("SelectCard8", IE_Pressed, this, &ARobotController::OnSelectCard8);
	InputComponent->BindAction("SelectCard9", IE_Pressed, this, &ARobotController::OnSelectCard9);

	// Execution and undo
	InputComponent->BindAction("ExecuteProgram", IE_Pressed, this, &ARobotController::OnExecuteProgram);
	InputComponent->BindAction("UndoSelection", IE_Pressed, this, &ARobotController::OnUndoSelection);
}

void ARobotController::OnMoveForward()
{
	if (!ControlledRobot || !GameMode) return;
	if (!ControlledRobot->bIsAlive) return;
	if (GameMode->CurrentState != EGameState::Programming) return;
	if (GameMode->bProcessingTileEffects) return;

	if (ControlledRobot->RobotMovement)
	{
		bool bCurrentlyMoving = ControlledRobot->RobotMovement->IsMoving() || ControlledRobot->RobotMovement->IsRotating();
		if (!bCurrentlyMoving)
		{
			ControlledRobot->RobotMovement->MoveInGrid(1);
			GameMode->StartManualMoveTick();
		}
	}
}

void ARobotController::OnMoveBackward()
{
	if (!ControlledRobot || !GameMode) return;
	if (!ControlledRobot->bIsAlive) return;
	if (GameMode->CurrentState != EGameState::Programming) return;
	if (GameMode->bProcessingTileEffects) return;

	if (ControlledRobot->RobotMovement)
	{
		bool bCurrentlyMoving = ControlledRobot->RobotMovement->IsMoving() || ControlledRobot->RobotMovement->IsRotating();
		if (!bCurrentlyMoving)
		{
			ControlledRobot->RobotMovement->MoveInGrid(-1);
			GameMode->StartManualMoveTick();
		}
	}
}

void ARobotController::OnMoveLeft()
{
	if (!ControlledRobot || !GameMode) return;
	if (!ControlledRobot->bIsAlive) return;
	if (GameMode->CurrentState != EGameState::Programming) return;
	if (GameMode->bProcessingTileEffects) return;

	if (ControlledRobot->RobotMovement)
	{
		bool bCurrentlyMoving = ControlledRobot->RobotMovement->IsMoving() || ControlledRobot->RobotMovement->IsRotating();
		if (!bCurrentlyMoving)
		{
			ControlledRobot->RobotMovement->RotateInGrid(-1);
		}
	}
}

void ARobotController::OnMoveRight()
{
	if (!ControlledRobot || !GameMode) return;
	if (!ControlledRobot->bIsAlive) return;
	if (GameMode->CurrentState != EGameState::Programming) return;
	if (GameMode->bProcessingTileEffects) return;

	if (ControlledRobot->RobotMovement)
	{
		bool bCurrentlyMoving = ControlledRobot->RobotMovement->IsMoving() || ControlledRobot->RobotMovement->IsRotating();
		if (!bCurrentlyMoving)
		{
			ControlledRobot->RobotMovement->RotateInGrid(1);
		}
	}
}

void ARobotController::SelectCard(int32 CardIndex)
{
	if (!GameMode) return;
	if (GameMode->CurrentState != EGameState::Programming) return;

	GameMode->SelectCardFromHand(CardIndex);
}

void ARobotController::OnSelectCard1() { SelectCard(0); }
void ARobotController::OnSelectCard2() { SelectCard(1); }
void ARobotController::OnSelectCard3() { SelectCard(2); }
void ARobotController::OnSelectCard4() { SelectCard(3); }
void ARobotController::OnSelectCard5() { SelectCard(4); }
void ARobotController::OnSelectCard6() { SelectCard(5); }
void ARobotController::OnSelectCard7() { SelectCard(6); }
void ARobotController::OnSelectCard8() { SelectCard(7); }
void ARobotController::OnSelectCard9() { SelectCard(8); }

void ARobotController::OnExecuteProgram()
{
	if (!GameMode) return;
	if (GameMode->CurrentState == EGameState::Programming)
	{
		GameMode->StartExecutionPhase();
	}
}

void ARobotController::OnUndoSelection()
{
	if (!GameMode) return;
	if (GameMode->CurrentState == EGameState::Programming)
	{
		GameMode->UndoLastSelection();
	}
}
