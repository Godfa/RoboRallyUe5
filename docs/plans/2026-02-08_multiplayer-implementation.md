# Multiplayer Implementation Plan

## Context

RobotRally is currently a **single-player** game where one local player controls robots. The game already supports **multiple robots** executing cards in priority order, which provides a strong foundation for multiplayer.

**Current State:**
- ✅ Multi-robot support (TArray<ARobotPawn*>)
- ✅ Priority-based card execution (FExecutionQueueEntry)
- ✅ Game state machine (Programming → Executing → GameOver)
- ✅ Card deck system with dealing and discards
- ❌ No network replication
- ❌ No session management
- ❌ Single-machine only

**Goal:** Enable **2-8 players** to play RobotRally together over a network, each controlling their own robot. Players program cards simultaneously during Programming phase, then watch synchronized execution.

**Why this matters:**
- RobotRally is fundamentally a multiplayer board game
- Turn-based gameplay fits well with client-server architecture
- Network lag is manageable (programming phase has no real-time pressure)
- Foundation for online matchmaking and ranked play

---

## Architecture: UE5 Network Replication (Client-Server)

### Design Decision: Dedicated Server Architecture

**Selected:** Client-Server with authoritative server
- **Server** owns game state (GridManager, GameMode, robots)
- **Clients** send input (card selections), receive state updates
- **Authority:** Server validates all actions, prevents cheating
- **Replication:** UPROPERTY(Replicated), RPCs (Server/Client/Multicast)

**Why this over alternatives:**
- ✅ Turn-based gameplay = no frame-perfect sync needed
- ✅ Server authority prevents card duplication exploits
- ✅ UE5 built-in replication handles most heavy lifting
- ✅ Easy to add AI opponents (server-controlled)
- ✅ Scales to 8 players without peer-to-peer complexity
- ❌ Requires dedicated server or listen server (host = player + server)

**Rejected Alternatives:**
- **Peer-to-Peer:** Too complex for state synchronization, host migration issues
- **Lockstep:** Overkill for turn-based game, all clients must tick together
- **Rollback Netcode:** Only needed for fighting games with frame-perfect input

### Network Ownership Model

| Actor/Component | Authority | Replication |
|-----------------|-----------|-------------|
| **ARobotRallyGameMode** | Server only | Not replicated (GameMode never replicates) |
| **ARobotRallyGameState** | Server | → All Clients |
| **ARobotRallyPlayerState** | Server | → All Clients |
| **AGridManager** | Server | → All Clients (tile changes rare) |
| **ARobotPawn** | Server | → All Clients (position, health) |
| **URobotMovementComponent** | Server | Position replicated via ARobotPawn |

**Key Insight:** GameMode is **server-only**. We create **GameState** (replicated) to hold shared state visible to all clients.

---

## Implementation Plan

### Phase 1: GameState & PlayerState Foundation

**Goal:** Create replicated containers for game state and per-player data.

**1.1 Create RobotRallyGameState**

**File:** `Source/RobotRally/RobotRallyGameState.h`

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RobotRallyGameMode.h"  // For EGameState, FRobotCard
#include "RobotRallyGameState.generated.h"

class AGridManager;
class ARobotPawn;

UCLASS()
class ROBOTRALLY_API ARobotRallyGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    ARobotRallyGameState();

    // Enable replication
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Current game phase (replicated to all clients)
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Game")
    EGameState CurrentGameState = EGameState::Programming;

    // Current register being executed (0-4)
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Game")
    int32 CurrentRegister = 0;

    // Reference to grid (spawned by server, replicated to clients)
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Game")
    AGridManager* GridManager;

    // All robots in game (server spawns, clients receive references)
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Game")
    TArray<ARobotPawn*> Robots;

    // Total checkpoints required to win
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Game")
    int32 TotalCheckpoints = 0;

    // Track which players have committed their programs
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Game")
    TArray<APlayerState*> ReadyPlayers;

    // Helper: Check if all alive players are ready
    UFUNCTION(BlueprintPure, Category = "Game")
    bool AreAllPlayersReady() const;
};
```

**File:** `Source/RobotRally/RobotRallyGameState.cpp`

```cpp
#include "RobotRallyGameState.h"
#include "Net/UnrealNetwork.h"
#include "RobotPawn.h"

