// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RobotPawn.generated.h"

class URobotMovementComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRobotDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCheckpointReached, int32, CheckpointNumber);

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

	// Customizable meshes (set in Blueprint or editor, falls back to engine shapes)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Robot|Visual|Meshes")
	UStaticMesh* BodyMeshAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Robot|Visual|Meshes")
	UStaticMesh* DirectionMeshAsset;

	// Robot body color
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Robot|Visual|Meshes")
	FLinearColor BodyColor = FLinearColor(0.2f, 0.5f, 0.9f);

	// Direction indicator color
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Robot|Visual|Meshes")
	FLinearColor DirectionColor = FLinearColor(0.9f, 0.8f, 0.1f);

	// Grid coordinates
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot|Grid")
	int32 GridX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot|Grid")
	int32 GridY;

	// Health system
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot|Health")
	int32 Health = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot|Health")
	int32 MaxHealth = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot|Health")
	bool bIsAlive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot|Health")
	int32 Lives = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot|Health")
	int32 MaxLives = 3;

	UFUNCTION(BlueprintCallable, Category = "Robot|Health")
	void ApplyDamage(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Robot|Health")
	void Respawn();

	UPROPERTY(BlueprintAssignable, Category = "Robot|Health")
	FOnRobotDeath OnDeath;

	// Checkpoint progress
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot|Checkpoint")
	int32 CurrentCheckpoint = 0;

	UFUNCTION(BlueprintCallable, Category = "Robot|Checkpoint")
	void ReachCheckpoint(int32 Number);

	UPROPERTY(BlueprintAssignable, Category = "Robot|Checkpoint")
	FOnCheckpointReached OnCheckpointReached;

	// Executing a command from a card
	UFUNCTION(BlueprintCallable, Category = "Robot|Actions")
	void ExecuteMoveCommand(int32 Distance);

	UFUNCTION(BlueprintCallable, Category = "Robot|Actions")
	void ExecuteRotateCommand(int32 Steps); // 1 = 90 deg right, -1 = 90 deg left

private:
	UFUNCTION()
	void OnGridPositionUpdated(int32 NewGridX, int32 NewGridY);

	// Respawn location (last checkpoint or starting position)
	FIntVector RespawnPosition = FIntVector(0, 0, 0);
};
