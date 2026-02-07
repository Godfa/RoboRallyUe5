// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RobotRallyHUD.generated.h"

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
	virtual void DrawHUD() override;

	void AddEventMessage(const FString& Text, FColor Color = FColor::White);

private:
	void DrawCardSelection();

	TArray<FEventMessage> Messages;

	static constexpr float MESSAGE_DURATION = 5.0f;
	static constexpr int32 MAX_MESSAGES = 8;
};
