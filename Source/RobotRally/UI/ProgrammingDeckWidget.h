// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../RobotRallyGameMode.h"
#include "ProgrammingDeckWidget.generated.h"

// Forward declarations
class UCardWidget;
class UUniformGridPanel;
class UHorizontalBox;

/**
 * Base C++ class for programming deck widget (hand + registers)
 * Manages card widgets and updates display based on network data
 * Visual layout is implemented in Blueprint child class (WBP_ProgrammingDeck)
 */
UCLASS()
class ROBOTRALLY_API UProgrammingDeckWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Current hand cards (replicated from PlayerState or GameMode) */
	UPROPERTY(BlueprintReadOnly, Category = "Deck")
	TArray<FRobotCard> HandCards;

	/** Current register slots (indices into hand array, -1 = empty) */
	UPROPERTY(BlueprintReadOnly, Category = "Deck")
	TArray<int32> RegisterSlots;

	/** Hand grid panel (3x3 for 9 cards) - bind in Blueprint with meta=(BindWidget) */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Deck")
	UUniformGridPanel* HandGridPanel;

	/** Register horizontal box (5 slots) - bind in Blueprint with meta=(BindWidget) */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Deck")
	UHorizontalBox* RegisterBox;

	/** Card widget class to instantiate (set in Blueprint) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deck")
	TSubclassOf<UCardWidget> CardWidgetClass;

	/**
	 * Update hand cards display
	 * @param NewHandCards Card data array from PlayerState or GameMode
	 */
	UFUNCTION(BlueprintCallable, Category = "Deck")
	void UpdateHandCards(const TArray<FRobotCard>& NewHandCards);

	/**
	 * Update register slots display
	 * @param NewRegisterSlots Hand indices in registers (-1 = empty)
	 */
	UFUNCTION(BlueprintCallable, Category = "Deck")
	void UpdateRegisterSlots(const TArray<int32>& NewRegisterSlots);

	/**
	 * Handle card selection (called by UCardWidget::OnCardClicked in Phase 7)
	 * @param HandIndex Index of card in hand array
	 */
	UFUNCTION(BlueprintCallable, Category = "Deck")
	void OnCardSelected(int32 HandIndex);

protected:
	/** Widget pool for hand cards (reused to avoid recreate overhead) */
	UPROPERTY()
	TArray<UCardWidget*> HandCardWidgets;

	/** Widget pool for register cards (reused to avoid recreate overhead) */
	UPROPERTY()
	TArray<UCardWidget*> RegisterCardWidgets;

	/** Rebuild hand widgets based on current HandCards array */
	void RebuildHandWidgets();

	/** Rebuild register widgets based on current RegisterSlots array */
	void RebuildRegisterWidgets();
};
