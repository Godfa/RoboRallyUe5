// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotPawn.h"
#include "RobotMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"

ARobotPawn::ARobotPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize movement component
	RobotMovement = CreateDefaultSubobject<URobotMovementComponent>(TEXT("RobotMovement"));

	// Shrink the capsule so it doesn't interfere
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCapsuleSize(30.0f, 30.0f);
	}

	// Disable gravity and rotation control from CharacterMovementComponent
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	if (UCharacterMovementComponent* CMC = GetCharacterMovement())
	{
		CMC->GravityScale = 0.0f;
		CMC->DefaultLandMovementMode = MOVE_Flying;
		CMC->SetMovementMode(MOVE_Flying);
		CMC->bOrientRotationToMovement = false;
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

#if ENABLE_DRAW_DEBUG
	// Draw robot as a large circle + arrow showing facing direction
	UWorld* World = GetWorld();
	if (World && RobotMovement)
	{
		FVector Pos = GetActorLocation();
		Pos.Z = 5.0f;

		// Big magenta circle for robot position
		DrawDebugCircle(World, Pos, 40.0f, 32, FColor::Magenta, false, -1.0f, 0, 4.0f,
			FVector(0, 1, 0), FVector(1, 0, 0), false);

		// Arrow showing facing direction
		FVector Forward = GetActorForwardVector();
		FVector ArrowEnd = Pos + Forward * 45.0f;
		DrawDebugDirectionalArrow(World, Pos, ArrowEnd, 25.0f, FColor::Magenta, false, -1.0f, 0, 4.0f);

		// Direction label
		const TCHAR* DirText = TEXT("?");
		switch (RobotMovement->GetFacingDirection())
		{
		case EGridDirection::North: DirText = TEXT("^ N"); break;
		case EGridDirection::East:  DirText = TEXT("> E"); break;
		case EGridDirection::South: DirText = TEXT("v S"); break;
		case EGridDirection::West:  DirText = TEXT("< W"); break;
		}

		FString Label = FString::Printf(TEXT("ROBOT (%d,%d) %s"), GridX, GridY, DirText);
		DrawDebugString(World, Pos + FVector(0, 0, 15.0f), Label, nullptr, FColor::Magenta, 0.0f, true);
	}
#endif

	// Simple keyboard input for testing
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	bool bBusy = RobotMovement && (RobotMovement->IsMoving() || RobotMovement->IsRotating());
	if (bBusy) return;

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
