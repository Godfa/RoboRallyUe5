// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotAIController.h"
#include "RobotPawn.h"
#include "RobotMovementComponent.h"
#include "GridManager.h"
#include "Engine/World.h"

ARobotAIController::ARobotAIController()
{
}

void ARobotAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ControlledRobot = Cast<ARobotPawn>(InPawn);
	GameMode = Cast<ARobotRallyGameMode>(GetWorld()->GetAuthGameMode());

	UE_LOG(LogTemp, Log, TEXT("RobotAIController::OnPossess - Robot: %s, Difficulty: %d"),
		ControlledRobot ? *ControlledRobot->GetName() : TEXT("NULL"),
		(int32)DifficultyLevel);
}

void ARobotAIController::StartCardSelection()
{
	if (!ControlledRobot || !GameMode) return;
	if (!ControlledRobot->bIsAlive) return;

	UE_LOG(LogTemp, Log, TEXT("AI StartCardSelection - Difficulty: %d"), (int32)DifficultyLevel);

	switch (DifficultyLevel)
	{
	case ERobotControllerType::AI_Easy:
		SelectCardsEasy();
		break;
	case ERobotControllerType::AI_Medium:
	case ERobotControllerType::AI_Hard:
		SelectCardsMedium();
		break;
	default:
		SelectCardsEasy();
		break;
	}

	// Signal ready to GameMode
	GameMode->OnControllerReady(this);
}

void ARobotAIController::SelectCardsEasy()
{
	// Easy AI: pick 5 random cards from hand
	FRobotProgram* Program = GameMode->RobotPrograms.FindByPredicate([this](const FRobotProgram& P)
	{
		return P.Robot == ControlledRobot;
	});

	if (!Program) return;

	int32 HandSize = Program->HandCards.Num();
	if (HandSize < ARobotRallyGameMode::NUM_REGISTERS) return;

	// Build list of available hand indices
	TArray<int32> Available;
	Available.Reserve(HandSize);
	for (int32 i = 0; i < HandSize; ++i)
	{
		Available.Add(i);
	}

	// Shuffle and pick first 5
	for (int32 i = Available.Num() - 1; i > 0; --i)
	{
		int32 j = FMath::RandRange(0, i);
		Available.Swap(i, j);
	}

	for (int32 i = 0; i < ARobotRallyGameMode::NUM_REGISTERS && i < Available.Num(); ++i)
	{
		GameMode->SelectCardFromHand(ControlledRobot, Available[i]);
	}

	UE_LOG(LogTemp, Log, TEXT("AI Easy: Selected %d random cards"), ARobotRallyGameMode::NUM_REGISTERS);
}

