# Phase 5 UI Implementation Plan - UMG Widget System

## Context

### Why This Change?
The current HUD uses pure Canvas API (DrawHUD with FCanvasTextItem/FCanvasTileItem). While functional, Canvas API has limitations:
- No native support for animations or visual effects
- Harder to create complex layouts (card grids, register slots)
- Limited scalability across resolutions
- No built-in interaction support (mouse clicks, drag-drop)

This plan implements **UMG (Unreal Motion Graphics)** widgets to:
1. Enable rich card selection UI with animations
2. Support mouse interaction (optional, keeping keyboard)
3. Improve visual polish and scalability
4. Maintain network compatibility with existing multiplayer architecture

### Current Architecture (Verified)
- **HUD**: ARobotRallyHUD (Canvas drawing in DrawHUD())
- **Network Replication**:
  - PlayerState.Rep_HandCards (TArray<FRobotCard>, COND_OwnerOnly, BlueprintReadOnly)
  - PlayerState.Rep_RegisterSlots (TArray<int32>, COND_OwnerOnly, BlueprintReadOnly)
  - OnRep_HandCards() and OnRep_RegisterSlots() handlers exist
- **Input**: ARobotController with keyboard (1-9, E, Backspace) + RPCs (ServerSelectCard, ServerUndoSelection, ServerCommitProgram)
- **Module Dependencies**: Core, CoreUObject, Engine, InputCore, EnhancedInput, AIModule, OnlineSubsystem, OnlineSubsystemUtils
  - **Missing**: UMG, Slate, SlateCore (need to add)

---

## Implementation Approach: Hybrid Incremental Migration

**Strategy**: Start with Canvas HUD, incrementally add UMG widgets, remove Canvas code as widgets stabilize.

**Why Hybrid?**
- ✅ Low risk: Canvas HUD remains functional during development
- ✅ Incremental testing: Add widgets one at a time
- ✅ Rollback safety: Can revert to Canvas if issues arise
- ✅ Network compatibility: Existing PlayerState replication works unchanged

**Migration Path**:
1. Phase 1-4: Add UMG for cards (keep Canvas health/events)
2. Phase 5-6: Add UMG for health/lives/checkpoints/events (remove Canvas code)
3. Phase 7-8: Polish with animations and optional mouse support

---

## Widget Architecture

### C++ Base Classes (for network data binding)

Create in `Source/RobotRally/UI/`:

1. **UCardWidget** - Single card display
   - Properties: FRobotCard CardData, int32 HandIndex, ECardWidgetState CurrentState
   - Methods: SetCardData(), SetCardState(), OnCardClicked()
   - Blueprint event: OnCardDataChanged() for visual updates

2. **UProgrammingDeckWidget** - Hand + Register container
   - Properties: TArray<FRobotCard> HandCards, TArray<int32> RegisterSlots
   - Methods: UpdateHandCards(), UpdateRegisterSlots()
   - Blueprint events: OnHandCardsChanged(), OnRegisterSlotsChanged()
   - Child widgets: UniformGridPanel (hand), HorizontalBox (registers)

3. **URobotRallyMainWidget** - Root HUD container
   - Methods: UpdateHealth(), UpdateLives(), UpdateCheckpoints(), UpdateGameState(), AddEventMessage()
   - Child widgets: HealthBar, ProgrammingDeck, EventLog, etc.

### Blueprint Child Classes

Create in `Content/RobotRally/UI/Widgets/`:

- **WBP_CardSlot** (child of UCardWidget)
  - Layout: Overlay with background image, icon, action name text, priority badge
  - Visual states: Default, Hover, Selected, InRegister, Disabled

- **WBP_ProgrammingDeck** (child of UProgrammingDeckWidget)
  - Layout: VerticalBox → Hand grid (3x3) + Registers box (5 slots) + Action buttons
  - Anchored bottom-center, visible in Programming state

- **WBP_MainHUD** (child of URobotRallyMainWidget)
  - Layout: Canvas Panel with anchored child widgets (health top-left, deck bottom-center, etc.)

