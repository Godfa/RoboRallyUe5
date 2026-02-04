// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RobotRallyGameMode.generated.h"

class AGridManager;
class ARobotPawn;

UENUM(BlueprintType)
enum class ECardAction : uint8
{
	Move1,
	Move2,
	Move3,
	MoveBack,
	RotateRight,
	RotateLeft,
	UTurn
};

USTRUCT(BlueprintType)
struct FRobotCard
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardAction Action = ECardAction::Move1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;
};

UENUM(BlueprintType)
enum class EGameState : uint8
{
	Programming,
	Executing,
	GameOver
};

UCLASS()
class ROBOTRALLY_API ARobotRallyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARobotRallyGameMode();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	EGameState CurrentState;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartExecutionPhase();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartProgrammingPhase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|TestScene")
	AGridManager* GridManagerInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game|TestScene")
	ARobotPawn* TestRobot;

private:
	void ProcessNextRegister();
	void SetupTestScene();

	int32 CurrentRegister = 0;
};
