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
	// Implementation in Phase 3
	// Creates/reuses card widgets and populates HandGridPanel
}

void UProgrammingDeckWidget::RebuildRegisterWidgets()
{
	// Implementation in Phase 3
	// Creates/reuses card widgets and populates RegisterBox
}
