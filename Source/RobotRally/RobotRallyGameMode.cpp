// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotRallyGameMode.h"
#include "RobotRallyHUD.h"
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
	HUDClass = ARobotRallyHUD::StaticClass();
}

void ARobotRallyGameMode::ShowEventMessage(const FString& Text, FColor Color)
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *Text);

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		ARobotRallyHUD* HUD = Cast<ARobotRallyHUD>(PC->GetHUD());
		if (HUD)
		{
			HUD->AddEventMessage(Text, Color);
		}
	}
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
		GridManagerInstance->SetTileType(FIntVector(6, 5, 0), LaserData);
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

		ShowEventMessage(TEXT("Robot ready at (5,5). WASD = move, E = execute cards"), FColor::Cyan);
	});
}

void ARobotRallyGameMode::StartProgrammingPhase()
{
	CurrentState = EGameState::Programming;
	CurrentRegister = 0;
	GetWorld()->GetTimerManager().ClearTimer(MovementCheckTimerHandle);
	ShowEventMessage(TEXT("Programming phase. Press E to execute cards."), FColor::Cyan);
}

void ARobotRallyGameMode::StartExecutionPhase()
{
	CurrentState = EGameState::Executing;
	CurrentRegister = 0;
	SetupTestCards();
	ShowEventMessage(TEXT("Executing cards..."), FColor::Cyan);
	ProcessNextRegister();
}

void ARobotRallyGameMode::ProcessNextRegister()
{
	if (CurrentState == EGameState::GameOver)
	{
		return;
	}

	if (!TestRobot || !TestRobot->bIsAlive)
	{
		ShowEventMessage(TEXT("Robot is dead, stopping execution."), FColor::Red);
		CurrentState = EGameState::GameOver;
		return;
	}

	if (CurrentRegister >= NUM_REGISTERS)
	{
		ShowEventMessage(TEXT("All registers executed."), FColor::Green);
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

	// Show card name on HUD
	static const TCHAR* CardNames[] = {
		TEXT("Move 1"), TEXT("Move 2"), TEXT("Move 3"), TEXT("Move Back"),
		TEXT("Rotate Right"), TEXT("Rotate Left"), TEXT("U-Turn")
	};
	int32 ActionIdx = static_cast<int32>(Card.Action);
	FString CardName = (ActionIdx >= 0 && ActionIdx < 7) ? CardNames[ActionIdx] : TEXT("???");
	ShowEventMessage(FString::Printf(TEXT("Register %d: %s"), CurrentRegister + 1, *CardName), FColor::White);

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
		// After card movement completes, process tile effects
		ProcessTileEffects();
	}
}

void ARobotRallyGameMode::ProcessTileEffects()
{
	if (!TestRobot || !TestRobot->bIsAlive || !GridManagerInstance)
	{
		OnTileEffectsComplete();
		return;
	}

	FIntVector RobotCoords(TestRobot->GridX, TestRobot->GridY, 0);
	FTileData TileData = GridManagerInstance->GetTileData(RobotCoords);

	switch (TileData.TileType)
	{
	case ETileType::Pit:
		ShowEventMessage(FString::Printf(TEXT("Robot fell into a pit at (%d, %d)!"), RobotCoords.X, RobotCoords.Y), FColor::Red);
		TestRobot->ApplyDamage(TestRobot->MaxHealth);
		break;

	case ETileType::Laser:
		ShowEventMessage(FString::Printf(TEXT("Laser hit! -1 HP at (%d, %d)"), RobotCoords.X, RobotCoords.Y), FColor::Orange);
		TestRobot->ApplyDamage(1);
		break;

	case ETileType::Checkpoint:
		if (TileData.CheckpointNumber == TestRobot->CurrentCheckpoint + 1)
		{
			ShowEventMessage(FString::Printf(TEXT("Checkpoint %d reached!"), TileData.CheckpointNumber), FColor::Yellow);
		}
		TestRobot->ReachCheckpoint(TileData.CheckpointNumber);
		break;

	default:
		break;
	}

	// Process conveyors after other effects
	ProcessConveyors();
}

