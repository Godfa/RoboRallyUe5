// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotRallyHUD.h"
#include "RobotPawn.h"
#include "RobotRallyGameMode.h"
#include "RobotRallyGameState.h"
#include "RobotRallyPlayerState.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "GameFramework/PlayerController.h"

ARobotRallyPlayerState* ARobotRallyHUD::GetLocalPlayerState() const
{
	APlayerController* PC = GetOwningPlayerController();
	if (PC)
	{
		return Cast<ARobotRallyPlayerState>(PC->PlayerState);
	}
	return nullptr;
}

ARobotRallyGameState* ARobotRallyHUD::GetRobotRallyGameState() const
{
	UWorld* World = GetWorld();
	if (World)
	{
		return Cast<ARobotRallyGameState>(World->GetGameState());
	}
	return nullptr;
}

void ARobotRallyHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas) return;

	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	UFont* Font = GEngine->GetLargeFont();
	const float LineHeight = 22.0f;
	const float Padding = 10.0f;

	// --- Event log (bottom-left) ---
	// Tick message timers and remove expired
	for (int32 i = Messages.Num() - 1; i >= 0; --i)
	{
		Messages[i].TimeRemaining -= DeltaTime;
		if (Messages[i].TimeRemaining <= 0.0f)
		{
			Messages.RemoveAt(i);
		}
	}

	if (Messages.Num() > 0)
	{
		float LogHeight = Messages.Num() * LineHeight + Padding * 2.0f;
		float LogY = Canvas->SizeY - LogHeight - 40.0f;
		float LogX = Padding;
		float LogWidth = 450.0f;

		// Semi-transparent background
		FCanvasTileItem BG(
			FVector2D(LogX, LogY),
			FVector2D(LogWidth, LogHeight),
			FLinearColor(0.0f, 0.0f, 0.0f, 0.6f));
		BG.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(BG);

		// Draw messages (oldest at top, newest at bottom)
		for (int32 i = 0; i < Messages.Num(); ++i)
		{
			float Alpha = FMath::Clamp(Messages[i].TimeRemaining / 1.0f, 0.3f, 1.0f);
			FLinearColor DrawColor(Messages[i].Color);
			DrawColor.A = Alpha;

			float TextY = LogY + Padding + i * LineHeight;
			FCanvasTextItem TextItem(
				FVector2D(LogX + Padding, TextY),
				FText::FromString(Messages[i].Text),
				Font, DrawColor);
			TextItem.EnableShadow(FLinearColor::Black);
			Canvas->DrawItem(TextItem);
		}
	}

	// --- Health bar + status (top-left) ---
	APlayerController* PC = GetOwningPlayerController();
	ARobotPawn* Robot = PC ? Cast<ARobotPawn>(PC->GetPawn()) : nullptr;
	if (Robot)
	{
		float BarX = Padding;
		float BarY = Padding;
		float BarWidth = 200.0f;
		float BarHeight = 24.0f;
		float HealthPct = (Robot->MaxHealth > 0)
			? static_cast<float>(Robot->Health) / static_cast<float>(Robot->MaxHealth)
			: 0.0f;

		// Background
		FCanvasTileItem BarBG(
			FVector2D(BarX, BarY),
			FVector2D(BarWidth, BarHeight),
			FLinearColor(0.0f, 0.0f, 0.0f, 0.7f));
		BarBG.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(BarBG);

		// Health fill
		FLinearColor HealthColor = FMath::Lerp(
			FLinearColor(0.8f, 0.1f, 0.1f),
			FLinearColor(0.1f, 0.8f, 0.2f),
			HealthPct);
		FCanvasTileItem BarFill(
			FVector2D(BarX + 2, BarY + 2),
			FVector2D((BarWidth - 4) * HealthPct, BarHeight - 4),
			HealthColor);
		BarFill.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(BarFill);

		// Health text
		FString HealthText = FString::Printf(TEXT("HP: %d / %d"), Robot->Health, Robot->MaxHealth);
		FCanvasTextItem HPText(FVector2D(BarX + 6, BarY + 3),
			FText::FromString(HealthText), Font, FLinearColor::White);
		HPText.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(HPText);

		// Lives display
		FString LivesStr = FString::Printf(TEXT("Lives: %d"), Robot->Lives);
		FLinearColor LivesColor = (Robot->Lives <= 1) ? FLinearColor(1.0f, 0.3f, 0.0f) : FLinearColor(0.3f, 1.0f, 0.3f);
		FCanvasTextItem LivesText(FVector2D(BarX, BarY + BarHeight + 4),
			FText::FromString(LivesStr), Font, LivesColor);
		LivesText.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(LivesText);

		// Checkpoint progress
		FString CheckpointStr = FString::Printf(TEXT("Checkpoint: %d"), Robot->CurrentCheckpoint);
		FCanvasTextItem CPText(FVector2D(BarX, BarY + BarHeight + 24),
			FText::FromString(CheckpointStr), Font, FLinearColor::Yellow);
		CPText.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(CPText);

		// Game state - read from GameState in network mode, GameMode in standalone
		FString StateText;
		EGameState CurrentGameState = EGameState::Programming;

		ARobotRallyGameState* GS = GetRobotRallyGameState();
		ARobotRallyGameMode* GM = Cast<ARobotRallyGameMode>(GetWorld()->GetAuthGameMode());

		if (GS && GetWorld()->GetNetMode() != NM_Standalone)
		{
			CurrentGameState = GS->Rep_CurrentGameState;
		}
		else if (GM)
		{
			CurrentGameState = GM->CurrentState;
		}

		switch (CurrentGameState)
		{
		case EGameState::Programming: StateText = TEXT("PROGRAMMING (1-9 = cards, Bksp = undo, E = go)"); break;
		case EGameState::Executing:   StateText = TEXT("EXECUTING..."); break;
		case EGameState::GameOver:    StateText = Robot->bIsAlive ? TEXT("VICTORY!") : TEXT("GAME OVER"); break;
		}

		FCanvasTextItem StateItem(FVector2D(BarX, BarY + BarHeight + 44),
			FText::FromString(StateText), Font, FLinearColor(0.0f, 1.0f, 1.0f));
		StateItem.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(StateItem);

		// Grid position
		FString PosText = FString::Printf(TEXT("Position: (%d, %d)"), Robot->GridX, Robot->GridY);
		FCanvasTextItem PosItem(FVector2D(BarX, BarY + BarHeight + 64),
			FText::FromString(PosText), Font, FLinearColor(0.7f, 0.7f, 0.7f));
		PosItem.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(PosItem);
	}

	DrawCardSelection();

	// Network debug overlay (only in network mode)
	if (GetWorld()->GetNetMode() != NM_Standalone)
	{
		DrawNetworkDebug();
	}
}

