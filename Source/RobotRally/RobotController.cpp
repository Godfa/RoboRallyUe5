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

	// Cache game mode reference (only valid on server/standalone)
	GameMode = Cast<ARobotRallyGameMode>(GetWorld()->GetAuthGameMode());

	UE_LOG(LogTemp, Log, TEXT("RobotController::BeginPlay - GameMode: %s, NetMode: %d"),
		GameMode ? TEXT("Valid") : TEXT("NULL"), (int32)GetNetMode());
}

void ARobotController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Update robot reference when possessed
	ControlledRobot = Cast<ARobotPawn>(InPawn);

	UE_LOG(LogTemp, Log, TEXT("RobotController::OnPossess - Pawn: %s"),
		ControlledRobot ? *ControlledRobot->GetName() : TEXT("NULL"));
}

void ARobotController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UE_LOG(LogTemp, Log, TEXT("RobotController::SetupInputComponent called - InputComponent: %s"),
		InputComponent ? *InputComponent->GetClass()->GetName() : TEXT("NULL"));

	if (!InputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("InputComponent is NULL!"));
		return;
	}

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

	UE_LOG(LogTemp, Log, TEXT("RobotController: All input bindings set up successfully"));
}

void ARobotController::OnMoveForward()
{
	// WASD disabled in network mode (debug-only feature)
	if (GetNetMode() != NM_Standalone) return;

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
	if (GetNetMode() != NM_Standalone) return;

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
	if (GetNetMode() != NM_Standalone) return;

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
	if (GetNetMode() != NM_Standalone) return;

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
	if (GetNetMode() == NM_Standalone)
	{
		// Standalone: call GameMode directly
		if (!GameMode || !ControlledRobot) return;
		if (GameMode->CurrentState != EGameState::Programming) return;

		GameMode->SelectCardFromHand(ControlledRobot, CardIndex);

		// Check if we just filled the 5th register
		FRobotProgram* Program = GameMode->RobotPrograms.FindByPredicate([this](const FRobotProgram& P)
		{
			return P.Robot == ControlledRobot;
		});

		if (Program)
		{
			int32 FilledCount = 0;
			for (int32 Slot : Program->RegisterSlots)
			{
				if (Slot != -1) FilledCount++;
			}

			// Signal ready when all 5 registers are filled
			if (FilledCount == ARobotRallyGameMode::NUM_REGISTERS)
			{
				GameMode->OnControllerReady(this);
			}
		}
	}
	else
	{
		// Network: send RPC to server
		ServerSelectCard(CardIndex);
	}
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
	if (GetNetMode() == NM_Standalone)
	{
		if (!GameMode) return;
		if (GameMode->CurrentState == EGameState::Programming)
		{
			GameMode->OnControllerReady(this);
		}
	}
	else
	{
		ServerCommitProgram();
	}
}

void ARobotController::OnUndoSelection()
{
	if (GetNetMode() == NM_Standalone)
	{
		if (!GameMode) return;
		if (GameMode->CurrentState == EGameState::Programming)
		{
			GameMode->UndoLastSelection();
		}
	}
	else
	{
		ServerUndoSelection();
	}
}

// --- Server RPC implementations ---

bool ARobotController::ServerSelectCard_Validate(int32 HandIndex)
{
	return HandIndex >= 0 && HandIndex < 20; // Reasonable upper bound
}

void ARobotController::ServerSelectCard_Implementation(int32 HandIndex)
{
	if (!GameMode) GameMode = Cast<ARobotRallyGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;

	ARobotPawn* Robot = Cast<ARobotPawn>(GetPawn());
	if (!Robot) return;

	if (GameMode->CurrentState != EGameState::Programming)
	{
		ClientNotifyError(TEXT("Not in programming phase!"));
		return;
	}

	GameMode->SelectCardFromHand(Robot, HandIndex);

	// Check if all registers filled - auto-ready
	FRobotProgram* Program = GameMode->RobotPrograms.FindByPredicate([Robot](const FRobotProgram& P)
	{
		return P.Robot == Robot;
	});

	if (Program)
	{
		int32 FilledCount = 0;
		for (int32 Slot : Program->RegisterSlots)
		{
			if (Slot != -1) FilledCount++;
		}

		if (FilledCount == ARobotRallyGameMode::NUM_REGISTERS)
		{
			GameMode->OnControllerReady(this);
		}
	}
}

bool ARobotController::ServerUndoSelection_Validate()
{
	return true;
}

void ARobotController::ServerUndoSelection_Implementation()
{
	if (!GameMode) GameMode = Cast<ARobotRallyGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;

	if (GameMode->CurrentState != EGameState::Programming) return;

	// Undo for this player's robot specifically
	ARobotPawn* Robot = Cast<ARobotPawn>(GetPawn());
	if (!Robot) return;

	FRobotProgram* Program = GameMode->RobotPrograms.FindByPredicate([Robot](const FRobotProgram& P)
	{
		return P.Robot == Robot;
	});

	if (!Program) return;

	// Find the last filled register and clear it
	for (int32 i = ARobotRallyGameMode::NUM_REGISTERS - 1; i >= 0; --i)
	{
		if (Program->RegisterSlots[i] != -1)
		{
			GameMode->ShowEventMessage(FString::Printf(TEXT("Cleared R%d"), i + 1), FColor::Yellow);
			Program->RegisterSlots[i] = -1;
			GameMode->SyncPlayerStateHand(Robot);
			return;
		}
	}
}

bool ARobotController::ServerCommitProgram_Validate()
{
	return true;
}

void ARobotController::ServerCommitProgram_Implementation()
{
	if (!GameMode) GameMode = Cast<ARobotRallyGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;

	if (GameMode->CurrentState != EGameState::Programming) return;

	GameMode->OnControllerReady(this);
}

void ARobotController::ClientNotifyError_Implementation(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("Server error: %s"), *Message);
}
