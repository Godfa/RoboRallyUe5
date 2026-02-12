# Phase 6: Card Test Widget - Testing Guide

**Purpose**: Test individual card widget (WBP_CardSlot) with icon system before full deck integration
**Complexity**: Simple
**Prerequisites**: Phase 2 C++ CardWidget complete, ActionIcons TMap configured

---

## Overview

WBP_CardTest is a simple test widget that displays a single card to verify:
- C++ icon mapping (GetCardIcon() + ActionIcons TMap)
- Card visual states (colors, priority badges, text)
- Blueprint event flow (OnCardDataChanged)

This test ensures the card widget works correctly before integrating into the full programming deck.

---

## Create WBP_CardTest Blueprint

### Step 1: Create Widget Blueprint

1. **Content Browser** → `Content/RobotRally/UI/Widgets/`
2. **Right-click** → **User Interface** → **Widget Blueprint**
3. Name: `WBP_CardTest`
4. **Double-click** to open Blueprint Editor

---

### Step 2: Designer Layout

#### Root Widget
- **Hierarchy**: Keep `CanvasPanel [Root]` as default

#### Add WBP_CardSlot
1. **Palette** → Search: `WBP_CardSlot` (User Created category)
2. **Drag** WBP_CardSlot onto Canvas Panel
3. **Select** WBP_CardSlot in Hierarchy

#### Configure Position
**Details → Slot (Canvas Panel Slot):**
- **Anchors**: Center preset (middle icon)
- **Position**: X: `0`, Y: `0`
- **Alignment**: X: `0.5`, Y: `0.5`
- **Size**: X: `120`, Y: `160`

#### Set Variable Name
**Details → Name:**
- Name: `TestCard`
- **Check** `Is Variable`

---

### Step 3: Event Graph Logic

**Graph Tab** → Build the following node chain:

#### Event Pre Construct Flow
```
Event Pre Construct
    ↓ (execution)
Set Card Data ──────────→ Set Card State
    ↑ (Target)                ↑ (Target)
TestCard (Get)              TestCard (from Set Card Data)
    ↑ (In Card)               ↑ (New State)
Make FRobotCard             Default (ECardWidgetState)
  - Action: Move2
  - Priority: 420
    ↑ (Hand Index)
    0
```

#### Node Details

**Node 1: Get TestCard**
- Drag `TestCard` variable from **My Blueprint** panel (Get mode)

**Node 2: Make FRobotCard**
- Right-click → Search: `Make FRobotCard`
- Set values:
  - **Action**: `Move2` (dropdown)
  - **Priority**: `420`

**Node 3: Set Card Data**
- Drag from TestCard → Search: `Set Card Data`
- Connect:
  - **Event Pre Construct** (exec) → **Set Card Data** (exec)
  - **TestCard** (Get) → **Target**
  - **Make FRobotCard** (return) → **In Card**
  - **Hand Index**: `0` (literal)

**Node 4: Set Card State**
- Drag from Set Card Data → Search: `Set Card State`
- Connect:
  - **Set Card Data** (exec) → **Set Card State** (exec)
  - **New State**: `Default` (dropdown)

**Compile & Save** (Ctrl+S)

---

## Add Widget to Game

Choose one of these methods:

### Method A: Blueprint Actor (Recommended)

1. **Content Browser** → Right-click → **Blueprint Class**
2. Parent Class: **Actor**
3. Name: `BP_CardTestActor`
4. Open Blueprint
5. **Add Component** → **Widget**
6. **Details** (Widget Component):
   - **Widget Class**: `WBP_CardTest`
   - **Draw Size**: X: `1920`, Y: `1080`
   - **Space**: `Screen`
7. **Compile & Save**
8. **Drag** `BP_CardTestActor` into **MainMap** level
9. **PIE** (Alt+P) to test

### Method B: Level Blueprint (Direct)

1. **Blueprints** → **Open Level Blueprint**
2. **Event BeginPlay** node
3. Right-click → Search: `Create Widget`
   - **Class**: `WBP_CardTest`
4. Connect output → Search: `Add to Viewport`
5. Blueprint flow:
```
Event BeginPlay → Create Widget → Add to Viewport
                  (WBP_CardTest)
```
6. **Compile & Save**
7. **PIE** (Alt+P) to test

---

## Expected Results

When you press **PIE**, you should see:

