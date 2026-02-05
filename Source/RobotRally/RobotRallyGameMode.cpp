// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotRallyGameMode.h"
#include "GridManager.h"
#include "RobotPawn.h"
#include "RobotMovementComponent.h"
#include "Engine/World.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/SkyLight.h"
#include "Components/SkyLightComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraActor.h"

ARobotRallyGameMode::ARobotRallyGameMode()
{
	CurrentState = EGameState::Programming;
	DefaultPawnClass = nullptr;
}

void ARobotRallyGameMode::BeginPlay()
{
	Super::BeginPlay();
	SetupTestScene();
	StartProgrammingPhase();
}

void ARobotRallyGameMode::SetupTestScene()
{
	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn scene lighting
	ADirectionalLight* DirLight = World->SpawnActor<ADirectionalLight>(
		ADirectionalLight::StaticClass(),
		FVector(0.0f, 0.0f, 500.0f),
		FRotator(-50.0f, -45.0f, 0.0f),
		SpawnParams);
	if (DirLight)
	{
		DirLight->GetLightComponent()->SetIntensity(4.0f);
	}

	ASkyLight* Sky = World->SpawnActor<ASkyLight>(
		ASkyLight::StaticClass(),
		FVector(0.0f, 0.0f, 500.0f),
		FRotator::ZeroRotator,
		SpawnParams);
	if (Sky)
	{
		USkyLightComponent* SkyComp = Sky->GetLightComponent();
		SkyComp->SetIntensity(1.5f);
		SkyComp->SourceType = ESkyLightSourceType::SLS_SpecifiedCubemap;
		SkyComp->RecaptureSky();
	}

	// Spawn GridManager at origin
	GridManagerInstance = World->SpawnActor<AGridManager>(
		AGridManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (!GridManagerInstance) return;

	// Wait one frame for GridManager::BeginPlay to initialize the grid, then set up tiles
	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimerForNextTick([this, World]()
	{
		if (!GridManagerInstance) return;

		// Set up special tiles using SetTileType (updates both data and visuals)
		FTileData PitData;
		PitData.TileType = ETileType::Pit;
		GridManagerInstance->SetTileType(FIntVector(3, 3, 0), PitData);
		GridManagerInstance->SetTileType(FIntVector(3, 4, 0), PitData);
		GridManagerInstance->SetTileType(FIntVector(7, 7, 0), PitData);

		FTileData ConveyorData;
		ConveyorData.TileType = ETileType::ConveyorNorth;
		GridManagerInstance->SetTileType(FIntVector(5, 2, 0), ConveyorData);
		GridManagerInstance->SetTileType(FIntVector(5, 3, 0), ConveyorData);
		GridManagerInstance->SetTileType(FIntVector(5, 4, 0), ConveyorData);

		FTileData ConveyorEast;
		ConveyorEast.TileType = ETileType::ConveyorEast;
		GridManagerInstance->SetTileType(FIntVector(2, 7, 0), ConveyorEast);
		GridManagerInstance->SetTileType(FIntVector(3, 7, 0), ConveyorEast);

		FTileData CheckpointData;
		CheckpointData.TileType = ETileType::Checkpoint;
		CheckpointData.CheckpointNumber = 1;
		GridManagerInstance->SetTileType(FIntVector(8, 1, 0), CheckpointData);

		FTileData Checkpoint2;
		Checkpoint2.TileType = ETileType::Checkpoint;
		Checkpoint2.CheckpointNumber = 2;
		GridManagerInstance->SetTileType(FIntVector(1, 8, 0), Checkpoint2);

		FTileData LaserData;
		LaserData.TileType = ETileType::Laser;
		GridManagerInstance->SetTileType(FIntVector(6, 6, 0), LaserData);

		// Spawn robot at grid center (5, 5)
		FVector SpawnLocation = GridManagerInstance->GridToWorld(FIntVector(5, 5, 0));
		SpawnLocation.Z = 30.0f;

		FActorSpawnParameters RobotSpawnParams;
		RobotSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		TestRobot = World->SpawnActor<ARobotPawn>(
			ARobotPawn::StaticClass(), SpawnLocation, FRotator::ZeroRotator, RobotSpawnParams);

		if (TestRobot)
		{
			TestRobot->GridX = 5;
			TestRobot->GridY = 5;

			// Re-initialize movement component with correct grid position
			if (TestRobot->RobotMovement)
			{
				TestRobot->RobotMovement->InitializeGridPosition(5, 5, EGridDirection::North);
			}

			// Have Player 0 possess the robot
			APlayerController* PC = World->GetFirstPlayerController();
			if (PC)
			{
				PC->Possess(TestRobot);

				// Spawn a top-down camera above the grid center
				FVector CamGridCenter = GridManagerInstance->GridToWorld(FIntVector(
					GridManagerInstance->Width / 2, GridManagerInstance->Height / 2, 0));
				float CameraHeight = GridManagerInstance->Width * GridManagerInstance->TileSize * 1.2f;
				FVector CamLocation(CamGridCenter.X, CamGridCenter.Y, CameraHeight);
				FRotator CamRotation(-90.0f, 0.0f, 0.0f);

				FActorSpawnParameters CamSpawnParams;
				CamSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				ACameraActor* TopDownCamera = World->SpawnActor<ACameraActor>(
					ACameraActor::StaticClass(), CamLocation, CamRotation, CamSpawnParams);

				if (TopDownCamera)
				{
					PC->SetViewTargetWithBlend(TopDownCamera, 0.0f);
				}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Test scene ready: Grid + Robot at (5,5). Use W/S to move, A/D to rotate."));
	});
}

void ARobotRallyGameMode::StartProgrammingPhase()
{
	CurrentState = EGameState::Programming;
	CurrentRegister = 0;
	GetWorld()->GetTimerManager().ClearTimer(MovementCheckTimerHandle);
	UE_LOG(LogTemp, Log, TEXT("Returned to Programming phase. Press E to execute cards."));
}

void ARobotRallyGameMode::StartExecutionPhase()
{
	CurrentState = EGameState::Executing;
	CurrentRegister = 0;
	SetupTestCards();
	UE_LOG(LogTemp, Log, TEXT("Starting Execution phase with %d cards."), ProgrammedCards.Num());
	ProcessNextRegister();
}

void ARobotRallyGameMode::ProcessNextRegister()
{
	if (CurrentRegister >= NUM_REGISTERS)
	{
		UE_LOG(LogTemp, Log, TEXT("All registers executed. Returning to Programming phase."));
		StartProgrammingPhase();
		return;
	}

	if (!ProgrammedCards.IsValidIndex(CurrentRegister))
	{
		UE_LOG(LogTemp, Warning, TEXT("No card at register %d"), CurrentRegister);
		StartProgrammingPhase();
		return;
	}

	FRobotCard Card = ProgrammedCards[CurrentRegister];
	UE_LOG(LogTemp, Log, TEXT("Executing Register %d: Action %d"), CurrentRegister, static_cast<int32>(Card.Action));

	ExecuteCardAction(Card.Action);
	CurrentRegister++;

	// Start timer to check when movement completes
	GetWorld()->GetTimerManager().SetTimer(
		MovementCheckTimerHandle,
		this,
		&ARobotRallyGameMode::CheckMovementComplete,
		0.1f,
		true);
}

void ARobotRallyGameMode::ExecuteCardAction(ECardAction Action)
{
	if (!TestRobot)
	{
		UE_LOG(LogTemp, Warning, TEXT("No TestRobot to execute card action"));
		return;
	}

	switch (Action)
	{
	case ECardAction::Move1:
		TestRobot->ExecuteMoveCommand(1);
		break;
	case ECardAction::Move2:
		TestRobot->ExecuteMoveCommand(2);
		break;
	case ECardAction::Move3:
		TestRobot->ExecuteMoveCommand(3);
		break;
	case ECardAction::MoveBack:
		TestRobot->ExecuteMoveCommand(-1);
		break;
	case ECardAction::RotateRight:
		TestRobot->ExecuteRotateCommand(1);
		break;
	case ECardAction::RotateLeft:
		TestRobot->ExecuteRotateCommand(-1);
		break;
	case ECardAction::UTurn:
		TestRobot->ExecuteRotateCommand(2);
		break;
	}
}

void ARobotRallyGameMode::CheckMovementComplete()
{
	if (!TestRobot || !TestRobot->RobotMovement)
	{
		GetWorld()->GetTimerManager().ClearTimer(MovementCheckTimerHandle);
		ProcessNextRegister();
		return;
	}

	bool bStillMoving = TestRobot->RobotMovement->IsMoving() || TestRobot->RobotMovement->IsRotating();
	if (!bStillMoving)
	{
		GetWorld()->GetTimerManager().ClearTimer(MovementCheckTimerHandle);
		ProcessNextRegister();
	}
}

void ARobotRallyGameMode::SetupTestCards()
{
	ProgrammedCards.Empty();

	// Test sequence: Move1, RotateRight, Move2, RotateLeft, MoveBack
	FRobotCard Card1;
	Card1.Action = ECardAction::Move1;
	Card1.Priority = 100;
	ProgrammedCards.Add(Card1);

	FRobotCard Card2;
	Card2.Action = ECardAction::RotateRight;
	Card2.Priority = 200;
	ProgrammedCards.Add(Card2);

	FRobotCard Card3;
	Card3.Action = ECardAction::Move2;
	Card3.Priority = 300;
	ProgrammedCards.Add(Card3);

	FRobotCard Card4;
	Card4.Action = ECardAction::RotateLeft;
	Card4.Priority = 400;
	ProgrammedCards.Add(Card4);

	FRobotCard Card5;
	Card5.Action = ECardAction::MoveBack;
	Card5.Priority = 500;
	ProgrammedCards.Add(Card5);

	UE_LOG(LogTemp, Log, TEXT("Test cards loaded: Move1, RotateRight, Move2, RotateLeft, MoveBack"));
}
