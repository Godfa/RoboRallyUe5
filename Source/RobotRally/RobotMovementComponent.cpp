// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotMovementComponent.h"
#include "GridManager.h"
#include "RobotPawn.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"

URobotMovementComponent::URobotMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	SetIsReplicatedByDefault(true);
}

void URobotMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	TargetLocation = GetOwner()->GetActorLocation();
	TargetRotation = GetOwner()->GetActorRotation();

	// Auto-find GridManager if not assigned in editor
	if (!GridManager)
	{
		GridManager = Cast<AGridManager>(
			UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass())
		);
	}

	if (!GridManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("URobotMovementComponent: No GridManager found! Move validation disabled."));
	}
}

void URobotMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URobotMovementComponent, Rep_TargetLocation);
	DOREPLIFETIME(URobotMovementComponent, Rep_TargetRotation);
}

void URobotMovementComponent::OnRep_TargetLocation()
{
	// Client receives new target from server - start interpolating
	TargetLocation = Rep_TargetLocation;
	bIsMoving = true;
}

void URobotMovementComponent::OnRep_TargetRotation()
{
	// Client receives new rotation target from server - start interpolating
	TargetRotation = Rep_TargetRotation;
	bIsRotating = true;
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

void URobotMovementComponent::GetDirectionDelta(EGridDirection Dir, int32& OutDX, int32& OutDY)
{
	switch (Dir)
	{
	case EGridDirection::North: OutDX = 1;  OutDY = 0;  break;
	case EGridDirection::East:  OutDX = 0;  OutDY = 1;  break;
	case EGridDirection::South: OutDX = -1; OutDY = 0;  break;
	case EGridDirection::West:  OutDX = 0;  OutDY = -1; break;
	}
}

void URobotMovementComponent::InitializeGridPosition(int32 InGridX, int32 InGridY, EGridDirection InFacing)
{
	CurrentGridX = InGridX;
	CurrentGridY = InGridY;
	FacingDirection = InFacing;

	// Sync target location/rotation with the grid position
	if (GridManager)
	{
		TargetLocation = GridManager->GridToWorld(FIntVector(InGridX, InGridY, 0));
		TargetLocation.Z = GetOwner()->GetActorLocation().Z;
	}
	else
	{
		TargetLocation = GetOwner()->GetActorLocation();
	}
	TargetRotation = GetOwner()->GetActorRotation();
	Rep_TargetLocation = TargetLocation;
	Rep_TargetRotation = TargetRotation;
}

ARobotPawn* URobotMovementComponent::FindRobotAtPosition(int32 X, int32 Y) const
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	ARobotPawn* OwnerRobot = Cast<ARobotPawn>(GetOwner());

	// Search all robot pawns in the world
	for (TActorIterator<ARobotPawn> It(World); It; ++It)
	{
		ARobotPawn* Robot = *It;
		if (Robot && Robot != OwnerRobot && Robot->bIsAlive)
		{
			if (Robot->GridX == X && Robot->GridY == Y)
			{
				return Robot;
			}
		}
	}

	return nullptr;
}

bool URobotMovementComponent::TryPushRobot(ARobotPawn* RobotToPush, int32 DX, int32 DY)
{
	if (!RobotToPush || !RobotToPush->RobotMovement) return false;

	// Calculate where the robot would be pushed to
	int32 PushX = RobotToPush->GridX + DX;
	int32 PushY = RobotToPush->GridY + DY;

	// Check if push destination is in bounds
	// (Pits and hazards are OK - that's the point of pushing!)
	if (GridManager)
	{
		if (!GridManager->IsInBounds(PushX, PushY))
		{
			UE_LOG(LogTemp, Log, TEXT("TryPushRobot: Cannot push - out of bounds at (%d, %d)"), PushX, PushY);
			return false;
		}
	}

	// Check if another robot is blocking the push destination
	ARobotPawn* BlockingRobot = FindRobotAtPosition(PushX, PushY);
	if (BlockingRobot)
	{
		// Try to recursively push that robot (chain pushing)
		if (!TryPushRobot(BlockingRobot, DX, DY))
		{
			UE_LOG(LogTemp, Log, TEXT("TryPushRobot: Cannot push - chain blocked"));
			return false;
		}
	}

	// Push successful - move the robot (even if it's into a pit/hazard!)
	if (GridManager)
	{
		FVector PushWorldPos = GridManager->GridToWorld(FIntVector(PushX, PushY, 0));
		RobotToPush->RobotMovement->MoveToWorldPosition(PushWorldPos);
		RobotToPush->RobotMovement->SetGridPosition(PushX, PushY);
	}

	UE_LOG(LogTemp, Log, TEXT("TryPushRobot: Pushed robot from (%d, %d) to (%d, %d)"),
		RobotToPush->GridX - DX, RobotToPush->GridY - DY, PushX, PushY);
	return true;
}

