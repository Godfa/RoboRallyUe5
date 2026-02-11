// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotRallyMainWidget.h"
#include "ProgrammingDeckWidget.h"
#include "../RobotRallyGameMode.h"

void URobotRallyMainWidget::UpdateHealth(int32 CurrentHealth, int32 MaxHealth)
{
	// Trigger Blueprint event for visual update
	OnHealthChanged(CurrentHealth, MaxHealth);
}

void URobotRallyMainWidget::UpdateLives(int32 RemainingLives)
{
	// Trigger Blueprint event for visual update
	OnLivesChanged(RemainingLives);
}

void URobotRallyMainWidget::UpdateCheckpoints(int32 CurrentCheckpoint, int32 TotalCheckpoints)
{
	// Trigger Blueprint event for visual update
	OnCheckpointsChanged(CurrentCheckpoint, TotalCheckpoints);
}

void URobotRallyMainWidget::UpdateGameState(EGameState NewState)
{
	// Trigger Blueprint event for visual update
	OnGameStateChanged(NewState);
}

void URobotRallyMainWidget::AddEventMessage(const FString& Message, FLinearColor MessageColor)
{
	// Trigger Blueprint event for visual update
	OnEventMessageAdded(Message, MessageColor);
}