void ARobotAIController::SelectCardsMedium()
{
	// Medium AI: greedy card selection toward next checkpoint
	FRobotProgram* Program = GameMode->RobotPrograms.FindByPredicate([this](const FRobotProgram& P)
	{
		return P.Robot == ControlledRobot;
	});

	if (!Program) return;

	int32 HandSize = Program->HandCards.Num();
	if (HandSize < ARobotRallyGameMode::NUM_REGISTERS)
	{
		SelectCardsEasy();
		return;
	}

	AGridManager* Grid = GameMode->GridManagerInstance;
	if (!Grid)
	{
		SelectCardsEasy();
		return;
	}

	FIntVector TargetPos = FindNextCheckpointPosition();
	FIntVector SimPos(ControlledRobot->GridX, ControlledRobot->GridY, 0);
	EGridDirection SimFacing = EGridDirection::North;

	if (ControlledRobot->RobotMovement)
	{
		SimFacing = ControlledRobot->RobotMovement->GetFacingDirection();
	}

	TSet<int32> UsedIndices;

	for (int32 Register = 0; Register < ARobotRallyGameMode::NUM_REGISTERS; ++Register)
	{
		float BestScore = -1e9f;
		int32 BestIndex = -1;

		for (int32 i = 0; i < HandSize; ++i)
		{
			if (UsedIndices.Contains(i)) continue;

			float Score = ScoreCard(Program->HandCards[i].Action, SimPos, SimFacing, TargetPos, Grid);

			if (Score > BestScore)
			{
				BestScore = Score;
				BestIndex = i;
			}
		}

		if (BestIndex >= 0)
		{
			GameMode->SelectCardFromHand(ControlledRobot, BestIndex);
			UsedIndices.Add(BestIndex);

			// Update simulation state for next register
			FIntVector NewPos;
			EGridDirection NewFacing;
			SimulateCardAction(Program->HandCards[BestIndex].Action, SimPos, SimFacing, NewPos, NewFacing);
			SimPos = NewPos;
			SimFacing = NewFacing;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("AI Medium: Selected %d cards toward checkpoint at (%d,%d)"),
		UsedIndices.Num(), TargetPos.X, TargetPos.Y);
}

float ARobotAIController::ScoreCard(ECardAction Action, FIntVector CurrentPos, EGridDirection CurrentFacing,
	FIntVector TargetPos, AGridManager* Grid) const
{
	FIntVector NewPos;
	EGridDirection NewFacing;
	SimulateCardAction(Action, CurrentPos, CurrentFacing, NewPos, NewFacing);

	// Manhattan distance before and after
	float DistBefore = FMath::Abs(CurrentPos.X - TargetPos.X) + FMath::Abs(CurrentPos.Y - TargetPos.Y);
	float DistAfter = FMath::Abs(NewPos.X - TargetPos.X) + FMath::Abs(NewPos.Y - TargetPos.Y);

	// Base score: distance improvement
	float Score = DistBefore - DistAfter;

	// Penalty for landing on dangerous tiles
	if (Grid)
	{
		ETileType NewTileType = Grid->GetTileType(NewPos);
		if (NewTileType == ETileType::Pit)
		{
			Score -= 100.0f;
		}
		else if (NewTileType == ETileType::Laser)
		{
			Score -= 2.0f;
		}

		// Penalty for going out of bounds
		if (!Grid->IsInBounds(NewPos.X, NewPos.Y))
		{
			Score -= 100.0f;
		}
	}

	// Small bonus for facing toward target (encourages alignment)
	int32 DX, DY;
	GetDirectionDelta(NewFacing, DX, DY);
	int32 TargetDX = FMath::Sign(TargetPos.X - NewPos.X);
	int32 TargetDY = FMath::Sign(TargetPos.Y - NewPos.Y);
	if (DX == TargetDX && DY == TargetDY)
	{
		Score += 0.5f;
	}

	return Score;
}

void ARobotAIController::SimulateCardAction(ECardAction Action, FIntVector InPos, EGridDirection InFacing,
	FIntVector& OutPos, EGridDirection& OutFacing) const
{
	OutPos = InPos;
	OutFacing = InFacing;

	int32 DX, DY;

	switch (Action)
	{
	case ECardAction::Move1:
	case ECardAction::Move2:
	case ECardAction::Move3:
	{
		int32 Distance = (Action == ECardAction::Move1) ? 1 :
		                 (Action == ECardAction::Move2) ? 2 : 3;
		GetDirectionDelta(InFacing, DX, DY);
		OutPos.X += DX * Distance;
		OutPos.Y += DY * Distance;
		break;
	}
	case ECardAction::MoveBack:
		GetDirectionDelta(InFacing, DX, DY);
		OutPos.X -= DX;
		OutPos.Y -= DY;
		break;
	case ECardAction::RotateRight:
		OutFacing = RotateDirection(InFacing, 1);
		break;
	case ECardAction::RotateLeft:
		OutFacing = RotateDirection(InFacing, -1);
		break;
	case ECardAction::UTurn:
		OutFacing = RotateDirection(InFacing, 2);
		break;
	}
}

FIntVector ARobotAIController::FindNextCheckpointPosition() const
{
	if (!ControlledRobot || !GameMode || !GameMode->GridManagerInstance)
	{
		return FIntVector(5, 5, 0);  // Fallback to grid center
	}

	int32 NextCheckpoint = ControlledRobot->CurrentCheckpoint + 1;
	AGridManager* Grid = GameMode->GridManagerInstance;

	// Search grid for the next checkpoint
	for (auto& Pair : Grid->GridMap)
	{
		if (Pair.Value.TileType == ETileType::Checkpoint &&
			Pair.Value.CheckpointNumber == NextCheckpoint)
		{
			return Pair.Key;
		}
	}

	// No checkpoint found - head toward grid center as fallback
	return FIntVector(Grid->Width / 2, Grid->Height / 2, 0);
}

EGridDirection ARobotAIController::RotateDirection(EGridDirection Dir, int32 Steps)
{
	int32 DirInt = static_cast<int32>(Dir);
	DirInt = ((DirInt + Steps) % 4 + 4) % 4;  // Ensure positive modulo
	return static_cast<EGridDirection>(DirInt);
}

void ARobotAIController::GetDirectionDelta(EGridDirection Dir, int32& DX, int32& DY)
{
	DX = 0;
	DY = 0;
	switch (Dir)
	{
	case EGridDirection::North: DX = 1; break;   // +X
	case EGridDirection::East:  DY = 1; break;   // +Y
	case EGridDirection::South: DX = -1; break;  // -X
	case EGridDirection::West:  DY = -1; break;  // -Y
	}
}
