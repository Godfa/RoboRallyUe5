// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "CardWidget.h"
#include "../RobotRallyGameMode.h"

void UCardWidget::SetCardData(const FRobotCard& InCard, int32 InHandIndex)
{
	CardData = InCard;
	HandIndex = InHandIndex;

	// Trigger Blueprint event to update visuals
	OnCardDataChanged();
}

void UCardWidget::SetCardState(ECardWidgetState NewState)
{
	if (CurrentState != NewState)
	{
		CurrentState = NewState;

		// Trigger Blueprint event to update visuals
		OnCardStateChanged();
	}
}

void UCardWidget::OnCardClicked()
{
	// Implementation in Phase 7 (mouse support)
	// Will call parent ProgrammingDeckWidget->OnCardSelected(HandIndex)
}

FText UCardWidget::GetCardActionName() const
{
	switch (CardData.Action)
	{
	case ECardAction::Move1:
		return FText::FromString(TEXT("Move 1"));
	case ECardAction::Move2:
		return FText::FromString(TEXT("Move 2"));
	case ECardAction::Move3:
		return FText::FromString(TEXT("Move 3"));
	case ECardAction::MoveBack:
		return FText::FromString(TEXT("Move Back"));
	case ECardAction::RotateRight:
		return FText::FromString(TEXT("Rotate Right"));
	case ECardAction::RotateLeft:
		return FText::FromString(TEXT("Rotate Left"));
	case ECardAction::UTurn:
		return FText::FromString(TEXT("U-Turn"));
	default:
		return FText::FromString(TEXT("Unknown"));
	}
}

FLinearColor UCardWidget::GetCardTypeColor() const
{
	// Movement cards: Blue gradient
	if (CardData.Action == ECardAction::Move1 ||
		CardData.Action == ECardAction::Move2 ||
		CardData.Action == ECardAction::Move3)
	{
		return FLinearColor(0.29f, 0.56f, 0.89f, 1.0f); // Light Blue (#4A90E2)
	}
	// Rotation cards: Green gradient
	else if (CardData.Action == ECardAction::RotateRight ||
			 CardData.Action == ECardAction::RotateLeft)
	{
		return FLinearColor(0.49f, 0.83f, 0.13f, 1.0f); // Green (#7ED321)
	}
	// Special cards (UTurn, MoveBack): Orange gradient
	else
	{
		return FLinearColor(0.96f, 0.65f, 0.14f, 1.0f); // Orange (#F5A623)
	}
}

FLinearColor UCardWidget::GetPriorityBadgeColor() const
{
	// High priority (600+): Gold/yellow
	if (CardData.Priority >= 600)
	{
		return FLinearColor(1.0f, 0.84f, 0.0f, 1.0f); // Gold (#FFD700)
	}
	// Medium priority (300-599): Silver/gray
	else if (CardData.Priority >= 300)
	{
		return FLinearColor(0.75f, 0.75f, 0.75f, 1.0f); // Silver (#C0C0C0)
	}
	// Low priority (0-299): Bronze/brown
	else
	{
		return FLinearColor(0.8f, 0.5f, 0.2f, 1.0f); // Bronze (#CD7F32)
	}
}

FString UCardWidget::GetCardIconName() const
{
	switch (CardData.Action)
	{
	case ECardAction::Move1:
		return TEXT("T_Icon_Move1");
	case ECardAction::Move2:
		return TEXT("T_Icon_Move2");
	case ECardAction::Move3:
		return TEXT("T_Icon_Move3");
	case ECardAction::MoveBack:
		return TEXT("T_Icon_MoveBack");
	case ECardAction::RotateRight:
		return TEXT("T_Icon_RotateRight");
	case ECardAction::RotateLeft:
		return TEXT("T_Icon_RotateLeft");
	case ECardAction::UTurn:
		return TEXT("T_Icon_UTurn");
	default:
		return TEXT("T_Icon_Unknown");
	}
}
