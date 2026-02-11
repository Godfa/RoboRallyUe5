# Phase 2 Implementation Guide - Card Widget

**Status**: In Progress
**Complexity**: Medium
**Prerequisites**: Phase 1 Complete (C++ base classes compiled)

---

## Overview

Phase 2 creates the visual card widgets that will display Robot Rally cards with icons, actions, priorities, and visual states. This phase requires work in **both** the Unreal Editor and creating texture assets.

## Part 1: C++ Enhancement (✅ COMPLETE)

The C++ CardWidget class has been enhanced with helper methods:

- ✅ `GetCardActionName()` - Returns display text ("Move 1", "Rotate Right", etc.)
- ✅ `GetCardTypeColor()` - Returns background color based on card type
- ✅ `GetPriorityBadgeColor()` - Returns badge color based on priority value
- ✅ `GetCardIconName()` - Returns icon texture name for asset lookup

**Color Scheme Implemented:**
- **Movement Cards** (Move 1/2/3): Light Blue (#4A90E2)
- **Rotation Cards** (Rotate Left/Right): Green (#7ED321)
- **Special Cards** (U-Turn, Move Back): Orange (#F5A623)
- **Priority Badges**:
  - High (600+): Gold (#FFD700)
  - Medium (300-599): Silver (#C0C0C0)
  - Low (0-299): Bronze (#CD7F32)

---

## Part 2: Create Card Icon Textures

### Icon Specifications

Create 7 icon textures for card actions. Each icon should be:
- **Size**: 64x64 pixels (or larger power-of-2: 128x128, 256x256)
- **Format**: PNG with transparency
- **Color**: White (#FFFFFF) - will be tinted by material
- **Style**: Simple, bold lines for readability
- **Background**: Transparent

### Icon Designs

1. **T_Icon_Move1.png** - Single upward arrow (↑)
2. **T_Icon_Move2.png** - Double upward arrows (↑↑)
3. **T_Icon_Move3.png** - Triple upward arrows (↑↑↑)
4. **T_Icon_MoveBack.png** - Downward arrow (↓)
5. **T_Icon_RotateRight.png** - Clockwise curved arrow (↻)
6. **T_Icon_RotateLeft.png** - Counter-clockwise curved arrow (↺)
7. **T_Icon_UTurn.png** - U-shaped 180° arrow (⟲)

### Creating Icons

**Option A: Use Free Icon Libraries**
- [Font Awesome](https://fontawesome.com/) - arrow icons
- [Material Design Icons](https://materialdesignicons.com/) - navigation icons
- Export as PNG at 64x64 or 128x128

**Option B: Create in Graphics Software**
- Use Photoshop, GIMP, Inkscape, or Figma
- Simple vector shapes: arrows, curves
- Export as PNG with transparency

**Option C: Use Unicode Symbols (Quick Placeholder)**
- Create 64x64 PNG with white text on transparent background
- Use Unicode arrows: ↑ ↓ ↻ ↺ ⟲

### Import to Unreal

1. Save all icon PNGs to a temporary folder
2. In Unreal Editor, navigate to **Content Browser**
3. Create folder: `Content/RobotRally/UI/Textures/Icons/`
4. Drag and drop all 7 PNG files into the folder
5. For each texture, set compression settings:
   - **Compression Settings**: UserInterface2D (or BC7)
   - **Texture Group**: UI
   - **sRGB**: Enabled
   - **Never Stream**: True (for small UI textures)

---

## Part 3: Create Blueprint Widget (WBP_CardSlot)

### Step 1: Create the Blueprint

1. Open Unreal Editor
2. In Content Browser, navigate to `Content/RobotRally/UI/Widgets/`
3. Right-click → **User Interface** → **Widget Blueprint**
4. Name it: `WBP_CardSlot`
5. Open the Blueprint

### Step 2: Set Parent Class

1. In the Blueprint editor, click **File** → **Reparent Blueprint**
2. Search for and select: `CardWidget` (your C++ class)
3. Save the Blueprint

### Step 3: Design the Widget Layout

**Root Widget: Size Box**
- Add **Size Box** as root
- Settings:
  - Width Override: 120
  - Height Override: 160
  - Child → Add **Overlay**

**Inside Overlay, add these child widgets:**

#### 1. Background Image
- Widget Type: **Image**
- Name: `BackgroundImage`
- Settings:
  - Anchors: Fill (0,0,1,1)
  - Color and Opacity: Bind to `GetCardTypeColor()`
  - Draw As: Box
  - Tiling: No Tile

#### 2. Card Icon
- Widget Type: **Image**
- Name: `CardIconImage`
- Settings:
  - Anchors: Center
  - Size: 64x64 (or Auto based on icon size)
  - Position: Center of card
  - Color: White (will show icon texture)
  - Brush: Will be set dynamically in Blueprint

#### 3. Action Text
- Widget Type: **Text Block**
- Name: `ActionTextBlock`
- Settings:
  - Anchors: Bottom Center
  - Position: Bottom of card, above bottom edge by 10px
  - Font Size: 14
  - Justification: Center
  - Color: White
  - Shadow Offset: (1, 1) for readability

#### 4. Priority Badge (Border + Text)
- Widget Type: **Border**
- Name: `PriorityBadge`
- Settings:
  - Anchors: Top Right
  - Position: Top-right corner (10px from edge)
  - Size: 40x40
  - Padding: 5px all sides
  - Brush Color: Bind to `GetPriorityBadgeColor()`
  - Content Alignment: Center

**Inside Priority Badge Border:**
- Widget Type: **Text Block**
- Name: `PriorityTextBlock`
- Settings:
  - Font Size: 16
  - Font: Bold
  - Justification: Center
  - Color: Black (for contrast on colored badge)

### Step 4: Implement OnCardDataChanged Event

1. In the **Graph** tab, find the inherited event: `OnCardDataChanged`
   (If not visible, click **Override** dropdown and add it)

2. Implement the following logic:

```
Event OnCardDataChanged
├─> Set Text (ActionTextBlock)
│   └─ Text: GetCardActionName()
│
├─> Set Text (PriorityTextBlock)
│   └─ Text: ToText(CardData.Priority)
│
└─> Set Brush from Texture (CardIconImage)
    └─ Texture: Load based on GetCardIconName()
       (Use "Make Literal SoftObjectPath" and "Load Asset Async")
```

**Detailed Blueprint Nodes:**
1. **Get Card Action Name** → **Set Text** (ActionTextBlock)
2. **Get** CardData → **Break FRobotCard** → Get Priority → **To Text** → **Set Text** (PriorityTextBlock)
3. **Get Card Icon Name** → **Append** "/Game/RobotRally/UI/Textures/Icons/" → **Make Soft Object Path** → **Load Asset (Texture2D)** → **Set Brush from Texture** (CardIconImage)

### Step 5: Implement OnCardStateChanged Event

1. Add the inherited event: `OnCardStateChanged`

2. Implement visual changes based on `CurrentState`:

```
Event OnCardStateChanged
└─> Switch on ECardWidgetState (CurrentState)
    ├─ Default:
    │  ├─ Set Opacity: 1.0
    │  ├─ Set Render Scale: (1.0, 1.0)
    │  └─ Set Border Brush Color: White
    │
    ├─ Hover:
    │  ├─ Set Opacity: 1.0
    │  ├─ Set Render Scale: (1.05, 1.05)
    │  └─ Set Border Brush Color: Yellow with Glow
    │
    ├─ Selected:
    │  ├─ Set Opacity: 1.0
    │  ├─ Set Render Scale: (1.0, 1.0)
    │  └─ Set Border Brush: Thick Yellow Border
    │
    ├─ InRegister:
    │  ├─ Set Opacity: 0.8
    │  ├─ Set Render Scale: (1.0, 1.0)
    │  └─ Set BackgroundImage Color: Gray (desaturated)
    │
    └─ Disabled:
       ├─ Set Opacity: 0.5
       ├─ Set Render Scale: (1.0, 1.0)
       └─ Desaturate colors
```

### Step 6: Add Border for Visual States (Optional)

To make states more visible, add a **Border** widget:
1. Insert **Border** between Size Box and Overlay
2. Name: `CardBorder`
3. Settings:
   - Padding: 2px all sides
   - Brush Color: White (default)
   - Content: Overlay (move existing overlay inside)

Update `OnCardStateChanged` to modify `CardBorder` color based on state.

### Step 7: Compile and Test

1. Click **Compile** in the Blueprint editor
2. Save the Blueprint
3. The widget is now ready to be instantiated by `UProgrammingDeckWidget`

---

## Part 4: Testing the Card Widget

### Create a Test Widget

To test the card widget before Phase 3 integration:

1. Create a new Widget Blueprint: `WBP_CardTest`
2. Add **Canvas Panel** as root
3. Add `WBP_CardSlot` as a child
4. Position it in the center
5. In the **Graph** tab:
   - **Event Construct** → **Create Widget** (WBP_CardSlot) → **Add to Viewport**
   - Call **Set Card Data** with test values:
     - Action: Move2
     - Priority: 420
     - HandIndex: 0
   - Call **Set Card State** with: Default

6. Add this test widget to your HUD temporarily to see the card

### Test Checklist

- [ ] Card displays with correct background color
- [ ] Icon shows correct arrow/symbol for action
- [ ] Action text shows correct label ("Move 2", etc.)
- [ ] Priority badge shows correct number
- [ ] Priority badge has correct color (gold/silver/bronze)
- [ ] All 7 card types display correctly
- [ ] Card is readable at 1920x1080 resolution
- [ ] Card is readable at 1280x720 resolution

---

## Part 5: Create Test Cards in Editor

### Option A: Create in C++ Test Code (Temporary)

Add to `ARobotRallyHUD::BeginPlay()` temporarily:

```cpp
void ARobotRallyHUD::BeginPlay()
{
    Super::BeginPlay();

    // Test card widget (remove after Phase 3)
    if (CardWidgetClass)
    {
        UCardWidget* TestCard = CreateWidget<UCardWidget>(GetOwningPlayerController(), CardWidgetClass);
        if (TestCard)
        {
            FRobotCard TestCardData;
            TestCardData.Action = ECardAction::Move2;
            TestCardData.Priority = 420;

            TestCard->SetCardData(TestCardData, 0);
            TestCard->SetCardState(ECardWidgetState::Default);
            TestCard->AddToViewport();
        }
    }
}
```

### Option B: Create Test Level Widget

1. Create a new level: `TestCardLevel`
2. Add a **Widget Component** to the level
3. Set Widget Class to `WBP_CardTest`
4. Play in Editor to see the card

---

## Success Criteria (Phase 2 Complete)

Check off each item:

- [x] ✅ C++ helper methods implemented and compiled
- [ ] All 7 card icon textures created and imported
- [ ] WBP_CardSlot Blueprint created with correct parent class
- [ ] Widget layout designed (Background, Icon, Text, Priority Badge)
- [ ] OnCardDataChanged event implemented
- [ ] OnCardStateChanged event implemented with all 5 states
- [ ] Card displays correctly with test data
- [ ] All 7 card types display with correct colors and icons
- [ ] Priority badges show correct colors
- [ ] Card is readable at 1920x1080
- [ ] Card is readable at 1280x720

---

## Troubleshooting

### Issue: Icons don't show up
- Check texture compression settings (should be UI-friendly)
- Verify icon path in `GetCardIconName()` matches actual asset path
- Use "Reference Viewer" to ensure texture is loaded

### Issue: Colors are wrong
- Check that `GetCardTypeColor()` is bound in Blueprint
- Verify Color and Opacity binding in Image widgets

### Issue: Text is too small/large
- Adjust Font Size in Text Block widgets
- Test at different resolutions using PIE settings

### Issue: Widget doesn't inherit from CardWidget
- Reparent the Blueprint (File → Reparent Blueprint)
- Ensure CardWidget.h is included and compiled

---

## Next Steps (Phase 3)

Once Phase 2 is complete, Phase 3 will:
- Create `WBP_ProgrammingDeck` layout (hand grid + register slots)
- Instantiate multiple `WBP_CardSlot` widgets
- Connect to network data flow (PlayerState / GameMode)

---

**Document Version**: 1.0
**Last Updated**: 2026-02-11
**Author**: RobotRally Development Team
