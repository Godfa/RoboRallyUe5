// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotRallyGameMode.h"
#include "RobotRallyHUD.h"
#include "RobotController.h"
#include "RobotAIController.h"
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

		// Spawn robots with controllers using the new spawn system
		SpawnRobotsWithControllers();
		StartProgrammingPhase();
	});
}

void ARobotRallyGameMode::StartProgrammingPhase()
{
	CurrentState = EGameState::Programming;
	CurrentRegister = 0;
	GetWorld()->GetTimerManager().ClearTimer(MovementCheckTimerHandle);
	DiscardHand();
	DealHandsToAllRobots();

	// Reset ready tracking
	ReadyControllers.Empty();

	// Notify all AI controllers to start thinking
	for (ARobotPawn* Robot : Robots)
	{
		if (!Robot || !Robot->bIsAlive) continue;

		AController* Controller = Robot->GetController();
		if (ARobotAIController* AIController = Cast<ARobotAIController>(Controller))
		{
			AIController->StartCardSelection();
		}
	}

	ShowEventMessage(TEXT("Programming phase. Select 5 cards (1-9), then press E."), FColor::Cyan);
}

void ARobotRallyGameMode::StartExecutionPhase()
{
	// Check that player's robot (Robot 0) has filled their registers
	// Other robots are optional (will be skipped if not programmed)
	if (Robots.IsValidIndex(0) && Robots[0]->bIsAlive)
	{
		FRobotProgram* PlayerProgram = RobotPrograms.FindByPredicate([this](const FRobotProgram& P)
		{
			return P.Robot == Robots[0];
		});

		if (PlayerProgram)
		{
			int32 FilledCount = 0;
			for (int32 Slot : PlayerProgram->RegisterSlots)
			{
				if (Slot != -1) FilledCount++;
			}

			if (FilledCount < NUM_REGISTERS)
			{
				ShowEventMessage(FString::Printf(TEXT("Robot 0 needs %d more cards!"),
					NUM_REGISTERS - FilledCount), FColor::Red);
				return;
			}
		}
	}

	CurrentState = EGameState::Executing;
	CommitAllRobotPrograms();
	DiscardHand();

	CurrentRegister = 0;
	BuildExecutionQueue(0);
	ProcessExecutionQueue();
}

void ARobotRallyGameMode::BuildExecutionQueue(int32 RegisterIndex)
{
	ExecutionQueue.Empty();

	// Collect cards from all robots for this register
	for (FRobotProgram& Program : RobotPrograms)
	{
		if (!Program.Robot || !Program.Robot->bIsAlive) continue;
		if (!Program.CommittedProgram.IsValidIndex(RegisterIndex)) continue;

		FExecutionQueueEntry Entry;
		Entry.Robot = Program.Robot;
		Entry.Card = Program.CommittedProgram[RegisterIndex];
		Entry.RegisterNumber = RegisterIndex;
		ExecutionQueue.Add(Entry);
	}

	// Sort by priority (higher first)
	ExecutionQueue.Sort();

	CurrentExecutionIndex = 0;

	UE_LOG(LogTemp, Log, TEXT("Built execution queue for register %d: %d entries"),
		RegisterIndex, ExecutionQueue.Num());
}

void ARobotRallyGameMode::ProcessExecutionQueue()
{
	if (CurrentExecutionIndex >= ExecutionQueue.Num())
	{
		// All cards in this register executed
		CurrentRegister++;

		if (CurrentRegister >= NUM_REGISTERS)
		{
			ShowEventMessage(TEXT("All registers executed!"), FColor::Green);
			StartProgrammingPhase();
			return;
		}

		// Build queue for next register
		BuildExecutionQueue(CurrentRegister);
		ProcessExecutionQueue();
		return;
	}

	// Get current priority level
	int32 CurrentPriority = ExecutionQueue[CurrentExecutionIndex].Card.Priority;

	// Execute all cards with same priority in parallel
	MovingRobots.Empty();

	while (CurrentExecutionIndex < ExecutionQueue.Num() &&
	       ExecutionQueue[CurrentExecutionIndex].Card.Priority == CurrentPriority)
	{
		FExecutionQueueEntry& Entry = ExecutionQueue[CurrentExecutionIndex];

		if (Entry.Robot && Entry.Robot->bIsAlive)
		{
			int32 RobotIndex = Robots.Find(Entry.Robot);
			ShowEventMessage(FString::Printf(TEXT("R%d: %s (P%d)"),
				RobotIndex, *GetCardActionName(Entry.Card.Action), Entry.Card.Priority),
				FColor::White);

			ExecuteCardAction(Entry.Robot, Entry.Card.Action);
			MovingRobots.Add(Entry.Robot);
		}

		CurrentExecutionIndex++;
	}

	// Start timer to wait for all parallel movements to complete
	GetWorld()->GetTimerManager().SetTimer(
		MovementCheckTimerHandle,
		this,
		&ARobotRallyGameMode::CheckParallelMovementComplete,
		0.1f,
		true);
}

