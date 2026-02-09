// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotRallyPlayerState.h"
#include "Net/UnrealNetwork.h"

ARobotRallyPlayerState::ARobotRallyPlayerState()
{
}

void ARobotRallyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ARobotRallyPlayerState, Rep_HandCards, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ARobotRallyPlayerState, Rep_RegisterSlots, COND_OwnerOnly);
	DOREPLIFETIME(ARobotRallyPlayerState, Rep_Robot);
	DOREPLIFETIME(ARobotRallyPlayerState, bIsReady);
}

void ARobotRallyPlayerState::OnRep_HandCards()
{
	UE_LOG(LogTemp, Log, TEXT("PlayerState: Hand replicated (%d cards)"), Rep_HandCards.Num());
}

void ARobotRallyPlayerState::OnRep_RegisterSlots()
{
	UE_LOG(LogTemp, Log, TEXT("PlayerState: Registers replicated (%d slots)"), Rep_RegisterSlots.Num());
}
