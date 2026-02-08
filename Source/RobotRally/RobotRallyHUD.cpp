// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotRallyHUD.h"
#include "RobotPawn.h"
#include "RobotRallyGameMode.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "GameFramework/PlayerController.h"

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

		// Game state
		FString StateText;
		ARobotRallyGameMode* GM = Cast<ARobotRallyGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			switch (GM->CurrentState)
			{
			case EGameState::Programming: StateText = TEXT("PROGRAMMING (1-9 = cards, Bksp = undo, E = go)"); break;
			case EGameState::Executing:   StateText = TEXT("EXECUTING..."); break;
			case EGameState::GameOver:    StateText = Robot->bIsAlive ? TEXT("VICTORY!") : TEXT("GAME OVER"); break;
			}
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
}

void ARobotRallyHUD::DrawCardSelection()
{
	ARobotRallyGameMode* GM = Cast<ARobotRallyGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM || GM->CurrentState != EGameState::Programming) return;

	// Get player's robot (Robot[0])
	if (!GM->Robots.IsValidIndex(0)) return;
	ARobotPawn* PlayerRobot = GM->Robots[0];

	// Find player's program
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

	UFont* Font = GEngine->GetLargeFont();
	const float Padding = 10.0f;
	const float SlotHeight = 28.0f;
	const float PanelWidth = 280.0f;
	const float PanelX = Canvas->SizeX - PanelWidth - Padding;
	float CurY = Padding;

	// --- Panel background ---
	float TotalHeight = SlotHeight * (ARobotRallyGameMode::NUM_REGISTERS + PlayerProgram->HandCards.Num() + 4);
	FCanvasTileItem PanelBG(
		FVector2D(PanelX, CurY),
		FVector2D(PanelWidth, TotalHeight),
		FLinearColor(0.0f, 0.0f, 0.0f, 0.7f));
	PanelBG.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(PanelBG);

	float TextX = PanelX + Padding;

	// --- Registers header ---
	int32 PlayerRobotIndex = GM->Robots.Find(PlayerRobot);
	FString HeaderText = FString::Printf(TEXT("ROBOT %d - PROGRAMMING"), PlayerRobotIndex);
	FCanvasTextItem Header(FVector2D(TextX, CurY),
		FText::FromString(HeaderText), Font, FLinearColor(1.0f, 1.0f, 0.0f));
	Header.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(Header);
	CurY += SlotHeight;

	// Count filled registers to determine "next" slot
	int32 FilledCount = 0;
	for (int32 i = 0; i < ARobotRallyGameMode::NUM_REGISTERS; ++i)
	{
		if (PlayerProgram->RegisterSlots.IsValidIndex(i) && PlayerProgram->RegisterSlots[i] != -1)
			FilledCount++;
	}

	// --- Register slots R1-R5 ---
	for (int32 i = 0; i < ARobotRallyGameMode::NUM_REGISTERS; ++i)
	{
		FString SlotText;
		FLinearColor SlotColor;

		bool bFilled = PlayerProgram->RegisterSlots.IsValidIndex(i) && PlayerProgram->RegisterSlots[i] != -1;
		if (bFilled)
		{
			int32 HandIdx = PlayerProgram->RegisterSlots[i];
			if (PlayerProgram->HandCards.IsValidIndex(HandIdx))
			{
				FString CardName = ARobotRallyGameMode::GetCardActionName(PlayerProgram->HandCards[HandIdx].Action);
				SlotText = FString::Printf(TEXT("R%d: %s (P%d)"), i + 1, *CardName, PlayerProgram->HandCards[HandIdx].Priority);
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
	for (int32 i = 0; i < PlayerProgram->HandCards.Num(); ++i)
	{
		FString CardName = ARobotRallyGameMode::GetCardActionName(PlayerProgram->HandCards[i].Action);
		bool bAssigned = GM->IsCardInRegister(PlayerProgram, i);

		FString CardText;
		FLinearColor CardColor;
		if (bAssigned)
		{
			CardText = FString::Printf(TEXT("%d: %s (P%d) [R]"), i + 1, *CardName, PlayerProgram->HandCards[i].Priority);
			CardColor = FLinearColor(0.4f, 0.4f, 0.4f); // Gray = already assigned
		}
		else
		{
			CardText = FString::Printf(TEXT("%d: %s (P%d)"), i + 1, *CardName, PlayerProgram->HandCards[i].Priority);
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
