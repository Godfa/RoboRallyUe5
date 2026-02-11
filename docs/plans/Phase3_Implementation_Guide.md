# Phase 3 Implementation Guide - Programming Deck Layout

**Status**: Partially Complete (C++ Done, Blueprint Pending)
**Complexity**: Medium
**Prerequisites**: Phase 1 & 2 Complete

---

## Overview

Phase 3 creates the programming deck layout that displays the player's hand of cards and the 5 register slots. The C++ logic for managing card widgets and updating from network data is complete. Blueprint widgets need to be created in the Unreal Editor.

## Part 1: C++ Implementation (✅ COMPLETE)

### ProgrammingDeckWidget Logic

**Implemented Methods:**

1. ✅ **UpdateHandCards()** - Updates hand card data and rebuilds widgets
   - Accepts TArray<FRobotCard> from PlayerState or GameMode
   - Calls RebuildHandWidgets() to refresh display

2. ✅ **UpdateRegisterSlots()** - Updates register slot data and rebuilds widgets
   - Accepts TArray<int32> (hand indices, -1 = empty)
   - Calls RebuildRegisterWidgets() to refresh display

3. ✅ **RebuildHandWidgets()** - Widget pooling for hand cards
   - Clears HandGridPanel
   - Creates or reuses card widgets from pool
   - Sets card data for each card in hand
   - Marks cards as InRegister if they're in RegisterSlots
   - Adds widgets to 3x3 UniformGridPanel

4. ✅ **RebuildRegisterWidgets()** - Widget pooling for registers
   - Clears RegisterBox
   - Creates 5 register slot widgets
   - Shows card if register has one (HandIndex >= 0)
   - Hides widget if register is empty (HandIndex == -1)
   - Adds widgets to HorizontalBox

### RobotRallyHUD Integration

**Implemented:**

1. ✅ **BeginPlay()** - Creates MainWidget if MainWidgetClass is set
   - Creates widget using CreateWidget<URobotRallyMainWidget>()
   - Adds widget to viewport with AddToViewport()

2. ✅ **UpdateWidgetData()** - Polls data each frame and updates widgets
   - **Network Mode**: Reads from PlayerState (Rep_HandCards, Rep_RegisterSlots, Rep_Robot)
   - **Standalone Mode**: Reads from GameMode (RobotPrograms[0])
   - Updates programming deck with hand and register data
   - Updates health, lives, checkpoints from robot
   - Updates game state display

3. ✅ **DrawHUD()** - Calls UpdateWidgetData() each frame
   - Widget updates happen before Canvas drawing
   - Canvas HUD still renders for backward compatibility

4. ✅ **AddEventMessage()** - Forwards messages to widget
   - Existing Canvas message system still works
   - Also calls MainWidget->AddEventMessage() if available

---

## Part 2: Create Blueprint Widgets (⏳ PENDING)

### Step 1: Create WBP_ProgrammingDeck

1. Open Unreal Editor
2. Navigate to `Content/RobotRally/UI/Widgets/`
3. Right-click → **User Interface** → **Widget Blueprint**
4. Name: `WBP_ProgrammingDeck`
5. Open the Blueprint

#### Set Parent Class

1. Click **File** → **Reparent Blueprint**
2. Search for and select: `ProgrammingDeckWidget` (C++ class)
3. Save

#### Design Widget Layout

**Root Widget: Vertical Box**
- Add **Vertical Box** as root
- Settings:
  - Anchors: Bottom Center
  - Position: Bottom of screen, centered
  - Size: 800x400 (or auto-size to content)

**Child 1: Hand Section Header**
- Widget Type: **Text Block**
- Text: "HAND"
- Font Size: 18
- Font: Bold
- Justification: Center
- Color: Yellow

**Child 2: Hand Grid Panel** (IMPORTANT: Must be named `HandGridPanel`)
- Widget Type: **Uniform Grid Panel**
- Name: `HandGridPanel` ⚠️ **Must match C++ BindWidget!**
- Settings:
  - Slot Padding: 5px
  - Min Desired Width: 400
  - Min Desired Height: 200