void ARobotRallyGameMode::ProcessConveyors(int32 ChainDepth)
{
	if (!TestRobot || !TestRobot->bIsAlive || !GridManagerInstance)
	{
		OnTileEffectsComplete();
		return;
	}

	if (ChainDepth >= MAX_CONVEYOR_CHAIN)
	{
		UE_LOG(LogTemp, Warning, TEXT("Conveyor chain depth limit reached (%d)!"), MAX_CONVEYOR_CHAIN);
		OnTileEffectsComplete();
		return;
	}

	FIntVector RobotCoords(TestRobot->GridX, TestRobot->GridY, 0);
	ETileType TileType = GridManagerInstance->GetTileType(RobotCoords);

	int32 DX = 0, DY = 0;
	switch (TileType)
	{
	case ETileType::ConveyorNorth: DX = 1;  DY = 0;  break;
	case ETileType::ConveyorSouth: DX = -1; DY = 0;  break;
	case ETileType::ConveyorEast:  DX = 0;  DY = 1;  break;
	case ETileType::ConveyorWest:  DX = 0;  DY = -1; break;
	default:
		// Not on a conveyor, done
		OnTileEffectsComplete();
		return;
	}

	// Move robot to conveyor destination
	int32 NewX = TestRobot->GridX + DX;
	int32 NewY = TestRobot->GridY + DY;

	ShowEventMessage(FString::Printf(TEXT("Conveyor: (%d,%d) -> (%d,%d)"),
		TestRobot->GridX, TestRobot->GridY, NewX, NewY), FColor::Cyan);

	// Update grid position and start smooth movement
	TestRobot->GridX = NewX;
	TestRobot->GridY = NewY;

	if (TestRobot->RobotMovement)
	{
		TestRobot->RobotMovement->SetGridPosition(NewX, NewY);
		FVector NewWorldPos = GridManagerInstance->GridToWorld(FIntVector(NewX, NewY, 0));
		TestRobot->RobotMovement->MoveToWorldPosition(NewWorldPos);
	}

	// Wait for conveyor movement to complete, then check for chains
	int32 NextChainDepth = ChainDepth + 1;
	GetWorld()->GetTimerManager().SetTimer(
		TileEffectTimerHandle,
		[this, NextChainDepth]()
		{
			if (!TestRobot || !TestRobot->RobotMovement)
			{
				OnTileEffectsComplete();
				return;
			}
			if (!TestRobot->RobotMovement->IsMoving())
			{
				GetWorld()->GetTimerManager().ClearTimer(TileEffectTimerHandle);
				// Check for pit/laser on new tile, then chain conveyors
				if (GridManagerInstance && TestRobot->bIsAlive)
				{
					FIntVector NewCoords(TestRobot->GridX, TestRobot->GridY, 0);
					FTileData NewTile = GridManagerInstance->GetTileData(NewCoords);
					if (NewTile.TileType == ETileType::Pit)
					{
						ShowEventMessage(TEXT("Conveyor pushed robot into pit!"), FColor::Red);
						TestRobot->ApplyDamage(TestRobot->MaxHealth);
					}
					else if (NewTile.TileType == ETileType::Laser)
					{
						ShowEventMessage(TEXT("Conveyor pushed robot into laser! -1 HP"), FColor::Orange);
						TestRobot->ApplyDamage(1);
					}
					else if (NewTile.TileType == ETileType::Checkpoint)
					{
						if (NewTile.CheckpointNumber == TestRobot->CurrentCheckpoint + 1)
						{
							ShowEventMessage(FString::Printf(TEXT("Checkpoint %d reached!"), NewTile.CheckpointNumber), FColor::Yellow);
						}
						TestRobot->ReachCheckpoint(NewTile.CheckpointNumber);
					}
				}
				ProcessConveyors(NextChainDepth);
			}
		},
		0.1f,
		true);
}

void ARobotRallyGameMode::CheckWinLoseConditions()
{
	if (!TestRobot) return;

	if (!TestRobot->bIsAlive)
	{
		ShowEventMessage(TEXT("GAME OVER - Robot destroyed!"), FColor::Red);
		CurrentState = EGameState::GameOver;
		GetWorld()->GetTimerManager().ClearTimer(MovementCheckTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(TileEffectTimerHandle);
		return;
	}

	if (GridManagerInstance)
	{
		int32 TotalCheckpoints = GridManagerInstance->GetTotalCheckpoints();
		if (TotalCheckpoints > 0 && TestRobot->CurrentCheckpoint >= TotalCheckpoints)
		{
			ShowEventMessage(FString::Printf(TEXT("VICTORY! All %d checkpoints reached!"), TotalCheckpoints), FColor::Green);
			CurrentState = EGameState::GameOver;
			GetWorld()->GetTimerManager().ClearTimer(MovementCheckTimerHandle);
			GetWorld()->GetTimerManager().ClearTimer(TileEffectTimerHandle);
			return;
		}
	}
}

void ARobotRallyGameMode::OnTileEffectsComplete()
{
	CheckWinLoseConditions();

	if (CurrentState == EGameState::GameOver) return;

	// Only continue register execution if in Executing phase
	// (manual WASD moves also call ProcessTileEffects but shouldn't advance registers)
	if (CurrentState == EGameState::Executing)
	{
		FTimerHandle DelayHandle;
		GetWorld()->GetTimerManager().SetTimer(
			DelayHandle,
			this,
			&ARobotRallyGameMode::ProcessNextRegister,
			0.3f,
			false);
	}
}

void ARobotRallyGameMode::SetupTestCards()
{
	ProgrammedCards.Empty();

	// Demo route from (5,5) facing North:
	// 1. Move1 -> (6,5) LASER! takes 1 damage
	// 2. Move1 -> (7,5) safe
	// 3. RotateRight -> face East
	// 4. Move2 -> (7,7) PIT! robot dies
	// 5. (won't execute - robot is dead)
	FRobotCard Card1;
	Card1.Action = ECardAction::Move1;
	Card1.Priority = 100;
	ProgrammedCards.Add(Card1);

	FRobotCard Card2;
	Card2.Action = ECardAction::Move1;
	Card2.Priority = 200;
	ProgrammedCards.Add(Card2);

	FRobotCard Card3;
	Card3.Action = ECardAction::RotateRight;
	Card3.Priority = 300;
	ProgrammedCards.Add(Card3);

	FRobotCard Card4;
	Card4.Action = ECardAction::Move2;
	Card4.Priority = 400;
	ProgrammedCards.Add(Card4);

	FRobotCard Card5;
	Card5.Action = ECardAction::Move1;
	Card5.Priority = 500;
	ProgrammedCards.Add(Card5);

	UE_LOG(LogTemp, Log, TEXT("Test cards loaded: Move1, Move1, RotateRight, Move2, Move1 (hits laser + pit)"));
}
