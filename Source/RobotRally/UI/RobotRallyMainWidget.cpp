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

	// Update programming deck visibility based on game state
	// Show in Programming, hide in Executing/GameOver
	bool bShouldShow = (NewState == EGameState::Programming);
	SetProgrammingDeckVisible(bShouldShow);
}

void URobotRallyMainWidget::AddEventMessage(const FString& Message, FLinearColor MessageColor)
{
	// Trigger Blueprint event for visual update
	OnEventMessageAdded(Message, MessageColor);
}

void URobotRallyMainWidget::SetProgrammingDeckVisible(bool bVisible)
{
	bProgrammingDeckVisible = bVisible;

	if (ProgrammingDeck)
	{
		// Set visibility (Visible or Collapsed to maintain layout)
		ESlateVisibility NewVisibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
		ProgrammingDeck->SetVisibility(NewVisibility);
	}
}

bool URobotRallyMainWidget::IsProgrammingDeckVisible() const
{
	return bProgrammingDeckVisible;
}
