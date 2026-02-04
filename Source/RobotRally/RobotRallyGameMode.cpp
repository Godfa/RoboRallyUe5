// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotRallyGameMode.h"

ARobotRallyGameMode::ARobotRallyGameMode()
{
	CurrentState = EGameState::Programming;
}

void ARobotRallyGameMode::BeginPlay()
{
	Super::BeginPlay();
	StartProgrammingPhase();
}

void ARobotRallyGameMode::StartProgrammingPhase()
{
	CurrentState = EGameState::Programming;
	CurrentRegister = 0;
	// Logic to deal cards to players would go here
}

void ARobotRallyGameMode::StartExecutionPhase()
{
	CurrentState = EGameState::Executing;
	ProcessNextRegister();
}

void ARobotRallyGameMode::ProcessNextRegister()
{
	if (CurrentRegister >= 5)
	{
		StartProgrammingPhase();
		return;
	}

	// Logic to trigger robot actions based on cards in CurrentRegister
	// After all robots move, increment register
	CurrentRegister++;
	
	// In a real implementation, we would wait for animations to finish
	// For now, we'll just log or use a timer
}
