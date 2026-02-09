// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "RobotRallyGameInstance.generated.h"

UCLASS()
class ROBOTRALLY_API URobotRallyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	URobotRallyGameInstance();

	// --- Session management ---

	UFUNCTION(BlueprintCallable, Category = "Session")
	void HostSession(int32 MaxPlayers = 4);

	UFUNCTION(BlueprintCallable, Category = "Session")
	void FindSessions();

	UFUNCTION(BlueprintCallable, Category = "Session")
	void JoinFoundSession(int32 SessionIndex = 0);

	// Session search results (populated after FindSessions completes)
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

private:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	FDelegateHandle CreateSessionDelegateHandle;
	FDelegateHandle FindSessionsDelegateHandle;
	FDelegateHandle JoinSessionDelegateHandle;

	int32 DesiredMaxPlayers = 4;
};