void ARobotRallyGameMode::CheckParallelMovementComplete()
{
	// Remove robots that are no longer moving
	for (auto It = MovingRobots.CreateIterator(); It; ++It)
	{
		ARobotPawn* Robot = *It;
		if (!Robot || !Robot->RobotMovement) continue;

		bool bStillMoving = Robot->RobotMovement->IsMoving() ||
		                   Robot->RobotMovement->IsRotating();
		if (!bStillMoving)
		{
			It.RemoveCurrent();
		}
	}

	// All robots finished moving?
	if (MovingRobots.Num() == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(MovementCheckTimerHandle);
		ProcessAllRobotTileEffects();
	}
}

void ARobotRallyGameMode::ExecuteCardAction(ARobotPawn* Robot, ECardAction Action)
{
	if (!Robot || !Robot->bIsAlive) return;

	switch (Action)
	{
	case ECardAction::Move1:
		Robot->ExecuteMoveCommand(1);
		break;
	case ECardAction::Move2:
		Robot->ExecuteMoveCommand(2);
		break;
	case ECardAction::Move3:
		Robot->ExecuteMoveCommand(3);
		break;
	case ECardAction::MoveBack:
		Robot->ExecuteMoveCommand(-1);
		break;
	case ECardAction::RotateRight:
		Robot->ExecuteRotateCommand(1);
		break;
	case ECardAction::RotateLeft:
		Robot->ExecuteRotateCommand(-1);
		break;
	case ECardAction::UTurn:
		Robot->ExecuteRotateCommand(2);
		break;
	}
}

void ARobotRallyGameMode::ProcessTileEffects()
{
	// Legacy function for manual WASD movement - process all robots
	ProcessAllRobotTileEffects();
}

void ARobotRallyGameMode::ProcessAllRobotTileEffects()
{
	bProcessingTileEffects = true;

	// Process tile effects for all robots
	for (ARobotPawn* Robot : Robots)
	{
		if (Robot && Robot->bIsAlive)
		{
			ProcessRobotTileEffects(Robot);
		}
	}

	// Process conveyors
	ProcessAllConveyors();
}

void ARobotRallyGameMode::ProcessRobotTileEffects(ARobotPawn* Robot)
{
	if (!Robot || !Robot->bIsAlive || !GridManagerInstance) return;

	FIntVector CurrentPos(Robot->GridX, Robot->GridY, 0);
	FTileData TileData = GridManagerInstance->GetTileData(CurrentPos);

	int32 RobotIndex = Robots.Find(Robot);

	switch (TileData.TileType)
	{
	case ETileType::Pit:
		ShowEventMessage(FString::Printf(TEXT("R%d fell into pit!"), RobotIndex), FColor::Red);
		Robot->ApplyDamage(Robot->MaxHealth);
		break;

	case ETileType::Laser:
		ShowEventMessage(FString::Printf(TEXT("R%d hit by laser (-1 HP)"), RobotIndex), FColor::Orange);
		Robot->ApplyDamage(1);
		break;

	case ETileType::Checkpoint:
		Robot->ReachCheckpoint(TileData.CheckpointNumber);
		break;

	default:
		break;
	}
}

