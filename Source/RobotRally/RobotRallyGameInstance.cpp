// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "RobotRallyGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

URobotRallyGameInstance::URobotRallyGameInstance()
{
}

void URobotRallyGameInstance::HostSession(int32 MaxPlayers)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub)
	{
		UE_LOG(LogTemp, Error, TEXT("HostSession: No OnlineSubsystem found!"));
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("HostSession: No Session interface!"));
		return;
	}

	DesiredMaxPlayers = MaxPlayers;

	// Remove any existing delegate before adding a new one
	if (CreateSessionDelegateHandle.IsValid())
	{
		Sessions->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionDelegateHandle);
	}

	CreateSessionDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &URobotRallyGameInstance::OnCreateSessionComplete));

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = MaxPlayers;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = false;

	UE_LOG(LogTemp, Log, TEXT("HostSession: Creating LAN session for %d players..."), MaxPlayers);
	Sessions->CreateSession(0, NAME_GameSession, SessionSettings);
}

void URobotRallyGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Log, TEXT("OnCreateSessionComplete: %s, Success=%d"), *SessionName.ToString(), bWasSuccessful);

	if (bWasSuccessful)
	{
		// Travel to the game map as a listen server
		UWorld* World = GetWorld();
		if (World)
		{
			FString TravelURL = TEXT("/Game/Maps/MainMap?listen");
			World->ServerTravel(TravelURL);
			UE_LOG(LogTemp, Log, TEXT("ServerTravel to: %s"), *TravelURL);
		}
	}
}

void URobotRallyGameInstance::FindSessions()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub)
	{
		UE_LOG(LogTemp, Error, TEXT("FindSessions: No OnlineSubsystem found!"));
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("FindSessions: No Session interface!"));
		return;
	}

	// Remove any existing delegate
	if (FindSessionsDelegateHandle.IsValid())
	{
		Sessions->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsDelegateHandle);
	}

	FindSessionsDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &URobotRallyGameInstance::OnFindSessionsComplete));

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 20;
	UE_LOG(LogTemp, Log, TEXT("FindSessions: Searching LAN..."));
	Sessions->FindSessions(0, SessionSearch.ToSharedRef());
}

void URobotRallyGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (bWasSuccessful && SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("FindSessions: Found %d sessions"), SessionSearch->SearchResults.Num());
		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); ++i)
		{
			UE_LOG(LogTemp, Log, TEXT("  Session %d: %s"), i,
				*SessionSearch->SearchResults[i].GetSessionIdStr());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FindSessions: Search failed or no results"));
	}
}

void URobotRallyGameInstance::JoinFoundSession(int32 SessionIndex)
{
	if (!SessionSearch.IsValid() || !SessionSearch->SearchResults.IsValidIndex(SessionIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("JoinFoundSession: Invalid session index %d"), SessionIndex);
		return;
	}

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub) return;

	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid()) return;

	// Remove any existing delegate
	if (JoinSessionDelegateHandle.IsValid())
	{
		Sessions->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionDelegateHandle);
	}

	JoinSessionDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionCompleteDelegate::CreateUObject(this, &URobotRallyGameInstance::OnJoinSessionComplete));

	UE_LOG(LogTemp, Log, TEXT("JoinFoundSession: Joining session %d..."), SessionIndex);
	Sessions->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[SessionIndex]);
}

void URobotRallyGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Log, TEXT("OnJoinSessionComplete: %s, Result=%d"), *SessionName.ToString(), (int32)Result);

	if (Result != EOnJoinSessionCompleteResult::Success) return;

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub) return;

	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid()) return;

	FString ConnectString;
	if (Sessions->GetResolvedConnectString(SessionName, ConnectString))
	{
		APlayerController* PC = GetFirstLocalPlayerController();
		if (PC)
		{
			PC->ClientTravel(ConnectString, TRAVEL_Absolute);
			UE_LOG(LogTemp, Log, TEXT("ClientTravel to: %s"), *ConnectString);
		}
	}
}
