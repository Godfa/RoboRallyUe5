// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotPawn.h"
#include "RobotMovementComponent.h"
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

	// Load mesh assets
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFinder(
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeFinder(
		TEXT("/Engine/BasicShapes/Cone.Cone"));

	// Body mesh - cylinder
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);
	if (CylinderFinder.Succeeded())
	{
		BodyMesh->SetStaticMesh(CylinderFinder.Object);
	}
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -15.0f));
	BodyMesh->SetRelativeScale3D(FVector(0.55f, 0.55f, 0.25f));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Direction indicator - cone pointing forward
	DirectionIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DirectionCone"));
	DirectionIndicator->SetupAttachment(RootComponent);
	if (ConeFinder.Succeeded())
	{
		DirectionIndicator->SetStaticMesh(ConeFinder.Object);
	}
	DirectionIndicator->SetRelativeLocation(FVector(25.0f, 0.0f, -10.0f));
	DirectionIndicator->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	DirectionIndicator->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.35f));
	DirectionIndicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Default starting grid position
	GridX = 0;
	GridY = 0;
}

void ARobotPawn::BeginPlay()
{
	Super::BeginPlay();

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
		// Body: blue
		UMaterialInstanceDynamic* BodyMat = UMaterialInstanceDynamic::Create(BaseMat, this);
		BodyMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.2f, 0.5f, 0.9f));
		BodyMesh->SetMaterial(0, BodyMat);

		// Direction indicator: yellow
		UMaterialInstanceDynamic* ConeMat = UMaterialInstanceDynamic::Create(BaseMat, this);
		ConeMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.9f, 0.8f, 0.1f));
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
