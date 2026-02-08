// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotRallyGameMode.h"
#include "RobotRallyHUD.h"
#include "RobotController.h"
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
	PlayerControllerClass = ARobotController::StaticClass();
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
	BuildDeck();
	ShuffleDeck();
	SetupTestScene();
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

		// Conveyor belt 1: L-shaped, goes up (+X=North) then right (+Y=East)
		// (1,7)->(2,7)->(3,7) North, turn at (3,7)->East, (3,8)->(3,9) East
		FTileData ConvNorth;
		ConvNorth.TileType = ETileType::ConveyorNorth;
		GridManagerInstance->SetTileType(FIntVector(1, 7, 0), ConvNorth);
		GridManagerInstance->SetTileType(FIntVector(2, 7, 0), ConvNorth);

		FTileData ConvEastTurn1;
		ConvEastTurn1.TileType = ETileType::ConveyorEast;
		GridManagerInstance->SetTileType(FIntVector(3, 7, 0), ConvEastTurn1);
		GridManagerInstance->SetTileType(FIntVector(3, 8, 0), ConvEastTurn1);
		GridManagerInstance->SetTileType(FIntVector(3, 9, 0), ConvEastTurn1);

		// Conveyor belt 2: Straight West path (-Y direction on screen = left)
		// (8,6) -> (8,5) -> (8,4) -> (8,3)
		FTileData ConvWest;
		ConvWest.TileType = ETileType::ConveyorWest;
		GridManagerInstance->SetTileType(FIntVector(8, 6, 0), ConvWest);
		GridManagerInstance->SetTileType(FIntVector(8, 5, 0), ConvWest);
		GridManagerInstance->SetTileType(FIntVector(8, 4, 0), ConvWest);
		GridManagerInstance->SetTileType(FIntVector(8, 3, 0), ConvWest);

		// Conveyor belt 3: Goes right (+Y=East), turns up (+X=North), then right again
		// (5,1)->(5,2)->(5,3) East, turn at (5,3)->North, (6,3)->(6,4) East
		FTileData ConvEast;
		ConvEast.TileType = ETileType::ConveyorEast;
		GridManagerInstance->SetTileType(FIntVector(5, 1, 0), ConvEast);
		GridManagerInstance->SetTileType(FIntVector(5, 2, 0), ConvEast);

		FTileData ConvNorthTurn;
		ConvNorthTurn.TileType = ETileType::ConveyorNorth;
		GridManagerInstance->SetTileType(FIntVector(5, 3, 0), ConvNorthTurn);

		FTileData ConvEast2;
		ConvEast2.TileType = ETileType::ConveyorEast;
		GridManagerInstance->SetTileType(FIntVector(6, 3, 0), ConvEast2);
		GridManagerInstance->SetTileType(FIntVector(6, 4, 0), ConvEast2);

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

		ShowEventMessage(TEXT("Robot ready at (5,5). WASD = move, 1-9 = select cards, E = execute"), FColor::Cyan);
		StartProgrammingPhase();
	});
}

void ARobotRallyGameMode::StartProgrammingPhase()
{
	CurrentState = EGameState::Programming;
	CurrentRegister = 0;
	GetWorld()->GetTimerManager().ClearTimer(MovementCheckTimerHandle);
	DiscardHand();
	DealHand();
	ShowEventMessage(TEXT("Programming phase. Select 5 cards (1-9), then press E."), FColor::Cyan);
}

void ARobotRallyGameMode::StartExecutionPhase()
{
	if (!AreAllRegistersFilled())
	{
		ShowEventMessage(TEXT("Fill all 5 registers before executing!"), FColor::Red);
		return;
	}
	CurrentState = EGameState::Executing;
	CurrentRegister = 0;
	CommitRegistersToProgram();
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

	FString CardName = GetCardActionName(Card.Action);
	ShowEventMessage(FString::Printf(TEXT("Register %d: %s (P%d)"), CurrentRegister + 1, *CardName, Card.Priority), FColor::White);

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
	bProcessingTileEffects = true;

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
		// ReachCheckpoint handles all messaging internally
		TestRobot->ReachCheckpoint(TileData.CheckpointNumber);
		break;

	default:
		break;
	}

	// Process conveyors after other effects
	ProcessConveyors();
}