ARobotRallyGameState::ARobotRallyGameState()
{
    // Enable replication for this actor
    bReplicates = true;
}

void ARobotRallyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARobotRallyGameState, CurrentGameState);
    DOREPLIFETIME(ARobotRallyGameState, CurrentRegister);
    DOREPLIFETIME(ARobotRallyGameState, GridManager);
    DOREPLIFETIME(ARobotRallyGameState, Robots);
    DOREPLIFETIME(ARobotRallyGameState, TotalCheckpoints);
    DOREPLIFETIME(ARobotRallyGameState, ReadyPlayers);
}

bool ARobotRallyGameState::AreAllPlayersReady() const
{
    // Count living robots with player controllers
    int32 AlivePlayers = 0;
    for (ARobotPawn* Robot : Robots)
    {
        if (Robot && Robot->bIsAlive && Robot->GetController() && Robot->GetController()->IsPlayerController())
        {
            AlivePlayers++;
        }
    }

    // Check if all alive players are in ready list
    return ReadyPlayers.Num() >= AlivePlayers;
}
```

**1.2 Create RobotRallyPlayerState**

**File:** `Source/RobotRally/RobotRallyPlayerState.h`

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RobotRallyGameMode.h"  // For FRobotCard
#include "RobotRallyPlayerState.generated.h"

class ARobotPawn;

UCLASS()
class ROBOTRALLY_API ARobotRallyPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    ARobotRallyPlayerState();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Robot owned by this player (spawned by server)
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    ARobotPawn* ControlledRobot;

    // Player's hand (visible only to owning client + server)
    UPROPERTY(ReplicatedUsing = OnRep_HandCards, VisibleAnywhere, BlueprintReadOnly, Category = "Player|Cards")
    TArray<FRobotCard> HandCards;

    // Programmed registers (visible to all for spectating)
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Player|Cards")
    TArray<int32> RegisterSlots;  // Hand indices (-1 = empty)

    // Committed program (visible to all during execution)
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Player|Cards")
    TArray<FRobotCard> CommittedProgram;

    // Has this player finished programming?
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    bool bIsReady = false;

    // RepNotify for hand updates (client-side UI refresh)
    UFUNCTION()
    void OnRep_HandCards();
};
```

**File:** `Source/RobotRally/RobotRallyPlayerState.cpp`

```cpp
#include "RobotRallyPlayerState.h"
#include "Net/UnrealNetwork.h"

ARobotRallyPlayerState::ARobotRallyPlayerState()
{
    bReplicates = true;

    // Initialize registers
    RegisterSlots.Init(-1, 5);
}

void ARobotRallyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARobotRallyPlayerState, ControlledRobot);
    DOREPLIFETIME(ARobotRallyPlayerState, RegisterSlots);
    DOREPLIFETIME(ARobotRallyPlayerState, CommittedProgram);
    DOREPLIFETIME(ARobotRallyPlayerState, bIsReady);

    // Hand cards replicate only to owner
    DOREPLIFETIME_CONDITION(ARobotRallyPlayerState, HandCards, COND_OwnerOnly);
}

void ARobotRallyPlayerState::OnRep_HandCards()
{
    // Trigger UI refresh (to be implemented in Phase 6)
    UE_LOG(LogTemp, Log, TEXT("Hand updated: %d cards"), HandCards.Num());
}
```

**1.3 Update GameMode to Use GameState**

**File:** `RobotRallyGameMode.h`

```cpp
// Add to ARobotRallyGameMode class:

// Override to specify custom GameState class
virtual void InitGameState() override;

// Get typed GameState (convenience)
UFUNCTION(BlueprintPure, Category = "Game")
ARobotRallyGameState* GetRobotRallyGameState() const;
```

**File:** `RobotRallyGameMode.cpp`

```cpp
// In constructor:
ARobotRallyGameMode::ARobotRallyGameMode()
{
    // Set custom GameState class
    GameStateClass = ARobotRallyGameState::StaticClass();
    PlayerStateClass = ARobotRallyPlayerState::StaticClass();

    // ... existing code ...
}

void ARobotRallyGameMode::InitGameState()
{
    Super::InitGameState();

    // Cache reference to typed GameState
    ARobotRallyGameState* GS = GetRobotRallyGameState();
    if (GS)
    {
        GS->CurrentGameState = CurrentState;
    }
}

ARobotRallyGameState* ARobotRallyGameMode::GetRobotRallyGameState() const
{
    return Cast<ARobotRallyGameState>(GameState);
}
```