void ARobotRallyGameMode::ProcessAllConveyors()
{
	// Process conveyors for each robot separately
	for (ARobotPawn* Robot : Robots)
	{
		if (!Robot || !Robot->bIsAlive) continue;

		ProcessRobotConveyors(Robot);
	}

	OnTileEffectsComplete();
}

void ARobotRallyGameMode::ProcessRobotConveyors(ARobotPawn* Robot)
{
	if (!Robot || !Robot->bIsAlive || !GridManagerInstance) return;

	FIntVector CurrentPos(Robot->GridX, Robot->GridY, 0);
	ETileType Type = GridManagerInstance->GetTileType(CurrentPos);

	// Only move if currently on a conveyor
	if (!AGridManager::IsConveyor(Type)) return;

	// Calculate new position based on conveyor direction (move only ONE tile)
	int32 DX = 0, DY = 0;
	switch (Type)
	{
	case ETileType::ConveyorNorth: DX = 1; break;
	case ETileType::ConveyorSouth: DX = -1; break;
	case ETileType::ConveyorEast: DY = 1; break;
	case ETileType::ConveyorWest: DY = -1; break;
	}

	FIntVector NewPos(CurrentPos.X + DX, CurrentPos.Y + DY, 0);

	if (GridManagerInstance->IsValidTile(NewPos))
	{
		FVector NewWorldPos = GridManagerInstance->GridToWorld(NewPos);
		Robot->RobotMovement->MoveToWorldPosition(NewWorldPos);
		Robot->RobotMovement->SetGridPosition(NewPos.X, NewPos.Y);

		int32 RobotIndex = Robots.Find(Robot);
		ShowEventMessage(FString::Printf(TEXT("R%d moved by conveyor"), RobotIndex), FColor::Cyan);

		// Check new tile for checkpoint
		FTileData NewTile = GridManagerInstance->GetTileData(NewPos);
		if (NewTile.TileType == ETileType::Checkpoint)
		{
			Robot->ReachCheckpoint(NewTile.CheckpointNumber);
		}
	}
}

void ARobotRallyGameMode::CheckWinLoseConditions()
{
	if (!GridManagerInstance) return;

	int32 TotalCheckpoints = GridManagerInstance->GetTotalCheckpoints();
	int32 AliveRobots = 0;

	for (int32 i = 0; i < Robots.Num(); ++i)
	{
		ARobotPawn* Robot = Robots[i];
		if (!Robot) continue;

		if (Robot->bIsAlive)
		{
			AliveRobots++;

			// Win condition: first robot to collect all checkpoints wins
			if (TotalCheckpoints > 0 && Robot->CurrentCheckpoint >= TotalCheckpoints)
			{
				ShowEventMessage(FString::Printf(TEXT("VICTORY! Robot %d collected all %d checkpoints!"),
					i, TotalCheckpoints), FColor::Green);
				CurrentState = EGameState::GameOver;
				return;
			}
		}
	}

	// Lose condition: all robots destroyed
	if (AliveRobots == 0)
	{
		ShowEventMessage(TEXT("GAME OVER - All robots destroyed!"), FColor::Red);
		CurrentState = EGameState::GameOver;
	}
}

