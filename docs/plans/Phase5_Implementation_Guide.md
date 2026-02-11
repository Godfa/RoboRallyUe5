# Phase 5 Implementation Guide - HUD Elements Migration

**Status**: C++ Complete, Blueprint Pending
**Complexity**: Medium
**Prerequisites**: Phase 1-4 Complete

---

## Overview

Phase 5 migrates remaining HUD elements from Canvas drawing to UMG widgets. This includes health bar, lives counter, checkpoint progress, and game state panel. All C++ helper methods and update logic are implemented - only Blueprint widgets need to be created.

## Part 1: C++ Implementation (✅ COMPLETE)

### Helper Methods for Blueprint Binding

All implemented in `URobotRallyMainWidget`:

✅ **GetHealthPercent()** - Returns health as 0.0-1.0 float
- Used for ProgressBar binding
- Handles divide-by-zero (returns 0.0f if MaxHealth <= 0)

✅ **GetHealthBarColor()** - Returns color based on health percentage
- Green (#7ED321) for >70% health
- Yellow (#F5A623) for 30-70% health
- Red (#D0021B) for <30% health

✅ **GetHealthText()** - Returns formatted health string
- Format: "7/10"
- Uses FText for localization support

✅ **GetGameStateText()** - Returns game state display text
- "PROGRAMMING" for Programming state
- "EXECUTING" for Executing state
- "GAME OVER" for GameOver state

✅ **GetGameStatePanelColor()** - Returns background color for game state
- Blue (#4A90E2) for Programming
- Orange (#F5A623) for Executing
- Gold (#FFD700) for GameOver

### Value Caching System

Update methods now cache values for helper methods:

```cpp
void UpdateHealth(int32 CurrentHealth, int32 MaxHealth)
{
    CachedCurrentHealth = CurrentHealth;
    CachedMaxHealth = MaxHealth;
    OnHealthChanged(CurrentHealth, MaxHealth);
}
```

**Cached Values:**
- CachedCurrentHealth / CachedMaxHealth
- CachedLives
- CachedCurrentCheckpoint / CachedTotalCheckpoints
- CachedGameState

**Benefits:**
- Helper methods can be called anytime from Blueprint
- No need to pass parameters to pure functions
- Consistent state across widget lifecycle

### Canvas HUD Control Flags

Added in `ARobotRallyHUD`:

✅ **bUseUMGWidgets** (default: true)
- Enables UMG widget updates
- Disables Canvas drawing when true (unless bShowCanvasHUD is set)
- Allows clean migration path

✅ **bShowCanvasHUD** (default: false)
- Shows Canvas HUD even when UMG is active
- Useful during migration for side-by-side comparison
- Debug flag for troubleshooting

**Draw Logic:**
```cpp
void DrawHUD()
{
    if (bUseUMGWidgets)
    {
        UpdateWidgetData();
    }

    // Skip Canvas if UMG active (unless debug)
    if (bUseUMGWidgets && !bShowCanvasHUD)
    {
        return; // No Canvas drawing
    }

    // Canvas drawing code...
}
```

---

## Part 2: Create Blueprint Widgets (⏳ PENDING)

### Widget 1: WBP_HealthDisplay

**Purpose:** Show health bar with color interpolation and text

#### Create Widget

1. Navigate to `Content/RobotRally/UI/Widgets/`
2. Create Widget Blueprint: `WBP_HealthDisplay`
3. No need to set parent class (use UserWidget)

#### Design Layout

**Root: Horizontal Box**
- Add **Horizontal Box** as root
- Settings: Fill available space

**Child 1: Health Bar**
- Widget Type: **Progress Bar**
- Name: `HealthBar`
- Settings:
  - Size: 200x20 pixels
  - Percent: **Bind to** → GetHealthPercent()
  - Fill Color and Opacity: **Bind to** → GetHealthBarColor()
  - Style: Set bar image to solid color or simple texture

**Child 2: Health Text**
- Widget Type: **Text Block**
- Name: `HealthText`
- Settings:
  - Text: **Bind to** → GetHealthText()
  - Font Size: 16
  - Justification: Center
  - Padding: 5px left
  - Color: White with shadow

#### Blueprint Binding

In the **Details** panel for each widget:

1. **HealthBar → Percent**:
   - Click **Bind** dropdown
   - Select **Create Binding**
   - Select: Get Health Percent

2. **HealthBar → Fill Color and Opacity**:
   - Click **Bind** dropdown
   - Select: Get Health Bar Color

3. **HealthText → Text**:
   - Click **Bind** dropdown
   - Select: Get Health Text

#### Compile and Save

---

### Widget 2: WBP_LivesDisplay

**Purpose:** Show heart icons for remaining lives

#### Create Widget

1. Create Widget Blueprint: `WBP_LivesDisplay`

#### Design Layout

**Root: Horizontal Box**
- Add **Horizontal Box** as root

**Children: Heart Icons (3x)**
- Widget Type: **Image** (3 copies)
- Names: `Heart1`, `Heart2`, `Heart3`
- Settings:
  - Size: 32x32 pixels each
  - Spacing: 5px between hearts
  - Texture: Heart icon (filled/empty)
  - Color: Red (#FF0000) for filled, Gray (#808080) for empty

#### Blueprint Logic

Add **Event Graph** logic to update heart visibility:

1. **Event Construct** or **Tick**:
   - Get owning player → Get HUD → Get Main Widget
   - Access CachedLives
   - Set Heart1 visibility based on Lives >= 1
   - Set Heart2 visibility based on Lives >= 2
   - Set Heart3 visibility based on Lives >= 3

**Better Approach:** Implement in C++ as GetHeartVisible(int32 Index)

For now, manual binding works:
- Heart1: Visible if CachedLives >= 1
- Heart2: Visible if CachedLives >= 2
- Heart3: Visible if CachedLives >= 3

---

### Widget 3: WBP_CheckpointProgress

**Purpose:** Show flag icons for checkpoint collection

#### Create Widget

1. Create Widget Blueprint: `WBP_CheckpointProgress`

#### Design Layout

**Root: Horizontal Box**
- Add **Horizontal Box** as root

**Children: Flag Icons (5x for 5 checkpoints)**
- Widget Type: **Image** (5 copies)
- Names: `Flag1`, `Flag2`, `Flag3`, `Flag4`, `Flag5`
- Settings:
  - Size: 32x32 pixels each
  - Spacing: 5px between flags
  - Texture: Flag icon (filled/outline)
  - Color: Yellow (#FFD700) for collected, Gray (#808080) for remaining

#### Blueprint Logic

Similar to lives, bind visibility:
- Flag1: Filled if CurrentCheckpoint >= 1
- Flag2: Filled if CurrentCheckpoint >= 2
- Etc.

**Alternative:** Use single Image with overlay text showing "2/5"

---

### Widget 4: WBP_GameStatePanel

**Purpose:** Show current game phase with color-coded background

#### Create Widget

1. Create Widget Blueprint: `WBP_GameStatePanel`

#### Design Layout

**Root: Overlay**
- Add **Overlay** as root
- Size: 300x50 pixels

**Child 1: Background**
- Widget Type: **Border**
- Name: `PanelBackground`
- Settings:
  - Brush Color: **Bind to** → GetGameStatePanelColor()
  - Padding: 10px all sides
  - Horizontal Alignment: Center
  - Vertical Alignment: Center

**Child 2: State Text**
- Widget Type: **Text Block**
- Name: `StateText`
- Settings:
  - Text: **Bind to** → GetGameStateText()
  - Font Size: 20
  - Font: Bold
  - Justification: Center
  - Color: White with shadow

#### Compile and Save

---

### Widget 5: Update WBP_MainHUD

Now add all HUD element widgets to the main HUD.

#### Open WBP_MainHUD

1. Navigate to `Content/RobotRally/UI/Widgets/WBP_MainHUD`
2. Open in editor

#### Add HUD Elements

**Top-Left Corner:**
1. Add **Vertical Box** as child of Canvas Panel
2. Name: `TopLeftPanel`
3. Anchors: Top-Left (0, 0)
4. Position: (10, 10)

**Inside TopLeftPanel:**
1. Add **WBP_HealthDisplay**
2. Add **WBP_LivesDisplay**
3. Add **WBP_CheckpointProgress**

**Top-Center:**
1. Add **WBP_GameStatePanel** to Canvas Panel
2. Name: `GameStatePanel`
3. Anchors: Top-Center (0.5, 0)
4. Alignment: (0.5, 0)
5. Position: (0, 10)

#### Compile and Save

---

## Part 3: Testing Phase 5

### Test Checklist

#### Test 1: Health Bar
1. Start PIE
2. Verify health bar shows at top-left
3. Verify health text shows "10/10" (or current health)
4. Take damage (step on laser)
5. Verify bar decreases
6. Verify color changes: Green → Yellow → Red

**Expected:** Health bar updates in real-time with correct colors

#### Test 2: Lives Counter
1. Verify 3 hearts visible at top-left
2. Die (take 10 damage)
3. Verify one heart disappears
4. Respawn
5. Repeat to test all lives

**Expected:** Hearts disappear as lives decrease

#### Test 3: Checkpoint Progress
1. Verify 5 flags visible at top-left (all gray/outline)
2. Reach Checkpoint 1
3. Verify Flag 1 turns yellow/filled
4. Collect remaining checkpoints
5. Verify flags fill in order

**Expected:** Flags fill as checkpoints are collected

#### Test 4: Game State Panel
1. Verify panel shows "PROGRAMMING" with blue background
2. Press E to execute
3. Verify panel changes to "EXECUTING" with orange background
4. Complete execution
5. Verify returns to "PROGRAMMING"
6. Collect all checkpoints
7. Verify "GAME OVER" with gold background

**Expected:** Panel updates with correct text and colors

#### Test 5: Canvas HUD Toggle
1. Set `bUseUMGWidgets = true`, `bShowCanvasHUD = false`
2. Verify only UMG widgets show (no Canvas text)
3. Set `bShowCanvasHUD = true`
4. Verify both UMG and Canvas show (side-by-side)
5. Set `bUseUMGWidgets = false`
6. Verify only Canvas shows (UMG disabled)

**Expected:** Flags control which HUD system is active

---

## Part 4: Performance Considerations

### Update Frequency

All helpers are **BlueprintPure** (no side effects):
- Called on-demand by Blueprint bindings
- No explicit update needed
- Cached values ensure consistent state

### Binding Performance

**Property bindings are checked every frame (60 FPS)**

Typical costs:
- GetHealthPercent(): ~0.001ms (simple division)
- GetHealthBarColor(): ~0.002ms (3 comparisons)
- GetHealthText(): ~0.005ms (string formatting)

**Total per frame: < 0.01ms (negligible at 60 FPS)**

### Optimization Tips

If performance issues arise:
1. Use **Event-driven updates** instead of bindings
2. Update widgets only when values change (dirty flags)
3. Cache formatted strings (only reformat when changed)

For this project, bindings are fine (simple calculations).

---

## Success Criteria

Phase 5 is **complete** when:

- [x] ✅ Helper methods implemented (GetHealthPercent, GetHealthBarColor, etc.)
- [x] ✅ Value caching system added
- [x] ✅ Canvas HUD control flags added (bUseUMGWidgets, bShowCanvasHUD)
- [x] ✅ Project compiles successfully
- [ ] WBP_HealthDisplay created with bindings
- [ ] WBP_LivesDisplay created with hearts
- [ ] WBP_CheckpointProgress created with flags
- [ ] WBP_GameStatePanel created with color-coded background
- [ ] All widgets added to WBP_MainHUD
- [ ] Health bar updates in real-time
- [ ] Lives counter shows correct hearts
- [ ] Checkpoint progress shows correct flags
- [ ] Game state panel shows correct phase
- [ ] 60 FPS maintained

**Current Status:** 4/14 criteria met (29% complete)
**C++ Implementation:** 4/4 criteria met (100% complete)

---

## Migration Strategy

### Incremental Migration Path

1. **Phase 5A:** Health bar only
   - Create WBP_HealthDisplay
   - Test health updates
   - Set bShowCanvasHUD = true for comparison

2. **Phase 5B:** Add lives and checkpoints
   - Create WBP_LivesDisplay and WBP_CheckpointProgress
   - Test updates
   - Verify side-by-side with Canvas

3. **Phase 5C:** Add game state panel
   - Create WBP_GameStatePanel
   - Test state changes
   - Verify colors correct

4. **Phase 5D:** Finalize migration
   - Set bShowCanvasHUD = false
   - Remove Canvas drawing code (optional)
   - Full UMG mode

### Rollback Plan

If issues arise:
1. Set `bUseUMGWidgets = false`
2. Canvas HUD takes over immediately
3. Fix issues, then re-enable UMG

---

## Next Steps (Phase 6)

Phase 6 will migrate the event log:
- Create WBP_EventLog with scrollable message list
- Create WBP_EventMessage with fade animations
- Remove Canvas event log drawing

---

**Document Version**: 1.0
**Last Updated**: 2026-02-11
**Author**: RobotRally Development Team