### Visual Elements
- **Card**: Centered on screen
- **Background Color**: Light Blue (#4A90E2) - Move2 is a movement card
- **Icon**: Two upward arrows (↑↑) - Move2 icon
- **Action Text**: "Move 2" (bottom of card)
- **Priority Badge**: "420" with silver background (top-right)

### Priority Badge Colors
Based on priority value:
- **Gold** (#FFD700): Priority 600+
- **Silver** (#C0C0C0): Priority 300-599
- **Bronze** (#CD7F32): Priority 0-299

---

## Testing All Card Types

Change **Make FRobotCard** Action value in WBP_CardTest Event Graph to test each card:

| Action       | Priority | Expected Icon      | Background Color | Badge Color |
|--------------|----------|-------------------|-----------------|-------------|
| Move1        | 210      | 1 arrow (↑)       | Blue            | Bronze      |
| Move2        | 420      | 2 arrows (↑↑)     | Blue            | Silver      |
| Move3        | 630      | 3 arrows (↑↑↑)    | Blue            | Gold        |
| MoveBack     | 150      | Down arrow (↓)    | Orange          | Bronze      |
| RotateRight  | 540      | Clockwise (↻)     | Green           | Silver      |
| RotateLeft   | 360      | Counter-CW (↺)    | Green           | Silver      |
| UTurn        | 90       | U-turn (⟲)        | Orange          | Bronze      |

**Test each card type** to verify all icons and colors work correctly!

---

## Troubleshooting

### Issue: Icon Not Showing

#### Check 1: ActionIcons TMap Configured?
1. Open **WBP_CardSlot** Blueprint
2. **Class Settings** (toolbar)
3. **Details** → **Card Icons** → **Action Icons**
4. Verify **Move2** → **T_Icon_Move2** is assigned
5. If missing, fill TMap following Phase2_Implementation_Guide.md

#### Check 2: OnCardDataChanged Event
1. Open **WBP_CardSlot** Event Graph
2. Find **Event OnCardDataChanged**
3. Verify:
   - **Get Card Icon** node exists
   - Connected to **Set Brush from Texture** → **CardIconImage**

#### Check 3: Blueprint Debugger
Add debug print in WBP_CardSlot Event Graph:

```
Event OnCardDataChanged
    ↓
Get Card Icon
    ↓
Is Valid? ───→ True: Print String "Icon found!"
    ↓          False: Print String "Icon NULL!"
    ↓
Set Brush from Texture
```

**Is Valid node:**
- Right-click → **Utilities** → **Is Valid**
- Target: **Get Card Icon** output

#### Check 4: Output Log
During PIE:
- **Window** → **Developer Tools** → **Output Log**
- Search for errors: `LogUObjectGlobals`, `LogTexture`, `LogWidget`

#### Check 5: Widget Reflector
1. **Tools** → **Debug** → **Widget Reflector**
2. PIE → Click **Pick Live Widget**
3. Click card in viewport
4. Widget Hierarchy shows:
   - `WBP_CardSlot`
   - → `CardIconImage (Image)`
5. Verify **Brush** → **Resource Object** shows `T_Icon_Move2`

---

### Issue: Wrong Colors

#### Check Background Color Binding
1. **WBP_CardSlot** Designer
2. Select **BackgroundImage**
3. **Details** → **Appearance** → **Color and Opacity**
4. Should be **Bound** to `GetCardTypeColor()`

#### Check Priority Badge Color Binding
1. Select **PriorityBadge** Border
2. **Details** → **Appearance** → **Brush Color**
3. Should be **Bound** to `GetPriorityBadgeColor()`

---

### Issue: Text Not Showing

#### Check Text Bindings
1. **ActionTextBlock**: Bound to `GetCardActionName()`
2. **PriorityTextBlock**: Set in OnCardDataChanged event

#### Check Text Color
- **ActionTextBlock**: Color should be **White**
- **PriorityTextBlock**: Color should be **Black** (contrast on colored badge)

---

## Success Criteria

- [x] Card displays centered on screen
- [x] Icon renders correctly (two upward arrows for Move2)
- [x] Action text displays ("Move 2")
- [x] Priority badge displays ("420")
- [x] Colors are correct (blue background, silver badge)
- [x] All 7 card types work when Action is changed

---

## Next Steps

Once WBP_CardTest works correctly:

1. **Phase 6 Part 2**: Create WBP_ProgrammingDeck (hand grid + registers)
2. **Phase 6 Part 3**: Create WBP_MainHUD (full HUD integration)
3. **Phase 7**: Mouse input and card interaction

---

## Cleanup

**When done testing:**

Option A: Keep test widget for future debugging
- Disable `BP_CardTestActor` in level (uncheck visibility)

Option B: Remove test widget
- Delete `BP_CardTestActor` from level
- Keep `WBP_CardTest` Blueprint for future use

**Do NOT delete WBP_CardTest Blueprint** - it's useful for:
- Testing new card types
- Debugging icon issues
- Verifying card visual states

---

**Document Version**: 1.0
**Last Updated**: 2026-02-12
**Author**: RobotRally Development Team
