// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../RobotRallyGameMode.h"
#include "RobotRallyMainWidget.generated.h"

// Forward declarations
class UProgrammingDeckWidget;
class UVerticalBox;

/**
 * Event message types for color coding and icon selection
 */
UENUM(BlueprintType)
enum class EEventMessageType : uint8
{
	Success		UMETA(DisplayName = "Success"),		// Green - Checkpoint collected, etc.
	Warning		UMETA(DisplayName = "Warning"),		// Yellow - Wrong checkpoint, etc.
	Error		UMETA(DisplayName = "Error"),		// Red - Damage taken, death, etc.
	Info		UMETA(DisplayName = "Info")			// White - General information
};

/**
 * Root HUD container widget
 * Manages all UI elements: health, lives, checkpoints, game state, programming deck, event log
 * Visual layout is implemented in Blueprint child class (WBP_MainHUD)
 */
UCLASS()
class ROBOTRALLY_API URobotRallyMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Programming deck widget (hand + registers) - bind in Blueprint with meta=(BindWidget) */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "HUD")
	UProgrammingDeckWidget* ProgrammingDeck;

	/** Event log vertical box container - bind in Blueprint with meta=(BindWidget) */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "HUD")
	UVerticalBox* EventLogBox;

	/** Widget class for event messages (set in Blueprint) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
	TSubclassOf<UUserWidget> EventMessageWidgetClass;

	/**
	 * Update health display
	 * @param CurrentHealth Current health points
	 * @param MaxHealth Maximum health points
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHealth(int32 CurrentHealth, int32 MaxHealth);

	/**
	 * Update lives display
	 * @param RemainingLives Number of lives remaining
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateLives(int32 RemainingLives);

	/**
	 * Update checkpoint progress display
	 * @param CurrentCheckpoint Current checkpoint index
	 * @param TotalCheckpoints Total number of checkpoints
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateCheckpoints(int32 CurrentCheckpoint, int32 TotalCheckpoints);

	/**
	 * Update game state display (Programming, Executing, GameOver)
	 * @param NewState Current game state
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateGameState(EGameState NewState);

	/**
	 * Add event log message with color
	 * @param Message Text to display
	 * @param MessageColor Color for message text
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void AddEventMessage(const FString& Message, FLinearColor MessageColor);

	/**
	 * Add event log message with type
	 * @param Message Text to display
	 * @param MessageType Type of message (Success, Warning, Error, Info)
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void AddEventMessageTyped(const FString& Message, EEventMessageType MessageType);

	/**
	 * Get color for message type
	 * @param MessageType Type of message
	 * @return Color for message
	 */
	UFUNCTION(BlueprintPure, Category = "HUD")
	static FLinearColor GetMessageTypeColor(EEventMessageType MessageType);

	/**
	 * Get icon name for message type
	 * @param MessageType Type of message
	 * @return Icon identifier string
	 */
	UFUNCTION(BlueprintPure, Category = "HUD")
	static FString GetMessageTypeIcon(EEventMessageType MessageType);

	/**
	 * Blueprint event triggered when health changes
	 * Implement in WBP_MainHUD to update health bar and text
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnHealthChanged(int32 CurrentHealth, int32 MaxHealth);

	/**
	 * Blueprint event triggered when lives change
	 * Implement in WBP_MainHUD to update heart icons
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnLivesChanged(int32 RemainingLives);

	/**
	 * Blueprint event triggered when checkpoint progress changes
	 * Implement in WBP_MainHUD to update flag icons
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnCheckpointsChanged(int32 CurrentCheckpoint, int32 TotalCheckpoints);

	/**
	 * Blueprint event triggered when game state changes
	 * Implement in WBP_MainHUD to update phase panel (color, text, icon)
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnGameStateChanged(EGameState NewState);

	/**
	 * Blueprint event triggered when event message is added
	 * Implement in WBP_MainHUD to create message widget with fade animation
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnEventMessageAdded(const FString& Message, FLinearColor MessageColor);

	/**
	 * Update programming deck visibility based on game state
	 * Show in Programming state, hide in Executing/GameOver
	 * @param bVisible True to show deck, false to hide
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetProgrammingDeckVisible(bool bVisible);

	/**
	 * Get current visibility state of programming deck
	 * @return True if deck is visible
	 */
	UFUNCTION(BlueprintPure, Category = "HUD")
	bool IsProgrammingDeckVisible() const;

	/**
	 * Get health percentage (0.0 to 1.0) for progress bar binding
	 * @return Health percentage as float
	 */
	UFUNCTION(BlueprintPure, Category = "HUD|Health")
	float GetHealthPercent() const;

	/**
	 * Get health bar color based on health percentage
	 * Green (>70%), Yellow (30-70%), Red (<30%)
	 * @return Color for health bar
	 */
	UFUNCTION(BlueprintPure, Category = "HUD|Health")
	FLinearColor GetHealthBarColor() const;

	/**
	 * Get formatted health text ("7/10")
	 * @return Formatted health string
	 */
	UFUNCTION(BlueprintPure, Category = "HUD|Health")
	FText GetHealthText() const;

	/**
	 * Get game state display text ("PROGRAMMING", "EXECUTING", etc.)
	 * @return Game state text
	 */
	UFUNCTION(BlueprintPure, Category = "HUD|GameState")
	FText GetGameStateText() const;

	/**
	 * Get game state panel background color
	 * Blue (Programming), Orange (Executing), Gold (Victory), Red (GameOver)
	 * @return Background color for game state panel
	 */
	UFUNCTION(BlueprintPure, Category = "HUD|GameState")
	FLinearColor GetGameStatePanelColor() const;

	/** Maximum number of event messages to keep in log */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
	int32 MaxEventMessages = 8;

	/** Duration in seconds before event message fades out */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
	float EventMessageDuration = 5.0f;

private:
	/** Current visibility state of programming deck */
	bool bProgrammingDeckVisible = true;

	/** Cached health values for helper methods */
	int32 CachedCurrentHealth = 10;
	int32 CachedMaxHealth = 10;

	/** Cached lives value for helper methods */
	int32 CachedLives = 3;

	/** Cached checkpoint values for helper methods */
	int32 CachedCurrentCheckpoint = 0;
	int32 CachedTotalCheckpoints = 5;

	/** Cached game state for helper methods */
	EGameState CachedGameState = EGameState::Programming;

	/** Remove oldest message from event log if limit exceeded */
	void TrimEventLog();
};