void ARobotRallyHUD::DrawCardSelection()
{
	bool bIsNetwork = (GetWorld()->GetNetMode() != NM_Standalone);

	// Determine game state
	EGameState CurrentGameState = EGameState::Programming;
	if (bIsNetwork)
	{
		ARobotRallyGameState* GS = GetRobotRallyGameState();
		if (GS) CurrentGameState = GS->Rep_CurrentGameState;
	}
	else
	{
		ARobotRallyGameMode* GM = Cast<ARobotRallyGameMode>(GetWorld()->GetAuthGameMode());
		if (GM) CurrentGameState = GM->CurrentState;
	}

	if (CurrentGameState != EGameState::Programming) return;

	// In network mode, read from PlayerState; in standalone, read from GameMode
	TArray<FRobotCard> HandCards;
	TArray<int32> RegisterSlots;
	int32 RobotIndex = 0;

	if (bIsNetwork)
	{
		ARobotRallyPlayerState* PS = GetLocalPlayerState();
		if (!PS || PS->Rep_HandCards.Num() == 0) return;

		HandCards = PS->Rep_HandCards;
		RegisterSlots = PS->Rep_RegisterSlots;

		// Get robot index from GameState
		ARobotRallyGameState* GS = GetRobotRallyGameState();
		if (GS && PS->Rep_Robot)
		{
			RobotIndex = GS->AllRobots.Find(PS->Rep_Robot);
		}
	}
	else
	{
		ARobotRallyGameMode* GM = Cast<ARobotRallyGameMode>(GetWorld()->GetAuthGameMode());
		if (!GM || !GM->Robots.IsValidIndex(0)) return;

		ARobotPawn* PlayerRobot = GM->Robots[0];
		FRobotProgram* PlayerProgram = nullptr;
		for (FRobotProgram& Program : GM->RobotPrograms)
		{
			if (Program.Robot == PlayerRobot)
			{
				PlayerProgram = &Program;
				break;
			}
		}

		if (!PlayerProgram || PlayerProgram->HandCards.Num() == 0) return;

		HandCards = PlayerProgram->HandCards;
		RegisterSlots = PlayerProgram->RegisterSlots;
		RobotIndex = GM->Robots.Find(PlayerRobot);
	}

	UFont* Font = GEngine->GetLargeFont();
	const float Padding = 10.0f;
	const float SlotHeight = 28.0f;
	const float PanelWidth = 280.0f;
	const float PanelX = Canvas->SizeX - PanelWidth - Padding;
	float CurY = Padding;

	// --- Panel background ---
	float TotalHeight = SlotHeight * (ARobotRallyGameMode::NUM_REGISTERS + HandCards.Num() + 4);
	FCanvasTileItem PanelBG(
		FVector2D(PanelX, CurY),
		FVector2D(PanelWidth, TotalHeight),
		FLinearColor(0.0f, 0.0f, 0.0f, 0.7f));
	PanelBG.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(PanelBG);

	float TextX = PanelX + Padding;

	// --- Registers header ---
	FString HeaderText = FString::Printf(TEXT("ROBOT %d - PROGRAMMING"), RobotIndex);
	FCanvasTextItem Header(FVector2D(TextX, CurY),
		FText::FromString(HeaderText), Font, FLinearColor(1.0f, 1.0f, 0.0f));
	Header.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(Header);
	CurY += SlotHeight;

	// Count filled registers
	int32 FilledCount = 0;
	for (int32 i = 0; i < ARobotRallyGameMode::NUM_REGISTERS; ++i)
	{
		if (RegisterSlots.IsValidIndex(i) && RegisterSlots[i] != -1)
			FilledCount++;
	}

	// --- Register slots R1-R5 ---
	for (int32 i = 0; i < ARobotRallyGameMode::NUM_REGISTERS; ++i)
	{
		FString SlotText;
		FLinearColor SlotColor;

		bool bFilled = RegisterSlots.IsValidIndex(i) && RegisterSlots[i] != -1;
		if (bFilled)
		{
			int32 HandIdx = RegisterSlots[i];
			if (HandCards.IsValidIndex(HandIdx))
			{
				FString CardName = ARobotRallyGameMode::GetCardActionName(HandCards[HandIdx].Action);
				SlotText = FString::Printf(TEXT("R%d: %s (P%d)"), i + 1, *CardName, HandCards[HandIdx].Priority);
			}
			else
			{
				SlotText = FString::Printf(TEXT("R%d: ???"), i + 1);
			}
			SlotColor = FLinearColor(0.2f, 0.9f, 0.2f); // Green
		}
		else if (i == FilledCount)
		{
			SlotText = FString::Printf(TEXT("R%d: [next]"), i + 1);
			SlotColor = FLinearColor(1.0f, 1.0f, 0.3f); // Yellow
		}
		else
		{
			SlotText = FString::Printf(TEXT("R%d: ---"), i + 1);
			SlotColor = FLinearColor(0.5f, 0.5f, 0.5f); // Gray
		}

		FCanvasTextItem SlotItem(FVector2D(TextX, CurY),
			FText::FromString(SlotText), Font, SlotColor);
		SlotItem.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(SlotItem);
		CurY += SlotHeight;
	}

	// --- Hand header ---
	CurY += SlotHeight * 0.5f;
	FCanvasTextItem HandHeader(FVector2D(TextX, CurY),
		FText::FromString(TEXT("HAND")), Font, FLinearColor(1.0f, 1.0f, 0.0f));
	HandHeader.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(HandHeader);
	CurY += SlotHeight;

	// --- Hand cards ---
	for (int32 i = 0; i < HandCards.Num(); ++i)
	{
		FString CardName = ARobotRallyGameMode::GetCardActionName(HandCards[i].Action);

		// Check if this card is assigned to a register
		bool bAssigned = false;
		for (int32 Slot : RegisterSlots)
		{
			if (Slot == i) { bAssigned = true; break; }
		}

		FString CardText;
		FLinearColor CardColor;
		if (bAssigned)
		{
			CardText = FString::Printf(TEXT("%d: %s (P%d) [R]"), i + 1, *CardName, HandCards[i].Priority);
			CardColor = FLinearColor(0.4f, 0.4f, 0.4f); // Gray = already assigned
		}
		else
		{
			CardText = FString::Printf(TEXT("%d: %s (P%d)"), i + 1, *CardName, HandCards[i].Priority);
			CardColor = FLinearColor::White;
		}

		FCanvasTextItem CardItem(FVector2D(TextX, CurY),
			FText::FromString(CardText), Font, CardColor);
		CardItem.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(CardItem);
		CurY += SlotHeight;
	}

	// --- Instructions ---
	CurY += SlotHeight * 0.5f;
	FString InstructionText;
	if (FilledCount >= ARobotRallyGameMode::NUM_REGISTERS)
	{
		InstructionText = TEXT("Press E to execute!");
	}
	else
	{
		InstructionText = TEXT("1-9 = select, Bksp = undo");
	}
	FCanvasTextItem InstrItem(FVector2D(TextX, CurY),
		FText::FromString(InstructionText), Font, FLinearColor(0.0f, 1.0f, 1.0f));
	InstrItem.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(InstrItem);
}

