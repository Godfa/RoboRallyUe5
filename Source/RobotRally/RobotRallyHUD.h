// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RobotRallyHUD.generated.h"

class ARobotRallyPlayerState;
class ARobotRallyGameState;
class URobotRallyMainWidget;

USTRUCT()
struct FEventMessage
{
	GENERATED_BODY()

	FString Text;
	FColor Color = FColor::White;
	float TimeRemaining = 5.0f;
};

UCLASS()
class ROBOTRALLY_API ARobotRallyHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

	void AddEventMessage(const FString& Text, FColor Color = FColor::White);

	/** Widget class to create for main HUD (set in Blueprint or GameMode) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<URobotRallyMainWidget> MainWidgetClass;

private:
	void DrawCardSelection();
	void DrawNetworkDebug();

	// Helper: get local player's PlayerState
	ARobotRallyPlayerState* GetLocalPlayerState() const;

	// Helper: get GameState
	ARobotRallyGameState* GetRobotRallyGameState() const;

	/** Update widget data from PlayerState (network) or GameMode (standalone) */
	void UpdateWidgetData();

	/** Main widget instance (created in BeginPlay) */
	UPROPERTY()
	URobotRallyMainWidget* MainWidget;

	TArray<FEventMessage> Messages;

	static constexpr float MESSAGE_DURATION = 5.0f;
	static constexpr int32 MAX_MESSAGES = 8;
};