**Files Created/Modified:**
- `RobotRallyGameState.h/cpp` (NEW)
- `RobotRallyPlayerState.h/cpp` (NEW)
- `RobotRallyGameMode.h/cpp` (MODIFY)

---

### Phase 2: Robot & Grid Replication

**Goal:** Ensure robots and grid state replicate to all clients.

**2.1 Enable ARobotPawn Replication**

**File:** `RobotPawn.h`

```cpp
// Add to ARobotPawn class:

virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

// Replicated properties
UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Robot|Grid")
int32 GridX;

UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Robot|Grid")
int32 GridY;

UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, BlueprintReadOnly, Category = "Robot|Health")
int32 Health = 10;

UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Robot|Health")
bool bIsAlive = true;

UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Robot|Checkpoint")
int32 CurrentCheckpoint = 0;

UFUNCTION()
void OnRep_Health();  // Update HUD when health changes
```

**File:** `RobotPawn.cpp`

```cpp
#include "Net/UnrealNetwork.h"

ARobotPawn::ARobotPawn()
{
    // Enable replication
    bReplicates = true;
    SetReplicateMovement(true);  // Replicate location/rotation
}

void ARobotPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARobotPawn, GridX);
    DOREPLIFETIME(ARobotPawn, GridY);
    DOREPLIFETIME(ARobotPawn, Health);
    DOREPLIFETIME(ARobotPawn, bIsAlive);
    DOREPLIFETIME(ARobotPawn, CurrentCheckpoint);
}

void ARobotPawn::OnRep_Health()
{
    // Refresh HUD (to be implemented)
    UE_LOG(LogTemp, Log, TEXT("Robot health: %d"), Health);
}
```

**2.2 Enable AGridManager Replication**

**File:** `GridManager.h`

```cpp
// Add to AGridManager class:

virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

// Replicated grid data
UPROPERTY(Replicated, VisibleAnywhere, Category = "Grid")
TMap<FIntVector, FTileData> GridMap;
```

**File:** `GridManager.cpp`

```cpp
#include "Net/UnrealNetwork.h"

AGridManager::AGridManager()
{
    bReplicates = true;
    bAlwaysRelevant = true;  // Always replicate to all clients
}

void AGridManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGridManager, GridMap);
}
```

**Files Modified:**
- `RobotPawn.h/cpp`
- `GridManager.h/cpp`

---

### Phase 3: Card Selection via RPCs

**Goal:** Players select cards on their client, send to server for validation, server replicates state.

**3.1 Define RPCs in PlayerController**

**File:** `RobotController.h`

```cpp
// Add to ARobotController class:

// Client → Server: Player wants to select a card
UFUNCTION(Server, Reliable, WithValidation)
void ServerSelectCard(int32 HandIndex);

// Client → Server: Player wants to undo last selection
UFUNCTION(Server, Reliable, WithValidation)
void ServerUndoSelection();

// Client → Server: Player has finished programming (all 5 registers filled)
UFUNCTION(Server, Reliable, WithValidation)
void ServerCommitProgram();

// Server → Client: Notify client of invalid action
UFUNCTION(Client, Reliable)
void ClientShowError(const FString& ErrorMessage);
```

**File:** `RobotController.cpp`

