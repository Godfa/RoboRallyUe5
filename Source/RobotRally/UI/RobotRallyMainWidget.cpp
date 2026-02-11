// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotRallyMainWidget.h"
#include "ProgrammingDeckWidget.h"
#include "../RobotRallyGameMode.h"

void URobotRallyMainWidget::UpdateHealth(int32 CurrentHealth, int32 MaxHealth)
{
	// Cache values for helper methods
	CachedCurrentHealth = CurrentHealth;
	CachedMaxHealth = MaxHealth;

	// Trigger Blueprint event for visual update
	OnHealthChanged(CurrentHealth, MaxHealth);
}

void URobotRallyMainWidget::UpdateLives(int32 RemainingLives)
{
	// Cache value for helper methods
	CachedLives = RemainingLives;

	// Trigger Blueprint event for visual update
	OnLivesChanged(RemainingLives);
}

void URobotRallyMainWidget::UpdateCheckpoints(int32 CurrentCheckpoint, int32 TotalCheckpoints)
{
	// Cache values for helper methods
	CachedCurrentCheckpoint = CurrentCheckpoint;
	CachedTotalCheckpoints = TotalCheckpoints;

	// Trigger Blueprint event for visual update
	OnCheckpointsChanged(CurrentCheckpoint, TotalCheckpoints);
}

void URobotRallyMainWidget::UpdateGameState(EGameState NewState)
{
	// Cache value for helper methods
	CachedGameState = NewState;

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

float URobotRallyMainWidget::GetHealthPercent() const
{
	if (CachedMaxHealth <= 0)
	{
		return 0.0f;
	}
	return static_cast<float>(CachedCurrentHealth) / static_cast<float>(CachedMaxHealth);
}

FLinearColor URobotRallyMainWidget::GetHealthBarColor() const
{
	float HealthPercent = GetHealthPercent();

	// Green (>70%), Yellow (30-70%), Red (<30%)
	if (HealthPercent > 0.7f)
	{
		// Green: #7ED321
		return FLinearColor(0.49f, 0.83f, 0.13f, 1.0f);
	}
	else if (HealthPercent > 0.3f)
	{
		// Yellow: #F5A623
		return FLinearColor(0.96f, 0.65f, 0.14f, 1.0f);
	}
	else
	{
		// Red: #D0021B
		return FLinearColor(0.82f, 0.01f, 0.11f, 1.0f);
	}
}

FText URobotRallyMainWidget::GetHealthText() const
{
	FString HealthStr = FString::Printf(TEXT("%d/%d"), CachedCurrentHealth, CachedMaxHealth);
	return FText::FromString(HealthStr);
}

FText URobotRallyMainWidget::GetGameStateText() const
{
	switch (CachedGameState)
	{
	case EGameState::Programming:
		return FText::FromString(TEXT("PROGRAMMING"));
	case EGameState::Executing:
		return FText::FromString(TEXT("EXECUTING"));
	case EGameState::GameOver:
		// TODO: Determine win/lose from robot state
		return FText::FromString(TEXT("GAME OVER"));
	default:
		return FText::FromString(TEXT("UNKNOWN"));
	}
}

FLinearColor URobotRallyMainWidget::GetGameStatePanelColor() const
{
	switch (CachedGameState)
	{
	case EGameState::Programming:
		// Blue: #4A90E2
		return FLinearColor(0.29f, 0.56f, 0.89f, 1.0f);
	case EGameState::Executing:
		// Orange: #F5A623
		return FLinearColor(0.96f, 0.65f, 0.14f, 1.0f);
	case EGameState::GameOver:
		// Gold for victory, Red for defeat
		// TODO: Check robot alive state for proper color
		return FLinearColor(1.0f, 0.84f, 0.0f, 1.0f); // Gold: #FFD700
	default:
		return FLinearColor(0.5f, 0.5f, 0.5f, 1.0f); // Gray
	}
}
