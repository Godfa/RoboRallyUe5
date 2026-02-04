// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RobotPawn.generated.h"

class URobotMovementComponent;
class UStaticMeshComponent;

UCLASS()
class ROBOTRALLY_API ARobotPawn : public ACharacter
{
	GENERATED_BODY()

public:
	ARobotPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Movement component for grid-based logic
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot", meta = (AllowPrivateAccess = "true"))
	URobotMovementComponent* RobotMovement;

	// Visual components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot|Visual")
	UStaticMeshComponent* BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot|Visual")
	UStaticMeshComponent* DirectionIndicator;

	// Grid coordinates
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot|Grid")
	int32 GridX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot|Grid")
	int32 GridY;

	// Executing a command from a card
	UFUNCTION(BlueprintCallable, Category = "Robot|Actions")
	void ExecuteMoveCommand(int32 Distance);

	UFUNCTION(BlueprintCallable, Category = "Robot|Actions")
	void ExecuteRotateCommand(int32 Steps); // 1 = 90 deg right, -1 = 90 deg left

private:
	UFUNCTION()
	void OnGridPositionUpdated(int32 NewGridX, int32 NewGridY);
};