void ARobotRallyGameMode::OnTileEffectsComplete()
{
	bProcessingTileEffects = false;
	CheckWinLoseConditions();

	if (CurrentState == EGameState::GameOver) return;

	if (CurrentState == EGameState::Executing)
	{
		// Continue to next card after 0.3s delay
		FTimerHandle DelayHandle;
		GetWorld()->GetTimerManager().SetTimer(
			DelayHandle,
			this,
			&ARobotRallyGameMode::ProcessExecutionQueue,
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
	// Check if any robot is still moving (for WASD manual movement)
	bool bAnyMoving = false;
	for (ARobotPawn* Robot : Robots)
	{
		if (Robot && Robot->RobotMovement)
		{
			if (Robot->RobotMovement->IsMoving() || Robot->RobotMovement->IsRotating())
			{
				bAnyMoving = true;
				break;
			}
		}
	}

	if (!bAnyMoving)
	{
		GetWorld()->GetTimerManager().ClearTimer(ManualMoveTimerHandle);
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

void ARobotRallyGameMode::DealHandsToAllRobots()
{
	UE_LOG(LogTemp, Log, TEXT("DealHandsToAllRobots: %d programs"), RobotPrograms.Num());

	for (int32 ProgramIdx = 0; ProgramIdx < RobotPrograms.Num(); ++ProgramIdx)
	{
		FRobotProgram& Program = RobotPrograms[ProgramIdx];

		if (!Program.Robot)
		{
			UE_LOG(LogTemp, Warning, TEXT("Program %d has no robot!"), ProgramIdx);
			continue;
		}

		if (!Program.Robot->bIsAlive)
		{
			UE_LOG(LogTemp, Log, TEXT("Program %d robot is dead, skipping"), ProgramIdx);
			continue;
		}

		// Hand size depends on robot damage
		int32 Damage = Program.Robot->MaxHealth - Program.Robot->Health;
		int32 LockedCards = FMath::Max(0, Damage - 4);  // 5+ damage locks cards
		int32 HandSize = FMath::Clamp(BASE_HAND_SIZE - LockedCards, MIN_HAND_SIZE, BASE_HAND_SIZE);

		Program.HandCards.Empty();
		Program.HandCards.Reserve(HandSize);

		for (int32 i = 0; i < HandSize; ++i)
		{
			// Reshuffle discard pile if deck is empty
			if (Deck.Num() == 0)
			{
				if (DiscardPile.Num() == 0) break;
				Deck = MoveTemp(DiscardPile);
				DiscardPile.Empty();
				ShuffleDeck();
			}
			Program.HandCards.Add(Deck.Pop());
		}

		Program.RegisterSlots.Init(-1, NUM_REGISTERS);

		UE_LOG(LogTemp, Log, TEXT("Dealt %d cards to Robot %d"), Program.HandCards.Num(), ProgramIdx);
	}
}

void ARobotRallyGameMode::SelectCardFromHand(ARobotPawn* Robot, int32 HandIndex)
{
	UE_LOG(LogTemp, Log, TEXT("SelectCardFromHand called: Robot=%s, HandIndex=%d, State=%d"),
		Robot ? *Robot->GetName() : TEXT("NULL"), HandIndex, (int32)CurrentState);

	if (CurrentState != EGameState::Programming)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not in Programming state!"));
		return;
	}

	if (!Robot)
	{
		UE_LOG(LogTemp, Warning, TEXT("Robot is NULL!"));
		return;
	}

	// Find this robot's program
	FRobotProgram* Program = RobotPrograms.FindByPredicate([Robot](const FRobotProgram& P)
	{
		return P.Robot == Robot;
	});

	if (!Program)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find program for robot %s! Total programs: %d"),
			*Robot->GetName(), RobotPrograms.Num());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Found program with %d hand cards"), Program->HandCards.Num());

	if (!Program->HandCards.IsValidIndex(HandIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("HandIndex %d invalid (hand size: %d)"), HandIndex, Program->HandCards.Num());
		return;
	}

	if (IsCardInRegister(Program, HandIndex))
	{
		UE_LOG(LogTemp, Log, TEXT("Card %d already in register"), HandIndex);
		return;
	}

	// Find first empty register
	for (int32 i = 0; i < NUM_REGISTERS; ++i)
	{
		if (Program->RegisterSlots[i] == -1)
		{
			Program->RegisterSlots[i] = HandIndex;

			FRobotCard& Card = Program->HandCards[HandIndex];
			ShowEventMessage(FString::Printf(TEXT("Robot %d: R%d = %s (P%d)"),
				Robots.Find(Robot), i + 1,
				*GetCardActionName(Card.Action), Card.Priority),
				FColor::Green);
			return;
		}
	}
}

void ARobotRallyGameMode::UndoLastSelection()
{
	if (CurrentState != EGameState::Programming) return;

	// Undo for the player's robot (first robot)
	if (!Robots.IsValidIndex(0)) return;

	FRobotProgram* Program = RobotPrograms.FindByPredicate([this](const FRobotProgram& P)
	{
		return P.Robot == Robots[0];
	});

	if (!Program) return;

	// Find the last filled register and clear it
	for (int32 i = NUM_REGISTERS - 1; i >= 0; --i)
	{
		if (Program->RegisterSlots[i] != -1)
		{
			ShowEventMessage(FString::Printf(TEXT("Cleared R%d"), i + 1), FColor::Yellow);
			Program->RegisterSlots[i] = -1;
			return;
		}
	}
}

bool ARobotRallyGameMode::IsCardInRegister(const FRobotProgram* Program, int32 HandIndex) const
{
	if (!Program) return false;
	for (int32 Slot : Program->RegisterSlots)
	{
		if (Slot == HandIndex) return true;
	}
	return false;
}

void ARobotRallyGameMode::CommitAllRobotPrograms()
{
	for (FRobotProgram& Program : RobotPrograms)
	{
		if (!Program.Robot || !Program.Robot->bIsAlive) continue;

		Program.CommittedProgram.Empty();
		Program.CommittedProgram.Reserve(NUM_REGISTERS);

		for (int32 i = 0; i < NUM_REGISTERS; ++i)
		{
			int32 HandIdx = Program.RegisterSlots[i];
			if (Program.HandCards.IsValidIndex(HandIdx))
			{
				Program.CommittedProgram.Add(Program.HandCards[HandIdx]);
			}
		}
	}
}

void ARobotRallyGameMode::DiscardHand()
{
	// Discard all hands from all robot programs
	for (FRobotProgram& Program : RobotPrograms)
	{
		DiscardPile.Append(Program.HandCards);
		Program.HandCards.Empty();
		Program.RegisterSlots.Empty();
	}
}

TSubclassOf<AController> ARobotRallyGameMode::GetControllerClassForType(ERobotControllerType Type)
{
	switch (Type)
	{
	case ERobotControllerType::Player:
		return ARobotController::StaticClass();
	case ERobotControllerType::AI_Easy:
	case ERobotControllerType::AI_Medium:
	case ERobotControllerType::AI_Hard:
		return ARobotAIController::StaticClass();
	default:
		return ARobotController::StaticClass();
	}
}

void ARobotRallyGameMode::SpawnRobotsWithControllers()
{
	if (!GridManagerInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnRobotsWithControllers: GridManagerInstance is NULL!"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	Robots.Empty();
	RobotPrograms.Empty();

	// Use configured spawn configs if available, otherwise use fallback
	TArray<FRobotSpawnData> SpawnConfigs = RobotSpawnConfigs;
	if (SpawnConfigs.Num() == 0)
	{
		// Fallback default config for testing
		FRobotSpawnData PlayerConfig;
		PlayerConfig.StartPosition = FIntVector(1, 1, 0);
		PlayerConfig.StartFacing = EGridDirection::North;
		PlayerConfig.ControllerType = ERobotControllerType::Player;
		PlayerConfig.BodyColor = FLinearColor(0.2f, 0.5f, 0.9f);  // Blue

		FRobotSpawnData AIConfig;
		AIConfig.StartPosition = FIntVector(8, 8, 0);
		AIConfig.StartFacing = EGridDirection::North;
		AIConfig.ControllerType = ERobotControllerType::AI_Medium;
		AIConfig.BodyColor = FLinearColor(0.9f, 0.2f, 0.2f);  // Red

		SpawnConfigs.Add(PlayerConfig);
		SpawnConfigs.Add(AIConfig);
	}

	int32 NumRobotsToSpawn = SpawnConfigs.Num();
	Robots.Reserve(NumRobotsToSpawn);
	RobotPrograms.Reserve(NumRobotsToSpawn);

	FActorSpawnParameters RobotSpawnParams;
	RobotSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int32 i = 0; i < NumRobotsToSpawn; ++i)
	{
		const FRobotSpawnData& Config = SpawnConfigs[i];

		// Spawn robot pawn
		FIntVector SpawnGrid = Config.StartPosition;
		FVector SpawnLocation = GridManagerInstance->GridToWorld(SpawnGrid);
		SpawnLocation.Z = 30.0f;

		ARobotPawn* NewRobot = World->SpawnActor<ARobotPawn>(
			ARobotPawn::StaticClass(), SpawnLocation, FRotator::ZeroRotator, RobotSpawnParams);

		if (NewRobot)
		{
			NewRobot->GridX = SpawnGrid.X;
			NewRobot->GridY = SpawnGrid.Y;
			NewRobot->BodyColor = Config.BodyColor;

			if (NewRobot->RobotMovement)
			{
				NewRobot->RobotMovement->GridManager = GridManagerInstance;
				NewRobot->RobotMovement->InitializeGridPosition(SpawnGrid.X, SpawnGrid.Y, Config.StartFacing);
			}

			// Assign controller
			if (Config.ControllerType == ERobotControllerType::Player)
			{
				// Use the existing auto-created PlayerController instead of spawning a new one
				APlayerController* PC = World->GetFirstPlayerController();
				if (PC)
				{
					PC->Possess(NewRobot);

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
			else
			{
				// Spawn AI controller
				TSubclassOf<AController> ControllerClass = GetControllerClassForType(Config.ControllerType);
				if (ControllerClass)
				{
					AController* NewController = World->SpawnActor<AController>(
						ControllerClass, FVector::ZeroVector, FRotator::ZeroRotator, RobotSpawnParams);

					if (NewController)
					{
						if (ARobotAIController* AICtrl = Cast<ARobotAIController>(NewController))
						{
							AICtrl->DifficultyLevel = Config.ControllerType;
						}

						NewController->Possess(NewRobot);
					}
				}
			}

			Robots.Add(NewRobot);

			// Initialize program for this robot
			FRobotProgram& Program = RobotPrograms.AddDefaulted_GetRef();
			Program.Robot = NewRobot;
			Program.RegisterSlots.Init(-1, NUM_REGISTERS);

			UE_LOG(LogTemp, Log, TEXT("Spawned Robot %d at (%d,%d) with controller type %d"),
				i, SpawnGrid.X, SpawnGrid.Y, (int32)Config.ControllerType);
		}
	}

	FString Message = FString::Printf(TEXT("%d Robots ready. "), Robots.Num());
	if (Robots.Num() >= 2)
	{
		Message += FString::Printf(TEXT("Robot 0 (%s) at (%d,%d). Robot 1 (%s) at (%d,%d). WASD = move, 1-9 = select cards"),
			SpawnConfigs[0].ControllerType == ERobotControllerType::Player ? TEXT("Player") : TEXT("AI"),
			SpawnConfigs[0].StartPosition.X, SpawnConfigs[0].StartPosition.Y,
			SpawnConfigs[1].ControllerType == ERobotControllerType::Player ? TEXT("Player") : TEXT("AI"),
			SpawnConfigs[1].StartPosition.X, SpawnConfigs[1].StartPosition.Y);
	}
	ShowEventMessage(Message, FColor::Cyan);
}

bool ARobotRallyGameMode::AreAllRobotsReady() const
{
	for (ARobotPawn* Robot : Robots)
	{
		if (!Robot || !Robot->bIsAlive) continue;

		AController* Controller = Robot->GetController();
		if (!Controller) continue;

		if (!ReadyControllers.Contains(Controller))
			return false;
	}
	return true;
}

void ARobotRallyGameMode::OnControllerReady(AController* Controller)
{
	if (!Controller) return;

	ReadyControllers.Add(Controller);

	int32 RobotIndex = -1;
	ARobotPawn* ControlledRobot = Cast<ARobotPawn>(Controller->GetPawn());
	if (ControlledRobot)
	{
		RobotIndex = Robots.Find(ControlledRobot);
	}

	UE_LOG(LogTemp, Log, TEXT("Controller %d ready. Total ready: %d/%d"),
		RobotIndex, ReadyControllers.Num(), Robots.Num());

	// Check if all alive robots' controllers are ready
	if (AreAllRobotsReady())
	{
		ShowEventMessage(TEXT("All robots programmed. Starting execution..."), FColor::Green);

		// Small delay for visual feedback
		FTimerHandle DelayHandle;
		GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this]()
		{
			StartExecutionPhase();
		}, 0.5f, false);
	}
}