### Network Data Flow (No Changes Needed)

**Standalone Mode:**
```
GameMode.RobotPrograms[0] → HUD.UpdateWidgetData() → MainWidget.UpdateHandCards()
```

**Network Mode:**
```
Server: GameMode.SelectCardFromHand() → SyncPlayerStateHand() → PlayerState.Rep_HandCards (replicate)
Client: OnRep_HandCards() → HUD.UpdateWidgetData() → MainWidget.UpdateHandCards()
```

**Key Insight**: HUD already reads from correct sources (PlayerState in network, GameMode in standalone). Widgets just need to consume this data.

---

## Implementation Phases

### Phase 1: Foundation & Module Setup
**Complexity: Simple**

**Tasks:**
1. Add module dependencies to `Source/RobotRally/RobotRally.Build.cs`:
   ```csharp
   PublicDependencyModuleNames.AddRange(new string[] {
       "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
       "AIModule", "OnlineSubsystem", "OnlineSubsystemUtils",
       "UMG", "Slate", "SlateCore"  // NEW
   });
   ```

2. Create folder structure:
   - `Source/RobotRally/UI/` (C++ widget classes)
   - `Content/RobotRally/UI/Widgets/` (Blueprint widgets)
   - `Content/RobotRally/UI/Textures/` (card icons, UI assets)

3. Create C++ base classes (headers + empty implementations):
   - `UI/CardWidget.h/.cpp`
   - `UI/ProgrammingDeckWidget.h/.cpp`
   - `UI/RobotRallyMainWidget.h/.cpp`

4. Create enum `ECardWidgetState` in CardWidget.h:
   ```cpp
   UENUM(BlueprintType)
   enum class ECardWidgetState : uint8
   {
       Default,
       Hover,
       Selected,
       InRegister,
       Disabled
   };
   ```

5. Compile and verify no build errors

**Success Criteria:**
- ✅ Project compiles with UMG module
- ✅ C++ widget classes exist and compile
- ✅ No runtime errors in editor

**Critical Files:**
- `Source/RobotRally/RobotRally.Build.cs` (add UMG dependencies)
- `Source/RobotRally/UI/CardWidget.h/.cpp` (create)
- `Source/RobotRally/UI/ProgrammingDeckWidget.h/.cpp` (create)
- `Source/RobotRally/UI/RobotRallyMainWidget.h/.cpp` (create)

---

### Phase 2: Card Widget Implementation
**Complexity: Medium**

**Tasks:**
1. Implement `UCardWidget` C++ class:
   ```cpp
   class UCardWidget : public UUserWidget
   {
       UPROPERTY(BlueprintReadOnly, Category = "Card")
       FRobotCard CardData;

       UPROPERTY(BlueprintReadOnly, Category = "Card")
       int32 HandIndex = -1;

       UPROPERTY(BlueprintReadOnly, Category = "Card")
       ECardWidgetState CurrentState;

       UFUNCTION(BlueprintCallable)
       void SetCardData(const FRobotCard& InCard, int32 InHandIndex);

       UFUNCTION(BlueprintImplementableEvent)
       void OnCardDataChanged();

       UFUNCTION(BlueprintCallable)
       void SetCardState(ECardWidgetState NewState);
   };
   ```

2. Create **WBP_CardSlot** Blueprint (child of UCardWidget):
   - Root: Overlay (120x160px size box)
   - Background: Image (colored by card type)
   - Icon: Image (arrow/rotation symbol based on ECardAction)
   - Action text: TextBlock (e.g., "Move 2")
   - Priority badge: Border (top-right) with TextBlock (e.g., "420")

3. Create card icon textures (7 types):
   - Move1/2/3: Forward arrows (1/2/3 arrows)
   - MoveBack: Backward arrow
   - RotateLeft/Right: Curved arrows
   - UTurn: U-shaped arrow
   - Size: 64x64px, white (tinted by material)

