// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotPawn.h"
#include "RobotMovementComponent.h"

ARobotPawn::ARobotPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize movement component
	RobotMovement = CreateDefaultSubobject<URobotMovementComponent>(TEXT("RobotMovement"));
	
	// Default starting grid position
	GridX = 0;
	GridY = 0;
}

void ARobotPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ARobotPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARobotPawn::ExecuteMoveCommand(int32 Distance)
{
	if (RobotMovement)
	{
		RobotMovement->MoveInGrid(Distance);
	}
}

void ARobotPawn::ExecuteRotateCommand(int32 Steps)
{
	if (RobotMovement)
	{
		RobotMovement->RotateInGrid(Steps);
	}
}