void ARobotRallyHUD::DrawNetworkDebug()
{
	UFont* Font = GEngine->GetSmallFont();
	const float Padding = 10.0f;
	float CurY = Canvas->SizeY - 30.0f;

	// Net mode
	FString NetModeStr;
	switch (GetWorld()->GetNetMode())
	{
	case NM_Standalone:      NetModeStr = TEXT("Standalone"); break;
	case NM_DedicatedServer: NetModeStr = TEXT("Dedicated Server"); break;
	case NM_ListenServer:    NetModeStr = TEXT("Listen Server"); break;
	case NM_Client:          NetModeStr = TEXT("Client"); break;
	}

	// Player count from GameState
	int32 PlayerCount = 0;
	ARobotRallyGameState* GS = GetRobotRallyGameState();
	if (GS)
	{
		PlayerCount = GS->PlayerArray.Num();
	}

	FString DebugText = FString::Printf(TEXT("NET: %s | Players: %d"), *NetModeStr, PlayerCount);
	FCanvasTextItem DebugItem(FVector2D(Canvas->SizeX - 300.0f, CurY),
		FText::FromString(DebugText), Font, FLinearColor(0.5f, 0.8f, 1.0f, 0.8f));
	DebugItem.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(DebugItem);
}

void ARobotRallyHUD::AddEventMessage(const FString& Text, FColor Color)
{
	FEventMessage Msg;
	Msg.Text = Text;
	Msg.Color = Color;
	Msg.TimeRemaining = MESSAGE_DURATION;
	Messages.Add(Msg);

	while (Messages.Num() > MAX_MESSAGES)
	{
		Messages.RemoveAt(0);
	}
}