void URobotMovementComponent::MoveInGrid(int32 Distance)
{
	if (bIsMoving)
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveInGrid called while still moving; ignoring."));
		return;
	}

	int32 DX, DY;
	GetDirectionDelta(FacingDirection, DX, DY);

	// For MoveBack (Distance < 0), reverse direction
	int32 StepDir = (Distance >= 0) ? 1 : -1;
	int32 AbsDistance = FMath::Abs(Distance);

	int32 ValidSteps = 0;

	for (int32 Step = 1; Step <= AbsDistance; ++Step)
	{
		int32 NextX = CurrentGridX + DX * StepDir * Step;
		int32 NextY = CurrentGridY + DY * StepDir * Step;

		// Check for robot collision and try to push
		ARobotPawn* BlockingRobot = FindRobotAtPosition(NextX, NextY);
		if (BlockingRobot)
		{
			// Try to push the blocking robot in our movement direction
			bool bPushSuccessful = TryPushRobot(BlockingRobot, DX * StepDir, DY * StepDir);
			if (!bPushSuccessful)
			{
				UE_LOG(LogTemp, Log, TEXT("MoveInGrid: Blocked by robot at (%d, %d) after %d valid steps"), NextX, NextY, ValidSteps);
				break;
			}
			// Push succeeded, continue moving
		}

		// Check for tile validity
		if (GridManager)
		{
			FIntVector NextCoords(NextX, NextY, 0);
			if (!GridManager->IsValidTile(NextCoords))
			{
				UE_LOG(LogTemp, Log, TEXT("MoveInGrid: Blocked at tile (%d, %d) after %d valid steps"), NextX, NextY, ValidSteps);
				break;
			}
		}

		ValidSteps = Step;
	}

	// Update grid position
	CurrentGridX += DX * StepDir * ValidSteps;
	CurrentGridY += DY * StepDir * ValidSteps;

	// Compute world-space target
	if (GridManager)
	{
		TargetLocation = GridManager->GridToWorld(FIntVector(CurrentGridX, CurrentGridY, 0));
		TargetLocation.Z = GetOwner()->GetActorLocation().Z;
	}
	else
	{
		TargetLocation += FVector(DX * StepDir * ValidSteps * GridSize, DY * StepDir * ValidSteps * GridSize, 0.0f);
	}

	if (ValidSteps > 0)
	{
		bIsMoving = true;
	}

	// Sync replicated target for clients
	Rep_TargetLocation = TargetLocation;

	// Notify listeners of new grid position
	OnGridPositionChanged.Broadcast(CurrentGridX, CurrentGridY);
}

void URobotMovementComponent::MoveToWorldPosition(FVector NewTarget)
{
	TargetLocation = NewTarget;
	TargetLocation.Z = GetOwner()->GetActorLocation().Z;
	bIsMoving = true;
	Rep_TargetLocation = TargetLocation;
}

void URobotMovementComponent::SetGridPosition(int32 NewX, int32 NewY)
{
	CurrentGridX = NewX;
	CurrentGridY = NewY;
	OnGridPositionChanged.Broadcast(CurrentGridX, CurrentGridY);
}

void URobotMovementComponent::RotateInGrid(int32 Steps)
{
	if (bIsRotating)
	{
		UE_LOG(LogTemp, Warning, TEXT("RotateInGrid called while still rotating; ignoring."));
		return;
	}

	// Update discrete facing direction
	int32 Current = static_cast<int32>(FacingDirection);
	int32 NewDir = ((Current + Steps) % 4 + 4) % 4;
	FacingDirection = static_cast<EGridDirection>(NewDir);

	// Update target rotation and snap to nearest 90 degrees
	TargetRotation.Yaw += Steps * 90.0f;
	TargetRotation.Yaw = FMath::RoundToFloat(TargetRotation.Yaw / 90.0f) * 90.0f;
	bIsRotating = true;
	Rep_TargetRotation = TargetRotation;
}