```cpp
void ARobotController::SelectCard()
{
    // Get hand index from input (1-9 keys)
    int32 HandIndex = DetermineHandIndexFromInput();

    // Local prediction: Update UI immediately
    // (Server will validate and correct if needed)

    // Send to server
    ServerSelectCard(HandIndex);
}

void ARobotController::ServerSelectCard_Implementation(int32 HandIndex)
{
    // Server validates the selection
    ARobotRallyPlayerState* PS = GetPlayerState<ARobotRallyPlayerState>();
    if (!PS || !PS->ControlledRobot)
    {
        ClientShowError(TEXT("Invalid player state"));
        return;
    }

    // Validate hand index
    if (HandIndex < 0 || HandIndex >= PS->HandCards.Num())
    {
        ClientShowError(TEXT("Invalid card index"));
        return;
    }

    // Check if already in registers
    if (PS->RegisterSlots.Contains(HandIndex))
    {
        ClientShowError(TEXT("Card already selected"));
        return;
    }

    // Check if registers full
    int32 EmptySlot = PS->RegisterSlots.Find(-1);
    if (EmptySlot == INDEX_NONE)
    {
        ClientShowError(TEXT("All registers full"));
        return;
    }

    // Valid! Update server state (replicates to all clients)
    PS->RegisterSlots[EmptySlot] = HandIndex;

    // Check if player is now ready (all 5 filled)
    if (!PS->RegisterSlots.Contains(-1))
    {
        PS->bIsReady = true;

        // Add to GameState ready list
        ARobotRallyGameState* GS = GetWorld()->GetGameState<ARobotRallyGameState>();
        if (GS && !GS->ReadyPlayers.Contains(PS))
        {
            GS->ReadyPlayers.Add(PS);
        }

        // Check if all players ready → start execution
        ARobotRallyGameMode* GM = GetWorld()->GetAuthGameMode<ARobotRallyGameMode>();
        if (GM && GS && GS->AreAllPlayersReady())
        {
            GM->StartExecutionPhase();
        }
    }
}

bool ARobotController::ServerSelectCard_Validate(int32 HandIndex)
{
    // Basic cheat protection
    return HandIndex >= 0 && HandIndex < 100;
}

void ARobotController::ServerUndoSelection_Implementation()
{
    ARobotRallyPlayerState* PS = GetPlayerState<ARobotRallyPlayerState>();
    if (!PS) return;

    // Find last filled register
    int32 LastFilledIndex = -1;
    for (int32 i = 4; i >= 0; --i)
    {
        if (PS->RegisterSlots[i] != -1)
        {
            LastFilledIndex = i;
            break;
        }
    }

    if (LastFilledIndex != -1)
    {
        PS->RegisterSlots[LastFilledIndex] = -1;
        PS->bIsReady = false;

        // Remove from ready list
        ARobotRallyGameState* GS = GetWorld()->GetGameState<ARobotRallyGameState>();
        if (GS)
        {
            GS->ReadyPlayers.Remove(PS);
        }
    }
}

bool ARobotController::ServerUndoSelection_Validate()
{
    return true;
}

void ARobotController::ClientShowError_Implementation(const FString& ErrorMessage)
{
    // Display error to player (UI or HUD message)
    UE_LOG(LogTemp, Warning, TEXT("Server rejected action: %s"), *ErrorMessage);
}
```

**Files Modified:**
- `RobotController.h/cpp`

---

### Phase 4: Server-Authoritative Execution

**Goal:** Server executes all card actions, replicates movements to clients.

**4.1 Update GameMode Execution Logic**

**File:** `RobotRallyGameMode.cpp`

```cpp
void ARobotRallyGameMode::StartExecutionPhase()
{
    // Only server runs this
    if (!HasAuthority()) return;

    CurrentState = EGameState::Executing;

    // Update GameState (replicates to clients)
    ARobotRallyGameState* GS = GetRobotRallyGameState();
    if (GS)
    {
        GS->CurrentGameState = EGameState::Executing;
    }

    // Commit all player programs from PlayerStates
    CommitProgramsFromPlayerStates();

    // Build execution queue for first register
    CurrentRegister = 0;
    BuildExecutionQueue(CurrentRegister);

    // Start execution
    ProcessExecutionQueue();
}

void ARobotRallyGameMode::CommitProgramsFromPlayerStates()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        ARobotRallyPlayerState* PS = PC->GetPlayerState<ARobotRallyPlayerState>();
        if (!PS || !PS->ControlledRobot) continue;

        // Convert register slots to actual cards
        PS->CommittedProgram.Empty();
        for (int32 SlotIndex : PS->RegisterSlots)
        {
            if (SlotIndex >= 0 && SlotIndex < PS->HandCards.Num())
            {
                PS->CommittedProgram.Add(PS->HandCards[SlotIndex]);
            }
        }

        // Clear registers for next round
        PS->RegisterSlots.Init(-1, 5);
        PS->bIsReady = false;
    }

    // Clear ready list
    ARobotRallyGameState* GS = GetRobotRallyGameState();
    if (GS)
    {
        GS->ReadyPlayers.Empty();
    }
}
```

