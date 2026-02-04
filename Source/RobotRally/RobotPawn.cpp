// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotPawn.h"
#include "RobotMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"

ARobotPawn::ARobotPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize movement component
	RobotMovement = CreateDefaultSubobject<URobotMovementComponent>(TEXT("RobotMovement"));

	// Disable gravity - this is a board game, not a platformer
	if (UCharacterMovementComponent* CMC = GetCharacterMovement())
	{
		CMC->GravityScale = 0.0f;
		CMC->DefaultLandMovementMode = MOVE_Flying;
		CMC->SetMovementMode(MOVE_Flying);
	}

	// Default starting grid position
	GridX = 0;
	GridY = 0;
}

void ARobotPawn::BeginPlay()
{
	Super::BeginPlay();

	if (RobotMovement)
	{
		// Bind delegate for grid position sync
		RobotMovement->OnGridPositionChanged.AddDynamic(this, &ARobotPawn::OnGridPositionUpdated);

		// Determine initial facing direction from actor yaw
		float NormYaw = FMath::Fmod(GetActorRotation().Yaw + 360.0f, 360.0f);
		EGridDirection InitialFacing = EGridDirection::North;
		if (NormYaw >= 315.0f || NormYaw < 45.0f)
			InitialFacing = EGridDirection::North;
		else if (NormYaw >= 45.0f && NormYaw < 135.0f)
			InitialFacing = EGridDirection::East;
		else if (NormYaw >= 135.0f && NormYaw < 225.0f)
			InitialFacing = EGridDirection::South;
		else
			InitialFacing = EGridDirection::West;

		RobotMovement->InitializeGridPosition(GridX, GridY, InitialFacing);
	}
}

void ARobotPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Simple keyboard input for testing
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;
	if (RobotMovement && RobotMovement->IsMoving()) return;
	if (RobotMovement && RobotMovement->IsRotating()) return;

	if (PC->WasInputKeyJustPressed(EKeys::W))
		ExecuteMoveCommand(1);
	else if (PC->WasInputKeyJustPressed(EKeys::S))
		ExecuteMoveCommand(-1);
	else if (PC->WasInputKeyJustPressed(EKeys::D))
		ExecuteRotateCommand(1);
	else if (PC->WasInputKeyJustPressed(EKeys::A))
		ExecuteRotateCommand(-1);
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

void ARobotPawn::OnGridPositionUpdated(int32 NewGridX, int32 NewGridY)
{
	GridX = NewGridX;
	GridY = NewGridY;
}
