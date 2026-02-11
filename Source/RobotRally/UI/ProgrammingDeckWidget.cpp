// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "ProgrammingDeckWidget.h"
#include "CardWidget.h"
#include "../RobotRallyGameMode.h"
#include "Components/UniformGridPanel.h"
#include "Components/HorizontalBox.h"

void UProgrammingDeckWidget::UpdateHandCards(const TArray<FRobotCard>& NewHandCards)
{
	HandCards = NewHandCards;
	RebuildHandWidgets();
}

void UProgrammingDeckWidget::UpdateRegisterSlots(const TArray<int32>& NewRegisterSlots)
{
	RegisterSlots = NewRegisterSlots;
	RebuildRegisterWidgets();
}

void UProgrammingDeckWidget::OnCardSelected(int32 HandIndex)
{
	// Implementation in Phase 7 (mouse support)
	// Will call RobotController->SelectCard(HandIndex) to reuse existing RPC logic
}

void UProgrammingDeckWidget::RebuildHandWidgets()
{
	if (!HandGridPanel || !CardWidgetClass)
	{
		return;
	}

	// Clear existing widgets from grid
	HandGridPanel->ClearChildren();

	// Determine which cards are in registers (to mark as InRegister state)
	TSet<int32> CardsInRegisters;
	for (int32 RegIndex : RegisterSlots)
	{
		if (RegIndex >= 0 && RegIndex < HandCards.Num())
		{
			CardsInRegisters.Add(RegIndex);
		}
	}

	// Create/reuse card widgets for hand
	for (int32 i = 0; i < HandCards.Num(); ++i)
	{
		UCardWidget* CardWidget = nullptr;

		// Reuse existing widget from pool if available
		if (i < HandCardWidgets.Num() && HandCardWidgets[i])
		{
			CardWidget = HandCardWidgets[i];
		}
		else
		{
			// Create new widget and add to pool
			CardWidget = CreateWidget<UCardWidget>(GetWorld(), CardWidgetClass);
			if (i >= HandCardWidgets.Num())
			{
				HandCardWidgets.Add(CardWidget);
			}
			else
			{
				HandCardWidgets[i] = CardWidget;
			}
		}

		if (CardWidget)
		{
			// Set card data
			CardWidget->SetCardData(HandCards[i], i);

			// Set visual state (InRegister if card is in a register, Default otherwise)
			ECardWidgetState State = CardsInRegisters.Contains(i) ? ECardWidgetState::InRegister : ECardWidgetState::Default;
			CardWidget->SetCardState(State);

			// Add to grid (3x3 layout: row = i/3, col = i%3)
			int32 Row = i / 3;
			int32 Col = i % 3;
			HandGridPanel->AddChildToUniformGrid(CardWidget, Row, Col);
		}
	}
}

void UProgrammingDeckWidget::RebuildRegisterWidgets()
{
	if (!RegisterBox || !CardWidgetClass)
	{
		return;
	}

	// Clear existing widgets from register box
	RegisterBox->ClearChildren();

	// Create 5 register slots
	for (int32 RegIndex = 0; RegIndex < 5; ++RegIndex)
	{
		UCardWidget* CardWidget = nullptr;

		// Reuse existing widget from pool if available
		if (RegIndex < RegisterCardWidgets.Num() && RegisterCardWidgets[RegIndex])
		{
			CardWidget = RegisterCardWidgets[RegIndex];
		}
		else
		{
			// Create new widget and add to pool
			CardWidget = CreateWidget<UCardWidget>(GetWorld(), CardWidgetClass);
			if (RegIndex >= RegisterCardWidgets.Num())
			{
				RegisterCardWidgets.Add(CardWidget);
			}
			else
			{
				RegisterCardWidgets[RegIndex] = CardWidget;
			}
		}

		if (CardWidget)
		{
			// Check if this register has a card
			int32 HandIndex = (RegIndex < RegisterSlots.Num()) ? RegisterSlots[RegIndex] : -1;

			if (HandIndex >= 0 && HandIndex < HandCards.Num())
			{
				// Register has a card - show it
				CardWidget->SetCardData(HandCards[HandIndex], HandIndex);
				CardWidget->SetCardState(ECardWidgetState::InRegister);
				CardWidget->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				// Register is empty - hide the widget or show placeholder
				// For now, just hide it; Blueprint can add empty slot visuals
				CardWidget->SetVisibility(ESlateVisibility::Hidden);
			}

			// Add to register box
			RegisterBox->AddChild(CardWidget);
		}
	}
}