4. Implement Blueprint logic in WBP_CardSlot:
   - Bind OnCardDataChanged() to update icon/text/priority
   - Implement visual states (border color, opacity, scale)
   - Default: white border, 100% opacity
   - Hover: yellow glow, scale 1.05x
   - Selected: thick yellow border
   - InRegister: gray background, locked icon overlay
   - Disabled: 50% opacity, desaturated

**Success Criteria:**
- ✅ All 7 card types display correctly
- ✅ Priority badge shows correct number
- ✅ Card states visually distinct
- ✅ Readable at 1920x1080 and 1280x720

**Critical Files:**
- `Source/RobotRally/UI/CardWidget.cpp` (implement SetCardData logic)
- `Content/RobotRally/UI/Widgets/WBP_CardSlot.uasset` (create Blueprint)
- `Content/RobotRally/UI/Textures/` (create card icons)

---

### Phase 3: Programming Deck Layout
**Complexity: Medium**

**Tasks:**
1. Implement `UProgrammingDeckWidget` C++ class:
   ```cpp
   class UProgrammingDeckWidget : public UUserWidget
   {
       UPROPERTY(BlueprintReadOnly)
       TArray<FRobotCard> HandCards;

       UPROPERTY(BlueprintReadOnly)
       TArray<int32> RegisterSlots;

       UPROPERTY(meta = (BindWidget))
       UUniformGridPanel* HandGridPanel;

       UPROPERTY(meta = (BindWidget))
       UHorizontalBox* RegisterBox;

       UPROPERTY(EditDefaultsOnly)
       TSubclassOf<UCardWidget> CardWidgetClass;

       UFUNCTION(BlueprintCallable)
       void UpdateHandCards(const TArray<FRobotCard>& NewHandCards);

       UFUNCTION(BlueprintCallable)
       void UpdateRegisterSlots(const TArray<int32>& NewRegisterSlots);

   private:
       TArray<UCardWidget*> HandCardWidgets;
       TArray<UCardWidget*> RegisterCardWidgets;

       void RebuildHandWidgets();
       void RebuildRegisterWidgets();
   };
   ```

2. Create **WBP_ProgrammingDeck** Blueprint (child of UProgrammingDeckWidget):
   - Root: VerticalBox
   - Hand section:
     - TextBlock: "HAND"
     - UniformGridPanel (HandGridPanel): 3 rows x 3 cols
   - Spacer: 20px
   - Registers section:
     - TextBlock: "REGISTERS"
     - HorizontalBox (RegisterBox): 5 slots
   - Action buttons (optional for Phase 7):
     - HorizontalBox: Execute, Undo, Clear buttons

3. Implement C++ logic:
   - `UpdateHandCards()`: Create/reuse card widgets, call SetCardData(), populate grid
   - `UpdateRegisterSlots()`: Display cards in register slots, mark hand cards as "InRegister"
   - Widget pooling: Reuse existing widgets instead of destroy/create

4. Create **WBP_MainHUD** Blueprint (child of URobotRallyMainWidget):
   - Root: Canvas Panel
   - Add ProgrammingDeck widget (bottom-center anchor, 800x400px)

5. Integrate with `ARobotRallyHUD`:
   - Add member: `URobotRallyMainWidget* MainWidget`
   - Add property: `TSubclassOf<URobotRallyMainWidget> MainWidgetClass`
   - In `BeginPlay()`: Create MainWidget, AddToViewport()
   - In `DrawHUD()`: Call `UpdateWidgetData()`

**Success Criteria:**
- ✅ Programming deck displays at bottom of screen
- ✅ Hand shows 9 cards (or fewer based on damage)
- ✅ Registers show 5 slots with correct cards
- ✅ Widgets read from PlayerState (network) or GameMode (standalone)

