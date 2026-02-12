// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../RobotRallyGameMode.h"
#include "CardWidget.generated.h"

/**
 * Card widget states for visual representation
 */
UENUM(BlueprintType)
enum class ECardWidgetState : uint8
{
	Default		UMETA(DisplayName = "Default"),
	Hover		UMETA(DisplayName = "Hover"),
	Selected	UMETA(DisplayName = "Selected"),
	InRegister	UMETA(DisplayName = "In Register"),
	Disabled	UMETA(DisplayName = "Disabled")
};

/**
 * Base C++ class for individual card widgets
 * Handles card data binding and state management
 * Visual representation is implemented in Blueprint child class (WBP_CardSlot)
 */
UCLASS()
class ROBOTRALLY_API UCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Current card data (action, priority) */
	UPROPERTY(BlueprintReadOnly, Category = "Card")
	FRobotCard CardData;

	/** Index in hand array (0-8, or -1 if in register) */
	UPROPERTY(BlueprintReadOnly, Category = "Card")
	int32 HandIndex = -1;

	/** Current visual state of the card */
	UPROPERTY(BlueprintReadOnly, Category = "Card")
	ECardWidgetState CurrentState = ECardWidgetState::Default;

	/**
	 * Set card data and hand index
	 * @param InCard Card data to display
	 * @param InHandIndex Index in hand array (0-8)
	 */
	UFUNCTION(BlueprintCallable, Category = "Card")
	void SetCardData(const FRobotCard& InCard, int32 InHandIndex);

	/**
	 * Blueprint event triggered when card data changes
	 * Implement in WBP_CardSlot to update visuals (icon, text, priority badge)
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Card")
	void OnCardDataChanged();

	/**
	 * Change card visual state
	 * @param NewState New visual state (Default, Hover, Selected, etc.)
	 */
	UFUNCTION(BlueprintCallable, Category = "Card")
	void SetCardState(ECardWidgetState NewState);

	/**
	 * Blueprint event triggered when card state changes
	 * Implement in WBP_CardSlot to update visuals (border, opacity, scale)
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Card")
	void OnCardStateChanged();

	/**
	 * Handle card click (for mouse support in Phase 7)
	 */
	UFUNCTION(BlueprintCallable, Category = "Card")
	void OnCardClicked();

	/**
	 * Get display name for card action (e.g., "Move 1", "Rotate Right")
	 * @return Formatted action name string
	 */
	UFUNCTION(BlueprintPure, Category = "Card")
	FText GetCardActionName() const;

	/**
	 * Get card type color for background (Blue for movement, Green for rotation, Orange for special)
	 * @return Color for card background
	 */
	UFUNCTION(BlueprintPure, Category = "Card")
	FLinearColor GetCardTypeColor() const;

	/**
	 * Get priority badge color based on priority value
	 * High priority (600+): Gold/yellow, Medium (300-599): Silver/gray, Low (0-299): Bronze/brown
	 * @return Color for priority badge
	 */
	UFUNCTION(BlueprintPure, Category = "Card")
	FLinearColor GetPriorityBadgeColor() const;

	/**
	 * Get icon name/path for card action (for texture selection in Blueprint)
	 * @return Icon identifier string
	 */
	UFUNCTION(BlueprintPure, Category = "Card")
	FString GetCardIconName() const;

	/**
	 * Get texture icon for current card action
	 * Returns nullptr if icon not assigned in ActionIcons map
	 * @return Texture2D asset for card icon
	 */
	UFUNCTION(BlueprintPure, Category = "Card")
	UTexture2D* GetCardIcon() const;

protected:
	/**
	 * Map of card actions to icon textures
	 * Assign these in Blueprint Class Defaults (WBP_CardSlot)
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Card Icons")
	TMap<ECardAction, UTexture2D*> ActionIcons;
};
