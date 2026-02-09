// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "RobotMovementComponent.generated.h"

class AGridManager;

UENUM(BlueprintType)
enum class EGridDirection : uint8
{
	North,	// +X
	East,	// +Y
	South,	// -X
	West	// -Y
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGridPositionChanged, int32, NewGridX, int32, NewGridY);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROBOTRALLY_API URobotMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URobotMovementComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Grid spacing (default 100 units)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float GridSize = 100.0f;

	// Reference to the level's GridManager for move validation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	AGridManager* GridManager = nullptr;

	// Move the owner robot forward or backward in the grid
	UFUNCTION(BlueprintCallable, Category = "GridMovement")
	void MoveInGrid(int32 Distance);

	// Rotate the owner robot
	UFUNCTION(BlueprintCallable, Category = "GridMovement")
	void RotateInGrid(int32 Steps);

	// Initialize grid position and facing direction
	UFUNCTION(BlueprintCallable, Category = "GridMovement")
	void InitializeGridPosition(int32 InGridX, int32 InGridY, EGridDirection InFacing);

	UFUNCTION(BlueprintPure, Category = "GridMovement")
	int32 GetCurrentGridX() const { return CurrentGridX; }

	UFUNCTION(BlueprintPure, Category = "GridMovement")
	int32 GetCurrentGridY() const { return CurrentGridY; }

	UFUNCTION(BlueprintPure, Category = "GridMovement")
	EGridDirection GetFacingDirection() const { return FacingDirection; }

	UFUNCTION(BlueprintPure, Category = "GridMovement")
	bool IsMoving() const { return bIsMoving; }

	UFUNCTION(BlueprintPure, Category = "GridMovement")
	bool IsRotating() const { return bIsRotating; }

	// Broadcast when grid coordinates change after a move
	UPROPERTY(BlueprintAssignable, Category = "GridMovement")
	FOnGridPositionChanged OnGridPositionChanged;

	// Move directly to a world position (for conveyor/external movement)
	UFUNCTION(BlueprintCallable, Category = "GridMovement")
	void MoveToWorldPosition(FVector NewTarget);

	// Force-set grid position without movement (used after conveyor updates)
	void SetGridPosition(int32 NewX, int32 NewY);

	// Returns (DX, DY) delta for a given direction
	static void GetDirectionDelta(EGridDirection Dir, int32& OutDX, int32& OutDY);

	// Replicated targets for client interpolation
	UPROPERTY(ReplicatedUsing = OnRep_TargetLocation)
	FVector Rep_TargetLocation;

	UPROPERTY(ReplicatedUsing = OnRep_TargetRotation)
	FRotator Rep_TargetRotation;

private:
	UFUNCTION()
	void OnRep_TargetLocation();

	UFUNCTION()
	void OnRep_TargetRotation();

	FVector TargetLocation;
	FRotator TargetRotation;

	bool bIsMoving = false;
	bool bIsRotating = false;

	EGridDirection FacingDirection = EGridDirection::North;
	int32 CurrentGridX = 0;
	int32 CurrentGridY = 0;
};