**Critical Files:**
- `Source/RobotRally/UI/ProgrammingDeckWidget.cpp` (implement update logic)
- `Source/RobotRally/UI/RobotRallyMainWidget.h/.cpp` (create root widget)
- `Source/RobotRally/RobotRallyHUD.h/.cpp` (integrate widget creation)
- `Content/RobotRally/UI/Widgets/WBP_ProgrammingDeck.uasset` (create)
- `Content/RobotRally/UI/Widgets/WBP_MainHUD.uasset` (create)

---

### Phase 4: Keyboard Input Integration
**Complexity: Simple**

**Tasks:**
1. Modify `ARobotRallyHUD::DrawHUD()`:
   ```cpp
   void ARobotRallyHUD::DrawHUD()
   {
       Super::DrawHUD();

       if (!MainWidget && MainWidgetClass)
       {
           MainWidget = CreateWidget<URobotRallyMainWidget>(GetOwningPlayerController(), MainWidgetClass);
           MainWidget->AddToViewport();
       }

       UpdateWidgetData();  // Poll network data each frame

       // Keep Canvas for debug overlay (optional)
       if (bShowDebugOverlay)
       {
           DrawNetworkDebug();
       }
   }
   ```

2. Implement `ARobotRallyHUD::UpdateWidgetData()`:
   ```cpp
   void ARobotRallyHUD::UpdateWidgetData()
   {
       if (!MainWidget) return;

       bool bIsNetwork = (GetWorld()->GetNetMode() != NM_Standalone);

       if (bIsNetwork)
       {
           ARobotRallyPlayerState* PS = GetLocalPlayerState();
           if (PS && MainWidget->ProgrammingDeck)
           {
               MainWidget->ProgrammingDeck->UpdateHandCards(PS->Rep_HandCards);
               MainWidget->ProgrammingDeck->UpdateRegisterSlots(PS->Rep_RegisterSlots);
           }
       }
       else
       {
           ARobotRallyGameMode* GM = GetWorld()->GetAuthGameMode<ARobotRallyGameMode>();
           if (GM && GM->RobotPrograms.Num() > 0)
           {
               FRobotProgram& PlayerProgram = GM->RobotPrograms[0];
               MainWidget->ProgrammingDeck->UpdateHandCards(PlayerProgram.HandCards);
               MainWidget->ProgrammingDeck->UpdateRegisterSlots(PlayerProgram.RegisterSlots);
           }
       }
   }
   ```

3. Test keyboard input (no code changes needed - already works):
   - Press 1-9 → ARobotController::OnSelectCard() → ServerSelectCard RPC → GameMode updates → PlayerState replicates → HUD reads → Widget updates
   - Verify visual updates match input

**Success Criteria:**
- ✅ Pressing 1-9 updates widget display
- ✅ Backspace undo works visually
- ✅ E key shows/hides deck based on game state
- ✅ Network mode: client sees updates from server
- ✅ Standalone mode: immediate visual update

**Critical Files:**
- `Source/RobotRally/RobotRallyHUD.cpp` (implement UpdateWidgetData)

---

### Phase 5: HUD Elements Migration
**Complexity: Medium**

**Tasks:**
1. Create **WBP_HealthDisplay**:
   - Layout: HorizontalBox → ProgressBar + TextBlock
   - Bind ProgressBar.Percent to Health/MaxHealth
   - Bind TextBlock.Text to formatted string "7/10"
   - Color interpolation: green (>70%) → yellow (30-70%) → red (<30%)

2. Create **WBP_LivesDisplay**:
   - Layout: HorizontalBox with 3 Image widgets (heart icons)
   - Show filled hearts = Lives, empty hearts = MaxLives - Lives

3. Create **WBP_CheckpointProgress**:
   - Layout: HorizontalBox with flag icons (dynamically created)
   - Filled flags = collected checkpoints, outline flags = remaining

4. Create **WBP_GameStatePanel**:
   - Layout: Overlay → Background + Icon + TextBlock
   - Background color: Blue (Programming), Yellow (Executing), Green (Victory), Red (GameOver)
   - Text: "PROGRAMMING" / "EXECUTING" / "VICTORY" / "GAME OVER"