**Child 3: Spacer**
- Widget Type: **Spacer**
- Size: Height 20px

**Child 4: Registers Section Header**
- Widget Type: **Text Block**
- Text: "REGISTERS"
- Font Size: 18
- Font: Bold
- Justification: Center
- Color: Yellow

**Child 5: Register Box** (IMPORTANT: Must be named `RegisterBox`)
- Widget Type: **Horizontal Box**
- Name: `RegisterBox` ⚠️ **Must match C++ BindWidget!**
- Settings:
  - Slot Padding: 10px
  - Horizontal Alignment: Center

**Child 6: Action Buttons (Optional - Phase 7)**
- Widget Type: **Horizontal Box**
- Contents: Execute, Undo, Clear buttons (implement in Phase 7)

#### Set CardWidgetClass Property

In the **Details** panel:
1. Find **Card Widget Class** property
2. Set to: `WBP_CardSlot` (created in Phase 2)
3. This tells the C++ code which widget class to instantiate for cards

#### Compile and Save

1. Click **Compile**
2. Save the Blueprint
3. Widget is now ready for integration

---

### Step 2: Create WBP_MainHUD

1. In `Content/RobotRally/UI/Widgets/`, create new Widget Blueprint
2. Name: `WBP_MainHUD`
3. Open the Blueprint

#### Set Parent Class

1. Click **File** → **Reparent Blueprint**
2. Select: `RobotRallyMainWidget` (C++ class)
3. Save

#### Design Widget Layout

**Root Widget: Canvas Panel**
- Add **Canvas Panel** as root

**Child 1: Programming Deck** (IMPORTANT: Must be named `ProgrammingDeck`)
- Widget Type: **WBP_ProgrammingDeck** (your Blueprint from Step 1)
- Name: `ProgrammingDeck` ⚠️ **Must match C++ BindWidget!**
- Anchors: Bottom Center (0.5, 1.0)
- Alignment: (0.5, 1.0)
- Position: X=0, Y=-10 (10px from bottom)
- Size: Auto (will size to content)

**Child 2: Health Display (Optional - Phase 5)**
- Add health bar widget (implement in Phase 5)
- Anchors: Top Left

**Child 3: Event Log (Optional - Phase 6)**
- Add event log widget (implement in Phase 6)
- Anchors: Bottom Left

#### Implement Blueprint Events (Optional)

The following events can be implemented in Blueprint if you want custom behavior:

1. **Event OnHealthChanged** - Called when health updates
2. **Event OnLivesChanged** - Called when lives update
3. **Event OnCheckpointsChanged** - Called when checkpoints update
4. **Event OnGameStateChanged** - Called when game state changes
5. **Event OnEventMessageAdded** - Called when event message is added

For Phase 3, these can be left unimplemented. The C++ code will handle the basic functionality.

#### Compile and Save

1. Click **Compile**
2. Save the Blueprint

---

### Step 3: Set MainWidgetClass in GameMode

Now we need to tell the HUD which widget to create.

**Option A: Set in GameMode Blueprint**