void ARobotRallyGameMode::ProcessConveyors()
{
	if (!TestRobot || !TestRobot->bIsAlive || !GridManagerInstance)
	{
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
		OnTileEffectsComplete();
		return;
	}

	// Move robot one tile in conveyor direction (once per register)
	int32 NewX = TestRobot->GridX + DX;
	int32 NewY = TestRobot->GridY + DY;

	ShowEventMessage(FString::Printf(TEXT("Conveyor: (%d,%d) -> (%d,%d)"),
		TestRobot->GridX, TestRobot->GridY, NewX, NewY), FColor::Cyan);

	TestRobot->GridX = NewX;
	TestRobot->GridY = NewY;

	if (TestRobot->RobotMovement)
	{
		TestRobot->RobotMovement->SetGridPosition(NewX, NewY);
		FVector NewWorldPos = GridManagerInstance->GridToWorld(FIntVector(NewX, NewY, 0));
		TestRobot->RobotMovement->MoveToWorldPosition(NewWorldPos);
	}

	// Wait for movement to complete, then check destination tile
	GetWorld()->GetTimerManager().SetTimer(
		TileEffectTimerHandle,
		[this]()
		{
			if (!TestRobot || !TestRobot->RobotMovement)
			{
				OnTileEffectsComplete();
				return;
			}
			if (!TestRobot->RobotMovement->IsMoving())
			{
				GetWorld()->GetTimerManager().ClearTimer(TileEffectTimerHandle);

				// Check for hazards on the destination tile
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
						// ReachCheckpoint handles all messaging internally
						TestRobot->ReachCheckpoint(NewTile.CheckpointNumber);
					}
				}

				OnTileEffectsComplete();
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
	bProcessingTileEffects = false;

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

void ARobotRallyGameMode::StartManualMoveTick()
{
	bProcessingTileEffects = true;

	// Poll for movement completion, then trigger tile effects
	GetWorld()->GetTimerManager().SetTimer(
		ManualMoveTimerHandle,
		this,
		&ARobotRallyGameMode::CheckManualMoveComplete,
		0.05f,
		true);
}

void ARobotRallyGameMode::CheckManualMoveComplete()
{
	if (!TestRobot || !TestRobot->RobotMovement)
	{
		GetWorld()->GetTimerManager().ClearTimer(ManualMoveTimerHandle);
		bProcessingTileEffects = false;
		return;
	}

	bool bStillMoving = TestRobot->RobotMovement->IsMoving() || TestRobot->RobotMovement->IsRotating();
	if (!bStillMoving)
	{
		GetWorld()->GetTimerManager().ClearTimer(ManualMoveTimerHandle);
		// Movement done — now process tile effects (which may start conveyor movement)
		ProcessTileEffects();
	}
}

FString ARobotRallyGameMode::GetCardActionName(ECardAction Action)
{
	switch (Action)
	{
	case ECardAction::Move1:        return TEXT("Move 1");
	case ECardAction::Move2:        return TEXT("Move 2");
	case ECardAction::Move3:        return TEXT("Move 3");
	case ECardAction::MoveBack:     return TEXT("Back Up");
	case ECardAction::RotateRight:  return TEXT("Turn Right");
	case ECardAction::RotateLeft:   return TEXT("Turn Left");
	case ECardAction::UTurn:        return TEXT("U-Turn");
	default:                        return TEXT("???");
	}
}

void ARobotRallyGameMode::BuildDeck()
{
	Deck.Empty();
	Deck.Reserve(DECK_SIZE);

	auto AddCards = [this](ECardAction Action, int32 Count, int32 StartPriority, int32 Step)
	{
		for (int32 i = 0; i < Count; ++i)
		{
			FRobotCard Card;
			Card.Action = Action;
			Card.Priority = StartPriority + i * Step;
			Deck.Add(Card);
		}
	};

	// Robot Rally classic deck distribution (84 cards)
	AddCards(ECardAction::UTurn,       6,  10,  10);  // 10-60
	AddCards(ECardAction::RotateLeft, 18,  70,  20);  // 70, 90, 110, ... 410
	AddCards(ECardAction::RotateRight,18,  80,  20);  // 80, 100, 120, ... 420
	AddCards(ECardAction::MoveBack,    6, 430,  10);  // 430-480
	AddCards(ECardAction::Move1,      18, 490,  10);  // 490-660
	AddCards(ECardAction::Move2,      12, 670,  10);  // 670-780
	AddCards(ECardAction::Move3,       6, 790,  10);  // 790-840

	UE_LOG(LogTemp, Log, TEXT("Deck built: %d cards"), Deck.Num());
}

void ARobotRallyGameMode::ShuffleDeck()
{
	// Fisher-Yates shuffle
	for (int32 i = Deck.Num() - 1; i > 0; --i)
	{
		int32 j = FMath::RandRange(0, i);
		Deck.Swap(i, j);
	}
}

int32 ARobotRallyGameMode::CalculateHandSize() const
{
	if (!TestRobot) return BASE_HAND_SIZE;
	int32 DamageTaken = TestRobot->MaxHealth - TestRobot->Health;
	return FMath::Clamp(BASE_HAND_SIZE - DamageTaken, MIN_HAND_SIZE, BASE_HAND_SIZE);
}

void ARobotRallyGameMode::DealHand()
{
	int32 HandSize = CalculateHandSize();
	HandCards.Empty();
	HandCards.Reserve(HandSize);

	for (int32 i = 0; i < HandSize; ++i)
	{
		// Reshuffle discard pile if deck is empty
		if (Deck.Num() == 0)
		{
			if (DiscardPile.Num() == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("No cards left in deck or discard pile!"));
				break;
			}
			Deck = MoveTemp(DiscardPile);
			DiscardPile.Empty();
			ShuffleDeck();
			UE_LOG(LogTemp, Log, TEXT("Reshuffled discard pile into deck (%d cards)"), Deck.Num());
		}

		HandCards.Add(Deck.Pop());
	}

	// Initialize register slots to empty
	RegisterSlots.Init(-1, NUM_REGISTERS);

	UE_LOG(LogTemp, Log, TEXT("Dealt %d cards (hand size: %d)"), HandCards.Num(), HandSize);
}

void ARobotRallyGameMode::SelectCardFromHand(int32 HandIndex)
{
	if (CurrentState != EGameState::Programming) return;
	if (!HandCards.IsValidIndex(HandIndex)) return;
	if (IsCardInRegister(HandIndex)) return;

	// Find first empty register slot
	for (int32 i = 0; i < NUM_REGISTERS; ++i)
	{
		if (RegisterSlots[i] == -1)
		{
			RegisterSlots[i] = HandIndex;
			FString CardName = GetCardActionName(HandCards[HandIndex].Action);
			ShowEventMessage(FString::Printf(TEXT("R%d: %s (P%d)"),
				i + 1, *CardName, HandCards[HandIndex].Priority), FColor::Green);
			return;
		}
	}

	ShowEventMessage(TEXT("All registers full! Press E to execute."), FColor::Yellow);
}

void ARobotRallyGameMode::UndoLastSelection()
{
	if (CurrentState != EGameState::Programming) return;

	// Find the last filled register and clear it
	for (int32 i = NUM_REGISTERS - 1; i >= 0; --i)
	{
		if (RegisterSlots[i] != -1)
		{
			ShowEventMessage(FString::Printf(TEXT("Cleared R%d"), i + 1), FColor::Yellow);
			RegisterSlots[i] = -1;
			return;
		}
	}
}

bool ARobotRallyGameMode::AreAllRegistersFilled() const
{
	if (RegisterSlots.Num() != NUM_REGISTERS) return false;
	for (int32 Slot : RegisterSlots)
	{
		if (Slot == -1) return false;
	}
	return true;
}

bool ARobotRallyGameMode::IsCardInRegister(int32 HandIndex) const
{
	return RegisterSlots.Contains(HandIndex);
}

void ARobotRallyGameMode::CommitRegistersToProgram()
{
	ProgrammedCards.Empty();
	ProgrammedCards.Reserve(NUM_REGISTERS);

	for (int32 i = 0; i < NUM_REGISTERS; ++i)
	{
		int32 HandIdx = RegisterSlots[i];
		if (HandCards.IsValidIndex(HandIdx))
		{
			ProgrammedCards.Add(HandCards[HandIdx]);
		}
	}
}

void ARobotRallyGameMode::DiscardHand()
{
	DiscardPile.Append(HandCards);
	HandCards.Empty();
	RegisterSlots.Empty();
}