5. Add to **WBP_MainHUD**:
   - Top-left anchor: WBP_HealthDisplay, WBP_LivesDisplay, WBP_CheckpointProgress
   - Top-center anchor: WBP_GameStatePanel

6. Implement `URobotRallyMainWidget` update methods:
   ```cpp
   void URobotRallyMainWidget::UpdateHealth(int32 Current, int32 Max);
   void URobotRallyMainWidget::UpdateLives(int32 Remaining);
   void URobotRallyMainWidget::UpdateCheckpoints(int32 Current, int32 Total);
   void URobotRallyMainWidget::UpdateGameState(EGameState NewState);
   ```

7. Call update methods from `ARobotRallyHUD::UpdateWidgetData()`

8. Remove Canvas drawing code:
   - Comment out health bar drawing in `DrawHUD()`
   - Comment out checkpoint/lives text
   - Keep event log temporarily (migrate in Phase 6)

**Success Criteria:**
- ✅ Health bar updates in real-time
- ✅ Lives counter shows hearts
- ✅ Checkpoint progress shows flags
- ✅ Game state panel shows current phase
- ✅ 60 FPS maintained

**Critical Files:**
- `Content/RobotRally/UI/Widgets/WBP_HealthDisplay.uasset` (create)
- `Content/RobotRally/UI/Widgets/WBP_GameStatePanel.uasset` (create)
- `Source/RobotRally/UI/RobotRallyMainWidget.cpp` (implement update methods)
- `Source/RobotRally/RobotRallyHUD.cpp` (remove Canvas code)

---

### Phase 6: Event Log Widget
**Complexity: Medium**

**Tasks:**
1. Create **WBP_EventMessage**:
   - Layout: HorizontalBox → Icon + TextBlock
   - UMG Animation: FadeIn (0.3s) → Hold (5s) → FadeOut (0.3s)
   - Icon based on message type (checkmark, warning, damage, info)
   - Color-coded text (green, yellow, red, white)

2. Create **WBP_EventLog**:
   - Layout: ScrollBox → VerticalBox
   - Auto-scroll to bottom when new message added
   - Semi-transparent background
   - Anchored bottom-left

3. Implement `URobotRallyMainWidget::AddEventMessage()`:
   ```cpp
   void URobotRallyMainWidget::AddEventMessage(const FString& Text, FColor Color)
   {
       UUserWidget* MessageWidget = CreateWidget(GetWorld(), EventMessageWidgetClass);
       EventLogBox->AddChild(MessageWidget);
       // Call Blueprint function to set text/color
       // Play fade animation
       // Remove widget after animation completes (timer)
   }
   ```

4. Replace `ARobotRallyHUD::AddEventMessage()`:
   - Forward call to MainWidget->AddEventMessage()

5. Remove Canvas event log drawing code

**Success Criteria:**
- ✅ Event messages appear with fade in
- ✅ Messages fade out after 5 seconds
- ✅ Log scrollable if many messages
- ✅ Color-coded and icon-based messages

**Critical Files:**
- `Content/RobotRally/UI/Widgets/WBP_EventMessage.uasset` (create)
- `Content/RobotRally/UI/Widgets/WBP_EventLog.uasset` (create)
- `Source/RobotRally/UI/RobotRallyMainWidget.cpp` (implement AddEventMessage)
- `Source/RobotRally/RobotRallyHUD.cpp` (forward to widget)

---

### Phase 7: Mouse Support (Optional)
**Complexity: Medium**

**Tasks:**
1. Enable mouse cursor in `ARobotController`:
   ```cpp
   void ARobotController::BeginPlay()
   {
       Super::BeginPlay();
       UpdateMouseCursor();  // Call on game state change too
   }

   void ARobotController::UpdateMouseCursor()
   {
       ARobotRallyGameMode* GM = GetWorld()->GetAuthGameMode<ARobotRallyGameMode>();
       bool bShowCursor = (GM && GM->CurrentState == EGameState::Programming);
       bShowMouseCursor = bShowCursor;
       SetInputMode(bShowCursor ? FInputModeGameAndUI() : FInputModeGameOnly());
   }
   ```

