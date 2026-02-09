// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotRallyGameState.h"
#include "RobotRallyHUD.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

ARobotRallyGameState::ARobotRallyGameState()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void ARobotRallyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARobotRallyGameState, Rep_CurrentGameState);
	DOREPLIFETIME(ARobotRallyGameState, Rep_CurrentRegister);
	DOREPLIFETIME(ARobotRallyGameState, AllRobots);
	DOREPLIFETIME(ARobotRallyGameState, Rep_GridWidth);
	DOREPLIFETIME(ARobotRallyGameState, Rep_GridHeight);
	DOREPLIFETIME(ARobotRallyGameState, Rep_TileOverrides);
	DOREPLIFETIME(ARobotRallyGameState, Rep_TotalCheckpoints);
}

void ARobotRallyGameState::OnRep_CurrentGameState()
{
	UE_LOG(LogTemp, Log, TEXT("GameState replicated: %d"), (int32)Rep_CurrentGameState);
}

void ARobotRallyGameState::MulticastShowEventMessage_Implementation(const FString& Text, FColor Color)
{
	// On each client, find the local player's HUD and add the message
	UWorld* World = GetWorld();
	if (!World) return;

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->IsLocalController())
		{
			ARobotRallyHUD* HUD = Cast<ARobotRallyHUD>(PC->GetHUD());
			if (HUD)
			{
				HUD->AddEventMessage(Text, Color);
			}
		}
	}
}
