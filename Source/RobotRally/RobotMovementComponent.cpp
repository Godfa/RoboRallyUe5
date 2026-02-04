// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotMovementComponent.h"
#include "GameFramework/Actor.h"

URobotMovementComponent::URobotMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	GridSize = 100.0f;
}

void URobotMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	TargetLocation = GetOwner()->GetActorLocation();
	TargetRotation = GetOwner()->GetActorRotation();
}

void URobotMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Smoothly interpolate to target location
	if (bIsMoving)
	{
		FVector CurrentLocation = GetOwner()->GetActorLocation();
		FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, 5.0f);
		GetOwner()->SetActorLocation(NewLocation);

		if (FVector::Dist(NewLocation, TargetLocation) < 1.0f)
		{
			GetOwner()->SetActorLocation(TargetLocation);
			bIsMoving = false;
		}
	}

	// Smoothly interpolate to target rotation
	if (bIsRotating)
	{
		FRotator CurrentRotation = GetOwner()->GetActorRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 10.0f);
		GetOwner()->SetActorRotation(NewRotation);

		if (CurrentRotation.Equals(TargetRotation, 0.1f))
		{
			GetOwner()->SetActorRotation(TargetRotation);
			bIsRotating = false;
		}
	}
}

void URobotMovementComponent::MoveInGrid(int32 Distance)
{
	FVector ForwardVector = GetOwner()->GetActorForwardVector();
	TargetLocation += ForwardVector * (Distance * GridSize);
	bIsMoving = true;
}

void URobotMovementComponent::RotateInGrid(int32 Steps)
{
	TargetRotation.Yaw += Steps * 90.0f;
	bIsRotating = true;
}
