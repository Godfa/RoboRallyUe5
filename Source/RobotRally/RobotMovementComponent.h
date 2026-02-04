// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RobotMovementComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROBOTRALLY_API URobotMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URobotMovementComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Grid spacing (default 100 units)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float GridSize = 100.0f;

	// Move the owner robot forward or backward in the grid
	UFUNCTION(BlueprintCallable, Category = "GridMovement")
	void MoveInGrid(int32 Distance);

	// Rotate the owner robot
	UFUNCTION(BlueprintCallable, Category = "GridMovement")
	void RotateInGrid(int32 Steps);

private:
	// Internally handles the smooth interpolation to the target grid position
	FVector TargetLocation;
	FRotator TargetRotation;

	bool bIsMoving = false;
	bool bIsRotating = false;
};