**4.2 Movement Replication**

Robot movements are already replicated via `SetReplicateMovement(true)` in ARobotPawn constructor. UE5 automatically replicates:
- Location
- Rotation
- Velocity

URobotMovementComponent updates ARobotPawn's location locally on server, and UE5 replicates it to clients.

**No code changes needed** — built-in movement replication handles this!

**Files Modified:**
- `RobotRallyGameMode.cpp`

---

### Phase 5: Session Management (Lobby & Join)

**Goal:** Allow players to create/join game sessions.

**5.1 Update Build Configuration**

**File:** `RobotRally.Build.cs`

```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core",
    "CoreUObject",
    "Engine",
    "InputCore",
    "EnhancedInput",
    "OnlineSubsystem",       // NEW
    "OnlineSubsystemUtils"   // NEW
});
```

**5.2 Enable OnlineSubsystem in Config**

**File:** `Config/DefaultEngine.ini`

```ini
[OnlineSubsystem]
DefaultPlatformService=Null

[OnlineSubsystemNull]
bEnabled=true

[/Script/Engine.GameEngine]
+NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="OnlineSubsystemSteam.SteamNetDriver",DriverClassNameFallback="OnlineSubsystemUtils.IpNetDriver")
```

**5.3 Create Session Functions**

**File:** `RobotRallyGameMode.h`

```cpp
// Add to ARobotRallyGameMode class:

// Session management
UFUNCTION(BlueprintCallable, Category = "Game|Session")
void CreateSession(int32 MaxPlayers = 4);

UFUNCTION(BlueprintCallable, Category = "Game|Session")
void FindSessions();

UFUNCTION(BlueprintCallable, Category = "Game|Session")
void JoinSession(int32 SessionIndex);

private:
    // Session callbacks
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

    // Cached session search
    TSharedPtr<FOnlineSessionSearch> SessionSearch;
```

**File:** `RobotRallyGameMode.cpp`

```cpp
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

void ARobotRallyGameMode::CreateSession(int32 MaxPlayers)
{
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (!OnlineSub) return;

    IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
    if (!Sessions.IsValid()) return;

    // Setup session settings
    FOnlineSessionSettings SessionSettings;
    SessionSettings.NumPublicConnections = MaxPlayers;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bAllowJoinInProgress = false;
    SessionSettings.bIsLANMatch = true;  // Use LAN for now (Steam later)
    SessionSettings.bUsesPresence = false;
    SessionSettings.bAllowInvites = true;

    // Create session
    Sessions->OnCreateSessionCompleteDelegates.AddUObject(this, &ARobotRallyGameMode::OnCreateSessionComplete);
    Sessions->CreateSession(0, NAME_GameSession, SessionSettings);
}

void ARobotRallyGameMode::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("Session created successfully!"));

        // Load the game map as a listen server
        GetWorld()->ServerTravel("/Game/RobotRally/Maps/MainMap?listen");
    }
}

void ARobotRallyGameMode::FindSessions()
{
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (!OnlineSub) return;

    IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
    if (!Sessions.IsValid()) return;

    // Setup search parameters
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    SessionSearch->bIsLanQuery = true;
    SessionSearch->MaxSearchResults = 20;
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, false, EOnlineComparisonOp::Equals);

    // Start search
    Sessions->OnFindSessionsCompleteDelegates.AddUObject(this, &ARobotRallyGameMode::OnFindSessionsComplete);
    Sessions->FindSessions(0, SessionSearch.ToSharedRef());
}

void ARobotRallyGameMode::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (bWasSuccessful && SessionSearch->SearchResults.Num() > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Found %d sessions"), SessionSearch->SearchResults.Num());
        // Display results in UI (Phase 6)
    }
}

void ARobotRallyGameMode::JoinSession(int32 SessionIndex)
{
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (!OnlineSub || !SessionSearch.IsValid()) return;

    IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
    if (!Sessions.IsValid() || SessionIndex >= SessionSearch->SearchResults.Num()) return;

    // Join the selected session
    Sessions->OnJoinSessionCompleteDelegates.AddUObject(this, &ARobotRallyGameMode::OnJoinSessionComplete);
    Sessions->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[SessionIndex]);
}

void ARobotRallyGameMode::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (Result == EOnJoinSessionCompleteResult::Success)
    {
        IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
        IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

        // Get connection string
        FString ConnectionInfo;
        if (Sessions->GetResolvedConnectString(SessionName, ConnectionInfo))
        {
            // Travel to server
            APlayerController* PC = GetWorld()->GetFirstPlayerController();
            if (PC)
            {
                PC->ClientTravel(ConnectionInfo, ETravelType::TRAVEL_Absolute);
            }
        }
    }
}
```