2. Implement `UCardWidget::OnCardClicked()`:
   ```cpp
   void UCardWidget::OnCardClicked()
   {
       UProgrammingDeckWidget* DeckWidget = Cast<UProgrammingDeckWidget>(GetParent());
       if (DeckWidget)
       {
           DeckWidget->OnCardSelected(HandIndex);
       }
   }
   ```

3. Implement `UProgrammingDeckWidget::OnCardSelected()`:
   ```cpp
   void UProgrammingDeckWidget::OnCardSelected(int32 HandIndex)
   {
       APlayerController* PC = GetOwningPlayer();
       ARobotController* RC = Cast<ARobotController>(PC);
       if (RC)
       {
           RC->SelectCard(HandIndex);  // Reuses existing RPC logic!
       }
   }
   ```

4. Add Button widgets to WBP_ProgrammingDeck:
   - Execute button → calls RobotController->OnExecuteProgram()
   - Undo button → calls RobotController->OnUndoSelection()
   - Clear button → calls ServerUndoSelection() 5 times

5. Bind Button::OnClicked events in Blueprint

**Success Criteria:**
- ✅ Can select cards by clicking
- ✅ Can click Execute/Undo buttons
- ✅ Keyboard shortcuts still work
- ✅ Mouse cursor hides during Executing phase
- ✅ Network: mouse clicks trigger same RPCs

**Critical Files:**
- `Source/RobotRally/RobotController.cpp` (add UpdateMouseCursor)
- `Source/RobotRally/UI/CardWidget.cpp` (implement OnCardClicked)
- `Source/RobotRally/UI/ProgrammingDeckWidget.cpp` (implement OnCardSelected)
- `Content/RobotRally/UI/Widgets/WBP_ProgrammingDeck.uasset` (add buttons)

---

### Phase 8: Animations & Polish
**Complexity: Medium**

**Tasks:**
1. Card selection animation:
   - UMG Animation in WBP_CardSlot: slide from hand position to register position (0.3s ease-out)
   - Trigger when SetCardState(InRegister) called

2. Card undo animation:
   - Reverse animation (0.3s ease-out)
   - Trigger when SetCardState(Default) called

3. Health bar damage flash:
   - UMG Animation: flash red (0.2s), then smooth transition
   - Trigger in UpdateHealth() when health decreases

4. Checkpoint collection animation:
   - UMG Animation: flag scale up + glow (0.5s)
   - Trigger in UpdateCheckpoints() when count increases

5. Deck show/hide animation:
   - UMG Animation: scale down + fade out (0.4s) for Executing state
   - Scale up + fade in for Programming state

6. Register execution highlight:
   - Add UpdateCurrentRegister(int32 Index) to MainWidget
   - Highlight executing register with pulsing glow
   - Call from GameMode during execution phase

**Success Criteria:**
- ✅ All animations smooth at 60 FPS
- ✅ Card selection feels responsive (<0.3s)
- ✅ Visual feedback clear for all actions
- ✅ No animation glitches

**Critical Files:**
- `Content/RobotRally/UI/Widgets/WBP_CardSlot.uasset` (add animations)
- `Content/RobotRally/UI/Widgets/WBP_HealthDisplay.uasset` (add flash animation)
- `Source/RobotRally/UI/RobotRallyMainWidget.cpp` (trigger animations from C++)

---

### Phase 9: Testing & Optimization
**Complexity: Simple**

**Testing Checklist:**
- [ ] All 7 card types display correctly
- [ ] Hand size scales with damage (9 → 5)
- [ ] Register programming works (keyboard + mouse)
- [ ] Undo works (keyboard + mouse)
- [ ] Execute works (keyboard + mouse)
- [ ] Health/Lives/Checkpoint widgets update correctly
- [ ] Event log shows messages with fading
- [ ] UI hides/shows in correct game states
- [ ] Network: multiple clients see correct data
- [ ] Readable at 1280x720, 1920x1080, 2560x1440

