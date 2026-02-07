// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotPawn.h"
#include "RobotMovementComponent.h"
#include "RobotRallyGameMode.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"

#if WITH_EDITORONLY_DATA
#include "Materials/Material.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#endif

ARobotPawn::ARobotPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize movement component
	RobotMovement = CreateDefaultSubobject<URobotMovementComponent>(TEXT("RobotMovement"));

	// Shrink the capsule so it doesn't interfere
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCapsuleSize(30.0f, 30.0f);
		Capsule->SetVisibility(false);
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

	// Hide the default skeletal mesh (ACharacter has one)
	if (GetMesh())
	{
		GetMesh()->SetVisibility(false);
	}

	// Body mesh component
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -15.0f));
	BodyMesh->SetRelativeScale3D(FVector(0.55f, 0.55f, 0.25f));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Direction indicator component
	DirectionIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DirectionCone"));
	DirectionIndicator->SetupAttachment(RootComponent);
	DirectionIndicator->SetRelativeLocation(FVector(25.0f, 0.0f, -10.0f));
	DirectionIndicator->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	DirectionIndicator->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.35f));
	DirectionIndicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Load default engine meshes as fallbacks
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFinder(
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeFinder(
		TEXT("/Engine/BasicShapes/Cone.Cone"));

	if (CylinderFinder.Succeeded())
	{
		BodyMesh->SetStaticMesh(CylinderFinder.Object);
	}
	if (ConeFinder.Succeeded())
	{
		DirectionIndicator->SetStaticMesh(ConeFinder.Object);
	}

	// Default starting grid position
	GridX = 0;
	GridY = 0;
}

void ARobotPawn::BeginPlay()
{
	Super::BeginPlay();

	// Apply custom meshes if set, otherwise keep engine defaults
	if (BodyMeshAsset)
	{
		BodyMesh->SetStaticMesh(BodyMeshAsset);
	}
	if (DirectionMeshAsset)
	{
		DirectionIndicator->SetStaticMesh(DirectionMeshAsset);
	}

	// Apply colored materials (created at runtime for proper Color parameter support)
	UMaterialInterface* BaseMat = nullptr;

#if WITH_EDITORONLY_DATA
	UMaterial* Mat = NewObject<UMaterial>(GetTransientPackage(), NAME_None, RF_Transient);
	Mat->MaterialDomain = MD_Surface;

	UMaterialExpressionVectorParameter* ColorParam =
		NewObject<UMaterialExpressionVectorParameter>(Mat);
	ColorParam->ParameterName = TEXT("Color");
	ColorParam->DefaultValue = FLinearColor::White;

	UMaterialEditorOnlyData* EditorData = Mat->GetEditorOnlyData();
	EditorData->ExpressionCollection.Expressions.Add(ColorParam);
	EditorData->BaseColor.Expression = ColorParam;
	EditorData->BaseColor.OutputIndex = 0;

	Mat->PreEditChange(nullptr);
	Mat->PostEditChange();
	BaseMat = Mat;
#endif

	if (BaseMat)
	{
		// Apply body color from UPROPERTY
		UMaterialInstanceDynamic* BodyMat = UMaterialInstanceDynamic::Create(BaseMat, this);
		BodyMat->SetVectorParameterValue(TEXT("Color"), BodyColor);
		BodyMesh->SetMaterial(0, BodyMat);

		// Apply direction indicator color from UPROPERTY
		UMaterialInstanceDynamic* ConeMat = UMaterialInstanceDynamic::Create(BaseMat, this);
		ConeMat->SetVectorParameterValue(TEXT("Color"), DirectionColor);
		DirectionIndicator->SetMaterial(0, ConeMat);
	}

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

	if (!bIsAlive) return;

	// Simple keyboard input for testing
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	bool bCurrentlyMoving = RobotMovement && (RobotMovement->IsMoving() || RobotMovement->IsRotating());
	if (bCurrentlyMoving) return;

	ARobotRallyGameMode* GM = Cast<ARobotRallyGameMode>(GetWorld()->GetAuthGameMode());

	// Block input while tile effects (conveyors etc.) are processing
	if (GM && GM->bProcessingTileEffects) return;

	bool bDidMove = false;

	if (PC->WasInputKeyJustPressed(EKeys::W))
	{
		ExecuteMoveCommand(1);
		bDidMove = true;
	}
	else if (PC->WasInputKeyJustPressed(EKeys::S))
	{
		ExecuteMoveCommand(-1);
		bDidMove = true;
	}
	else if (PC->WasInputKeyJustPressed(EKeys::D))
	{
		ExecuteRotateCommand(1);
		bDidMove = true;
	}
	else if (PC->WasInputKeyJustPressed(EKeys::A))
	{
		ExecuteRotateCommand(-1);
		bDidMove = true;
	}
	else if (PC->WasInputKeyJustPressed(EKeys::E))
	{
		if (GM && GM->CurrentState == EGameState::Programming)
		{
			GM->StartExecutionPhase();
		}
	}

	// After WASD input, tell GameMode to poll for movement completion -> tile effects
	if (bDidMove && GM && GM->CurrentState == EGameState::Programming)
	{
		GM->StartManualMoveTick();
	}

	// Card selection input (works even while moving)
	if (GM && GM->CurrentState == EGameState::Programming)
	{
		static const FKey NumberKeys[] = {
			EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five,
			EKeys::Six, EKeys::Seven, EKeys::Eight, EKeys::Nine
		};
		for (int32 i = 0; i < 9; ++i)
		{
			if (PC->WasInputKeyJustPressed(NumberKeys[i]))
			{
				GM->SelectCardFromHand(i);
				break;
			}
		}
		if (PC->WasInputKeyJustPressed(EKeys::BackSpace))
		{
			GM->UndoLastSelection();
		}
	}
}

void ARobotPawn::ApplyDamage(int32 Amount)
{
	if (!bIsAlive) return;

	Health = FMath::Max(0, Health - Amount);
	UE_LOG(LogTemp, Log, TEXT("Robot took %d damage! Health: %d/%d"), Amount, Health, MaxHealth);

	if (Health <= 0)
	{
		bIsAlive = false;
		UE_LOG(LogTemp, Log, TEXT("Robot destroyed!"));
		OnDeath.Broadcast();
	}
}

void ARobotPawn::ReachCheckpoint(int32 Number)
{
	if (Number == CurrentCheckpoint + 1)
	{
		CurrentCheckpoint = Number;
		UE_LOG(LogTemp, Log, TEXT("Checkpoint %d reached!"), Number);
		OnCheckpointReached.Broadcast(Number);
	}
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