**Files Modified:**
- `RobotRally.Build.cs`
- `Config/DefaultEngine.ini`
- `RobotRallyGameMode.h/cpp`

---

### Phase 6: UI & Polish

**Goal:** Create lobby UI for session creation/joining, player list display.

**6.1 Create Lobby Widget (Blueprint)**

**File:** `/Content/RobotRally/UI/WBP_Lobby.uasset` (Blueprint)

**Structure:**
- **Create Server** button → Calls `GameMode->CreateSession(4)`
- **Find Games** button → Calls `GameMode->FindSessions()`
- **Session List** (ScrollBox with session entries)
- **Join** button → Calls `GameMode->JoinSession(Index)`

**6.2 Create In-Game Player List Widget**

**File:** `/Content/RobotRally/UI/WBP_PlayerList.uasset` (Blueprint)

**Structure:**
- Loop through `GameState->PlayerArray`
- Display player name, robot color, ready status
- Show "Waiting for players..." when not all ready

**6.3 Add Network Role Display (Debug)**

Add text render to HUD showing:
- `GetLocalRole()` (ROLE_Authority vs ROLE_SimulatedProxy)
- Player count
- Current game state

**Files Created:**
- `WBP_Lobby.uasset` (Blueprint)
- `WBP_PlayerList.uasset` (Blueprint)
- Update `RobotRallyHUD.cpp` with network debug info

---

## Critical Files Summary

| File | Purpose | Phase | Type |
|------|---------|-------|------|
| `RobotRallyGameState.h/cpp` | Replicated game state | 1 | NEW |
| `RobotRallyPlayerState.h/cpp` | Per-player replicated state | 1 | NEW |
| `RobotRallyGameMode.h/cpp` | Session management, execution | 1, 4, 5 | MODIFY |
| `RobotPawn.h/cpp` | Enable replication | 2 | MODIFY |
| `GridManager.h/cpp` | Enable grid replication | 2 | MODIFY |
| `RobotController.h/cpp` | Card selection RPCs | 3 | MODIFY |
| `RobotRally.Build.cs` | Add OnlineSubsystem | 5 | MODIFY |
| `DefaultEngine.ini` | OnlineSubsystem config | 5 | MODIFY |
| `WBP_Lobby.uasset` | Lobby UI | 6 | NEW (Blueprint) |
| `WBP_PlayerList.uasset` | In-game player list | 6 | NEW (Blueprint) |

---

## Testing Plan

### Test 1: Local Multiplayer (PIE with 2 Clients)

**Setup:**
1. Editor Settings → Play → Number of Players: 2
2. Net Mode: Play As Listen Server
3. Start PIE

**Expected:**
- 2 windows open (1 server/client, 1 pure client)
- Both players see the same grid
- Both players have separate hands (9 cards each)
- Cards selected by one player appear in their registers only
- When both players fill 5 registers → execution starts automatically
- Both clients see robots move in sync

**Pass Criteria:** ✅ All robots move together, no desync

---

### Test 2: Card Selection Authority

**Setup:**
1. Start PIE with 2 players
2. Player 1: Select card #1 twice in a row
3. Player 2: Select 10 cards rapidly (spam key)

**Expected:**
- Server rejects duplicate card selection
- Server prevents more than 5 cards in registers
- Client receives error message
- No crashes

**Pass Criteria:** ✅ Server validates all actions, prevents exploits

---

### Test 3: Late Join (Join in Progress)

**Setup:**
1. Start dedicated server: `UE5Editor.exe RobotRally.uproject -server -log`
2. Client 1 connects
3. Client 1 programs 5 cards and commits
4. Client 2 connects during execution phase

**Expected:**
- Client 2 receives current game state via replication
- Client 2 sees robots mid-execution
- After execution ends, Client 2 can program cards normally

