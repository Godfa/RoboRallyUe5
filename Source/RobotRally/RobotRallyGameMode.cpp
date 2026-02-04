// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotRallyGameMode.h"
#include "GridManager.h"
#include "RobotPawn.h"
#include "RobotMovementComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
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

	// Spawn GridManager at origin
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GridManagerInstance = World->SpawnActor<AGridManager>(
		AGridManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (!GridManagerInstance) return;

	// Wait one frame for GridManager::BeginPlay to initialize the grid, then set up tiles
	// For immediate setup, call InitializeGrid manually isn't possible (private)
	// Instead, modify tiles after BeginPlay has run - use a timer
	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimerForNextTick([this, World, SpawnParams]()
	{
		if (!GridManagerInstance) return;

		// Add some interesting tiles
		FTileData PitData;
		PitData.TileType = ETileType::Pit;
		GridManagerInstance->GridMap.Add(FIntVector(3, 3, 0), PitData);
		GridManagerInstance->GridMap.Add(FIntVector(3, 4, 0), PitData);
		GridManagerInstance->GridMap.Add(FIntVector(7, 7, 0), PitData);

		FTileData ConveyorData;
		ConveyorData.TileType = ETileType::ConveyorNorth;
		GridManagerInstance->GridMap.Add(FIntVector(5, 2, 0), ConveyorData);
		GridManagerInstance->GridMap.Add(FIntVector(5, 3, 0), ConveyorData);
		GridManagerInstance->GridMap.Add(FIntVector(5, 4, 0), ConveyorData);

		FTileData ConveyorEast;
		ConveyorEast.TileType = ETileType::ConveyorEast;
		GridManagerInstance->GridMap.Add(FIntVector(2, 7, 0), ConveyorEast);
		GridManagerInstance->GridMap.Add(FIntVector(3, 7, 0), ConveyorEast);

		FTileData CheckpointData;
		CheckpointData.TileType = ETileType::Checkpoint;
		CheckpointData.CheckpointNumber = 1;
		GridManagerInstance->GridMap.Add(FIntVector(8, 1, 0), CheckpointData);

		FTileData Checkpoint2;
		Checkpoint2.TileType = ETileType::Checkpoint;
		Checkpoint2.CheckpointNumber = 2;
		GridManagerInstance->GridMap.Add(FIntVector(1, 8, 0), Checkpoint2);

		FTileData LaserData;
		LaserData.TileType = ETileType::Laser;
		GridManagerInstance->GridMap.Add(FIntVector(6, 6, 0), LaserData);

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
			// (BeginPlay ran during SpawnActor with default GridX=0, GridY=0)
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
				FRotator CamRotation(-90.0f, 0.0f, 0.0f); // Look straight down

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

		// Spawn a floor plane under the grid
		FActorSpawnParameters FloorSpawnParams;
		FloorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector GridCenter = GridManagerInstance->GridToWorld(FIntVector(
			GridManagerInstance->Width / 2, GridManagerInstance->Height / 2, 0));

		AStaticMeshActor* Floor = World->SpawnActor<AStaticMeshActor>(
			AStaticMeshActor::StaticClass(), FVector(GridCenter.X, GridCenter.Y, -1.0f),
			FRotator::ZeroRotator, FloorSpawnParams);

		if (Floor)
		{
			Floor->SetMobility(EComponentMobility::Movable);
			UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr,
				TEXT("/Engine/BasicShapes/Plane.Plane"));
			if (PlaneMesh)
			{
				Floor->GetStaticMeshComponent()->SetStaticMesh(PlaneMesh);
				float ScaleX = GridManagerInstance->Width * GridManagerInstance->TileSize / 100.0f;
				float ScaleY = GridManagerInstance->Height * GridManagerInstance->TileSize / 100.0f;
				Floor->SetActorScale3D(FVector(ScaleX, ScaleY, 1.0f));
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Test scene ready: Grid + Robot at (5,5). Use W/S to move, A/D to rotate."));
	});
}

void ARobotRallyGameMode::StartProgrammingPhase()
{
	CurrentState = EGameState::Programming;
	CurrentRegister = 0;
}

void ARobotRallyGameMode::StartExecutionPhase()
{
	CurrentState = EGameState::Executing;
	ProcessNextRegister();
}

void ARobotRallyGameMode::ProcessNextRegister()
{
	if (CurrentRegister >= 5)
	{
		StartProgrammingPhase();
		return;
	}

	CurrentRegister++;
}