1. Open your GameMode Blueprint (or create one if it doesn't exist)
2. In **Class Defaults**, find **HUD Class**
3. Make sure it's set to `RobotRallyHUD`
4. Find **Default Pawn Class** and ensure robot is set
5. Save

**Option B: Set MainWidgetClass via Code (for testing)**

Add to `ARobotRallyGameMode::BeginPlay()`:

```cpp
void ARobotRallyGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Set HUD widget class
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        ARobotRallyHUD* HUD = Cast<ARobotRallyHUD>(PC->GetHUD());
        if (HUD && !HUD->MainWidgetClass)
        {
            // Find widget class by soft object path
            HUD->MainWidgetClass = LoadClass<URobotRallyMainWidget>(
                nullptr,
                TEXT("/Game/RobotRally/UI/Widgets/WBP_MainHUD.WBP_MainHUD_C")
            );
        }
    }

    // ... rest of BeginPlay
}
```

**Option C: Create Blueprint Child of RobotRallyHUD**

1. Right-click in Content Browser → **Blueprint Class**
2. Parent Class: Search for and select `RobotRallyHUD`
3. Name: `BP_RobotRallyHUD`
4. Open the Blueprint
5. In **Class Defaults** panel:
   - Find **Main Widget Class**
   - Set to: `WBP_MainHUD`
6. Save and Compile
7. In **Project Settings** → **Maps & Modes**:
   - Set **HUD Class** to `BP_RobotRallyHUD`

**Option C is recommended** as it's the most Blueprint-friendly approach.

---

## Part 3: Testing

### Test Checklist

1. **Start Play in Editor (PIE)**
   - Widget should appear at bottom of screen
   - No Blueprint compilation errors

2. **Verify Hand Display**
   - Hand section shows "HAND" header
   - 9 cards appear in 3x3 grid (or fewer based on damage)
   - Each card shows icon, action name, priority

3. **Verify Register Display**
   - Register section shows "REGISTERS" header
   - 5 empty slots visible (or hidden if empty)

4. **Test Card Selection (via keyboard)**
   - Press 1-9 to select cards
   - Selected card should appear in register
   - Card in hand should change state to InRegister (grayed out)
   - Register should display the card

5. **Test Undo**
   - Press Backspace
   - Card should return to hand
   - Card state should return to Default (white border)
   - Register slot should empty

6. **Test Multiple Cards**
   - Select 5 cards (fill all registers)
   - All cards should show in registers
   - All should be grayed in hand

7. **Test Network Mode** (if applicable)
   - Start as Listen Server with 2 players
   - Each client sees their own hand and registers
   - Cards update correctly when selected

### Common Issues

**Issue: Widget doesn't appear**
- Check that MainWidgetClass is set in HUD
- Verify BeginPlay() is creating widget
- Check AddToViewport() is called
- Look for Blueprint compilation errors

**Issue: Cards don't show up**
- Verify CardWidgetClass is set in WBP_ProgrammingDeck
- Check that WBP_CardSlot exists from Phase 2
- Ensure HandGridPanel and RegisterBox are named correctly

**Issue: Widget names mismatch error**
- C++ uses meta=(BindWidget) which requires exact naming
- HandGridPanel must be named exactly "HandGridPanel"
- RegisterBox must be named exactly "RegisterBox"
- ProgrammingDeck must be named exactly "ProgrammingDeck"

**Issue: Cards appear but don't update**
- Check UpdateWidgetData() is being called in DrawHUD()
- Verify data is coming from correct source (PlayerState vs GameMode)
- Add debug logs to UpdateHandCards() and UpdateRegisterSlots()

**Issue: Crashes on widget creation**
- Ensure parent classes are set correctly in Blueprints
- Check that all widget pointers are validated before use
- Verify CardWidgetClass is not null

---

## Part 4: Phase 3 Success Criteria

Phase 3 is **complete** when:

- [x] ✅ ProgrammingDeckWidget C++ logic implemented
- [x] ✅ RobotRallyHUD integration complete
- [x] ✅ UpdateWidgetData() polls from PlayerState/GameMode
- [x] ✅ Widget pooling prevents memory leaks
- [x] ✅ Project compiles with no errors
- [ ] WBP_ProgrammingDeck Blueprint created
- [ ] WBP_MainHUD Blueprint created
- [ ] MainWidgetClass set in HUD
- [ ] Programming deck displays at bottom of screen
- [ ] Hand shows 9 cards in 3x3 grid
- [ ] Registers show 5 slots
- [ ] Keyboard input (1-9, Backspace) updates widgets
- [ ] Network mode: clients see their own data
- [ ] Standalone mode: widgets update correctly

**Current Progress: 60% (C++ complete, Blueprint pending)**

---

## Next Steps (Phase 4)

Once Phase 3 is complete, Phase 4 will add keyboard input integration to ensure card selection triggers visual updates smoothly. The input system already works (from RobotController), Phase 4 just ensures widgets respond correctly.

---

**Document Version**: 1.0
**Last Updated**: 2026-02-11
**Author**: RobotRally Development Team