**Pass Criteria:** ✅ Late joiners sync correctly

---

### Test 4: Session Creation & Join

**Setup:**
1. Player 1: Open game → Create Session (4 players)
2. Player 2: Open game → Find Sessions → Join session
3. Player 3: Repeat step 2

**Expected:**
- Player 1 creates listen server
- Players 2-3 see session in list
- Players 2-3 successfully connect
- Lobby shows "3/4 players"

**Pass Criteria:** ✅ Session browser works, players connect

---

### Test 5: Network Lag Simulation

**Setup:**
1. Start PIE with 2 players
2. Enable network emulation: `Net PktLag=200` (200ms latency)
3. Both players program cards
4. Execute

**Expected:**
- Card selection feels slightly delayed but works
- Execution is smooth (server-authoritative, no interpolation issues)
- No rubber-banding during movement

**Pass Criteria:** ✅ Game playable with 200ms lag

---

## Implementation Checklist

### Phase 1: Foundation
- [ ] Create `RobotRallyGameState` class
- [ ] Create `RobotRallyPlayerState` class
- [ ] Update `GameMode` to use new GameState/PlayerState classes
- [ ] Test: PIE with 2 clients, verify GameState replicates

### Phase 2: Replication
- [ ] Add replication to `ARobotPawn` (health, grid position)
- [ ] Add replication to `AGridManager` (GridMap)
- [ ] Test: Spawn robot on server, verify clients see it

### Phase 3: Card Selection
- [ ] Implement `ServerSelectCard` RPC in `RobotController`
- [ ] Implement `ServerUndoSelection` RPC
- [ ] Implement `ServerCommitProgram` RPC
- [ ] Add validation logic (prevent cheating)
- [ ] Test: Card selection on client → validated on server → replicates state

### Phase 4: Execution
- [ ] Update `StartExecutionPhase` to use PlayerStates
- [ ] Implement `CommitProgramsFromPlayerStates`
- [ ] Verify movement replication (built-in UE5 system)
- [ ] Test: Both players execute cards, movements sync

### Phase 5: Sessions
- [ ] Add `OnlineSubsystem` to Build.cs
- [ ] Configure `DefaultEngine.ini` for Null subsystem
- [ ] Implement `CreateSession`, `FindSessions`, `JoinSession`
- [ ] Test: Create session, find session, join session

### Phase 6: UI
- [ ] Create `WBP_Lobby` Blueprint widget
- [ ] Create `WBP_PlayerList` Blueprint widget
- [ ] Add network debug overlay to HUD
- [ ] Test: Full flow (lobby → session → gameplay)

### Verification
- [ ] Run all 5 test scenarios
- [ ] Ensure no compiler warnings
- [ ] Test on LAN with 2 physical machines
- [ ] Profile network traffic (should be < 5KB/s per client)

---

## Future Enhancements (Out of Scope)

### Short-term (Next Phase)
- **Steam Integration:** Replace Null OSS with Steam for matchmaking
- **Spectator Mode:** Allow players to watch ongoing games
- **Reconnect Support:** Handle client disconnects gracefully

### Long-term
- **Dedicated Server Builds:** Headless server executable
- **Anti-Cheat:** Encrypt card hands, prevent memory hacking
- **Replays:** Record and replay matches
- **Tournament Mode:** Bracket system, ELO ranking

---

## Known Limitations

1. **No Rollback:** Clients see movements slightly delayed (not an issue for turn-based game)
2. **LAN Only Initially:** Requires port forwarding for internet play (Steam integration solves this)
3. **No Host Migration:** If listen server (host) quits, game ends
4. **Max 8 Players:** TArray-based, not designed for MMO scale (fine for board game)

---

## References

- [UE5 Networking Docs](https://docs.unrealengine.com/5.0/en-US/networking-overview-for-unreal-engine/)
- [Online Subsystem Overview](https://docs.unrealengine.com/5.0/en-US/online-subsystem-in-unreal-engine/)
- [Replication Graph](https://docs.unrealengine.com/5.0/en-US/replication-graph-in-unreal-engine/) (advanced optimization)

---

**Complexity:** Complex
**Estimated Scope:** Large feature (6 phases, ~15 files modified/created)
**Dependencies:** None (standalone feature)
**Risk:** Medium (networking bugs can be hard to debug)