**Performance:**
- Profile with `stat UMG` command
- Target: <2ms frame time for UI
- Disable Tick on all widgets (use manual updates)
- Use Retainer Boxes for static elements
- Pool card widgets (reuse, don't recreate)

**Critical Files:**
- All widget Blueprints (optimize settings)

---

## Verification & Testing

### Manual Testing Workflow:
1. **Standalone Mode**:
   - Start PIE, verify widgets display
   - Select 5 cards with 1-9 keys
   - Press E to execute
   - Verify health/lives/checkpoints update
   - Take damage, verify health bar changes
   - Collect checkpoint, verify flag animation

2. **Network Mode**:
   - Start as Listen Server (2+ players)
   - Each client: select cards, verify replication
   - Execute program, verify all robots move
   - Verify event log shows on all clients
   - Check performance with `stat UMG`

3. **Resolution Testing**:
   - Test at 1280x720, 1920x1080, 2560x1440
   - Verify text readability
   - Check widget anchoring (no clipping)

### Performance Testing:
- Run `stat UMG` in console
- Target: UI < 2ms per frame
- Check memory with `stat Memory`
- Verify no memory leaks over 30 min session

---

## Critical Files Summary

**Files to Modify:**
- `Source/RobotRally/RobotRally.Build.cs` - Add UMG/Slate/SlateCore
- `Source/RobotRally/RobotRallyHUD.h/.cpp` - Integrate widgets, remove Canvas code
- `Source/RobotRally/RobotController.cpp` - Add mouse cursor handling (Phase 7)

**Files to Create:**
- `Source/RobotRally/UI/CardWidget.h/.cpp`
- `Source/RobotRally/UI/ProgrammingDeckWidget.h/.cpp`
- `Source/RobotRally/UI/RobotRallyMainWidget.h/.cpp`
- `Content/RobotRally/UI/Widgets/WBP_CardSlot.uasset`
- `Content/RobotRally/UI/Widgets/WBP_ProgrammingDeck.uasset`
- `Content/RobotRally/UI/Widgets/WBP_MainHUD.uasset`
- `Content/RobotRally/UI/Widgets/WBP_HealthDisplay.uasset`
- `Content/RobotRally/UI/Widgets/WBP_GameStatePanel.uasset`
- `Content/RobotRally/UI/Widgets/WBP_EventMessage.uasset`
- `Content/RobotRally/UI/Widgets/WBP_EventLog.uasset`

**Files Read (No Changes):**
- `Source/RobotRally/RobotRallyPlayerState.h/.cpp` - Already has Rep_HandCards/Rep_RegisterSlots
- `Source/RobotRally/RobotRallyGameMode.h/.cpp` - Data source for standalone mode
- `Config/DefaultInput.ini` - Keyboard mappings already configured

---

## Risk Mitigation

| Risk | Mitigation |
|------|-----------|
| **Performance degradation** | Profile early (Phase 9), use Retainer Boxes, disable Tick |
| **Network desync** | Use OnRep_ functions to trigger updates, poll in HUD as backup |
| **Resolution scaling issues** | Use anchors properly, test at multiple resolutions in Phase 5 |
| **Animation lag** | Keep animations <0.5s, allow skipping with rapid input |
| **Memory leaks** | Pool widgets instead of destroy/create, profile memory usage |

---

## Estimated Complexity

- **Phase 1**: Simple (1 day)
- **Phase 2**: Medium (2-3 days)
- **Phase 3**: Medium (2-3 days)
- **Phase 4**: Simple (1 day)
- **Phase 5**: Medium (2 days)
- **Phase 6**: Medium (1-2 days)
- **Phase 7**: Medium (2 days, optional)
- **Phase 8**: Medium (2-3 days)
- **Phase 9**: Simple (1 day)

**Total**: 2-3 weeks (with optional Phase 7)
