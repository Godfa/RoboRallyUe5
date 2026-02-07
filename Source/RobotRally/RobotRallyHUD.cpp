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

		// Checkpoint progress
		FString CheckpointStr = FString::Printf(TEXT("Checkpoint: %d"), Robot->CurrentCheckpoint);
		FCanvasTextItem CPText(FVector2D(BarX, BarY + BarHeight + 4),
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
			case EGameState::Programming: StateText = TEXT("PROGRAMMING (WASD = move, E = execute)"); break;
			case EGameState::Executing:   StateText = TEXT("EXECUTING..."); break;
			case EGameState::GameOver:    StateText = Robot->bIsAlive ? TEXT("VICTORY!") : TEXT("GAME OVER"); break;
			}
		}
		FCanvasTextItem StateItem(FVector2D(BarX, BarY + BarHeight + 24),
			FText::FromString(StateText), Font, FLinearColor(0.0f, 1.0f, 1.0f));
		StateItem.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(StateItem);

		// Grid position
		FString PosText = FString::Printf(TEXT("Position: (%d, %d)"), Robot->GridX, Robot->GridY);
		FCanvasTextItem PosItem(FVector2D(BarX, BarY + BarHeight + 44),
			FText::FromString(PosText), Font, FLinearColor(0.7f, 0.7f, 0.7f));
		PosItem.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(PosItem);
	}
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
