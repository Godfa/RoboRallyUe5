# UI Improvement Plan — Enhanced Visual Interface

**Created**: 2026-02-08
**Status**: Planning
**Complexity**: Medium
**Phase**: 5 (UI & Interface)

---

## Executive Summary

This plan outlines the transformation of RobotRally's functional but basic text-based HUD into a polished, intuitive visual interface using Unreal Engine's UMG (Unreal Motion Graphics) system. The goal is to enhance usability, provide better feedback, and create a more engaging player experience while maintaining the game's strategic depth.

---

## Current State Analysis

### What Exists Now

**C++ HUD (ARobotRallyHUD)**
- ✅ Health bar (numeric display)
- ✅ Checkpoint counter (text)
- ✅ Event log (scrolling text messages)
- ✅ Game state indicator (Programming/Executing/GameOver)

**Input System**
- ✅ Keyboard-only (WASD movement, 1-9 card selection, E execute, Backspace undo)
- ✅ No visual feedback for card selection (beyond console logs)
- ✅ Hand and registers exist in code but not visually

### Current Limitations

1. **No Visual Card Representation**: Players must remember which number corresponds to which card
2. **No Register Visualization**: Cannot see programmed sequence before execution
3. **Minimal Feedback**: Text-only messages, no animations or visual polish
4. **No Mouse Support**: Keyboard-only limits accessibility
5. **Basic Health Display**: Simple bar, no visual danger indicators
6. **Cluttered Text**: Event log can become overwhelming

---

## UI/UX Improvement Goals

### Primary Goals

1. **Visual Card System**: Cards displayed with icons, actions, and priorities
2. **Interactive Programming Deck**: Drag-and-drop or click-to-select card programming
3. **Clear Register Display**: Visual sequence of programmed cards before execution
4. **Enhanced Feedback**: Animations, transitions, and visual effects for all actions
5. **Improved Readability**: Clear hierarchy, consistent styling, accessible design

### User Experience Principles

- **Clarity First**: Information should be instantly readable
- **Minimal Clicks**: Efficient interaction (keyboard shortcuts remain)
- **Visual Feedback**: Every action gets immediate visual response
- **Forgiveness**: Easy undo, clear consequences before committing
- **Accessibility**: Colorblind-friendly, scalable UI, keyboard shortcuts

---

## Proposed UI Architecture

### Widget Hierarchy

```
WBP_MainHUD (Root Widget)
├── WBP_TopBar
│   ├── WBP_HealthDisplay
│   │   ├── Health Bar (progress bar)
│   │   ├── Health Text (7/10)
│   │   └── Damage Indicator (pulse on hit)
│   ├── WBP_LivesDisplay
│   │   └── Heart Icons x3 (filled/empty)
│   └── WBP_CheckpointProgress
│       ├── Checkpoint Icons (flags)
│       └── Counter Text (2/5)
├── WBP_GameStatePanel
│   ├── Phase Text (Programming/Executing)
│   ├── Phase Icon
│   └── Timer (optional - for timed mode)
├── WBP_ProgrammingDeck
│   ├── WBP_HandArea
│   │   └── WBP_CardSlot x9 (scalable array)
│   ├── WBP_RegisterArea
│   │   └── WBP_RegisterSlot x5
│   └── WBP_ActionButtons
│       ├── Execute Button (E)
│       ├── Undo Button (Backspace)
│       └── Clear All Button (optional)
└── WBP_EventLog
    ├── Message Container (scrollable)
    └── Message Widgets (fade in/out)
```

---

## Widget Specifications

### 1. WBP_CardSlot (Individual Card Widget)

**Purpose**: Display a single Robot Rally card with visual design

#### Visual Design
```
┌─────────────────────┐
│  [Priority Badge]   │  ← Top-right corner: Priority number in colored circle
│                     │
│     [Action Icon]   │  ← Center: Large icon representing action
│        ↑ → ↻        │     Move1 = ↑, RotateRight = ↻, etc.
│                     │
│   [Action Text]     │  ← Bottom: "Move 1", "Rotate Right", etc.
└─────────────────────┘
```

#### Properties
- **Size**: 120x160 pixels (card aspect ratio ~3:4)
- **Background**: Colored gradient based on card type
  - Movement cards: Blue gradient
  - Rotation cards: Green gradient
  - Special (UTurn/MoveBack): Orange gradient
- **Priority Badge**: Circular badge, size based on priority
  - High priority (600+): Gold/yellow
  - Medium (300-599): Silver/gray
  - Low (0-299): Bronze/brown

#### Card States
- **Default**: White border, full opacity
- **Hover**: Glowing border, slight scale up (1.1x)
- **Selected**: Thick border, highlight color
- **In Register**: Locked appearance, gray background
- **Disabled**: Desaturated, 50% opacity
- **Executing**: Pulsing glow effect

#### Actions/Icons by Card Type
| Card Action | Icon | Color Accent |
|---|---|---|
| Move1 | ↑ (single arrow) | Light Blue |
| Move2 | ↑↑ (double arrow) | Blue |
| Move3 | ↑↑↑ (triple arrow) | Dark Blue |
| MoveBack | ↓ (down arrow) | Red |
| RotateRight | ↻ (clockwise arrow) | Green |
| RotateLeft | ↺ (counter-clockwise) | Green |
| UTurn | ⟲ (180° arrow) | Orange |

### 2. WBP_ProgrammingDeck (Card Selection Interface)

**Purpose**: Main interaction panel for programming robot movement

#### Layout
```
┌────────────────────────────────────────────────────────────┐
│  Hand (Your Cards)                              [9 - Damage] │
│  ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐    │
│  │ 1 │ │ 2 │ │ 3 │ │ 4 │ │ 5 │ │ 6 │ │ 7 │ │ 8 │ │ 9 │    │
│  └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘    │
│                                                              │
│  ─────────────────────────────────────────────────────────  │
│                                                              │
│  Program (Your Sequence)                                     │
│  ┌───────┐ ┌───────┐ ┌───────┐ ┌───────┐ ┌───────┐        │
│  │Reg 1  │ │Reg 2  │ │Reg 3  │ │Reg 4  │ │Reg 5  │        │
│  │ [   ] │ │ [   ] │ │ [   ] │ │ [   ] │ │ [   ] │        │
│  └───────┘ └───────┘ └───────┘ └───────┘ └───────┘        │
│                                                              │
│  [Execute (E)]  [Undo Last (Backspace)]  [Clear All]       │
└────────────────────────────────────────────────────────────┘
```

#### Positioning
- **Anchor**: Bottom of screen, centered
- **Size**: 90% screen width, ~300 pixels height
- **Visibility**: Show in Programming state, hide in Executing/GameOver

#### Hand Area
- **Display**: Horizontal scroll box (if >9 cards ever needed)
- **Card Spacing**: 10px gap between cards
- **Number Labels**: Small badge on each card (1-9)
- **Empty Slots**: Grayed out rectangles when damage reduces hand

#### Register Area
- **Display**: 5 fixed slots, labeled Register 1-5
- **Empty State**: Dashed border, "Drop card here" or "Press 1-9"
- **Filled State**: Card displayed, subtle glow
- **Execution Indicator**: Progress bar underneath showing which register is executing

#### Action Buttons
- **Execute Button**: Large green button, disabled if no cards programmed
- **Undo Button**: Yellow button, disabled if registers empty
- **Clear All**: Red button, confirmation dialog

### 3. WBP_HealthDisplay (Health & Damage)

**Purpose**: Show robot health status with visual clarity

#### Visual Design
```
┌──────────────────────────────┐
│  HEALTH                      │
│  ████████████░░░░░░  70%     │  ← Green bar transitions to red when low
│  7/10                        │
└──────────────────────────────┘
```

#### Features
- **Color-Coded Bar**:
  - 70-100%: Green
  - 40-69%: Yellow
  - 0-39%: Red
- **Damage Flash**: Red pulse effect when taking damage
- **Smooth Transitions**: Bar animates smoothly when damage changes
- **Numeric Display**: Both current/max and percentage

### 4. WBP_LivesDisplay (Life Counter)

**Purpose**: Show remaining lives with iconic representation

#### Visual Design
```
┌─────────────────┐
│  LIVES          │
│  ❤ ❤ ❤          │  ← 3 lives remaining
│  or              │
│  ❤ ❤ ♡          │  ← 2 lives remaining (empty heart)
└─────────────────┘
```

#### Features
- **Heart Icons**: Filled for remaining lives, empty/cracked for lost lives
- **Loss Animation**: Heart breaks/fades when life lost
- **Pulse Effect**: Hearts pulse when near death (1 life remaining)

### 5. WBP_CheckpointProgress (Checkpoint Tracking)

**Purpose**: Show checkpoint collection progress

#### Visual Design
```
┌──────────────────────────────┐
│  CHECKPOINTS                 │
│  🚩 🚩 ⚑ ⚑ ⚑    2/5          │  ← Filled flags = collected, outline = remaining
└──────────────────────────────┘
```

#### Features
- **Flag Icons**: Filled when collected, outline when remaining
- **Sequential Indication**: Glow on next checkpoint to collect
- **Collection Animation**: Flag fills with color burst when collected

### 6. WBP_GameStatePanel (Phase Display)

**Purpose**: Show current game phase clearly

#### Visual Design
```
┌────────────────────────┐
│  🔧 PROGRAMMING         │  ← Icon + text, color-coded
└────────────────────────┘
```

#### Phase Styles
- **Programming**: 🔧 Wrench icon, Blue background
- **Executing**: ▶ Play icon, Orange background
- **Game Over (Win)**: ★ Star icon, Gold background
- **Game Over (Lose)**: ✖ X icon, Red background

### 7. WBP_EventLog (Message Feed)

**Purpose**: Show game events and feedback messages

#### Visual Design
```
┌────────────────────────────┐
│ ✓ Checkpoint 1 collected   │  ← Recent (fade in)
│ ⚠ Wrong checkpoint!        │  ← Older (normal opacity)
│ ⚡ Laser damage: 1         │  ← Oldest (fading out)
└────────────────────────────┘
```

#### Features
- **Icon-Based**: Each message type has icon (✓ success, ⚠ warning, ⚡ damage, etc.)
- **Color-Coded**: Green for positive, red for negative, yellow for warnings
- **Fade System**: Messages fade in, display for 5s, fade out
- **Scrollable**: Can scroll to see message history
- **Position**: Bottom-right corner, semi-transparent background

---

## Interaction Design

### Keyboard Workflow (Current - Keep Working)

1. **Programming Phase**:
   - View hand (9 cards visible at bottom)
   - Press **1-9** → Card moves from hand to next empty register with animation
   - Press **Backspace** → Last card returns to hand with animation
   - Press **E** → Deck minimizes, execution begins

2. **Executing Phase**:
   - Deck hidden or minimized
   - Current register highlighted/glowing
   - Cards gray out as they execute

### Mouse Workflow (New - Optional)

1. **Click to Select**:
   - Click card in hand → Moves to next empty register
   - Click card in register → Returns to hand (undo)

2. **Drag and Drop** (Advanced):
   - Drag card from hand → Drop on specific register slot
   - Drag card between registers → Swap or reorder
   - Drag card outside → Cancel (returns to hand)

3. **Button Clicks**:
   - Click "Execute" → Start execution
   - Click "Undo" → Remove last card
   - Click "Clear All" → Clear all registers (with confirmation)

### Visual Feedback

Every action gets immediate visual feedback:
- **Card Selection**: Card slides from hand to register (0.3s animation)
- **Undo**: Card slides from register back to hand (0.3s animation)
- **Execution Start**: Deck minimizes with scale animation
- **Register Execution**: Current register pulses/glows
- **Damage Taken**: Health bar flashes red, screen shake
- **Checkpoint Collected**: Flag fills with particle burst
- **Death**: Screen fade, dramatic effect

---

## Technical Implementation

### File Structure
```
Content/RobotRally/UI/
├── Widgets/
│   ├── Cards/
│   │   ├── WBP_CardSlot.uasset
│   │   └── WBP_CardSlot_BP.uasset (for testing)
│   ├── Deck/
│   │   ├── WBP_ProgrammingDeck.uasset
│   │   ├── WBP_HandArea.uasset
│   │   └── WBP_RegisterArea.uasset
│   ├── HUD/
│   │   ├── WBP_MainHUD.uasset
│   │   ├── WBP_HealthDisplay.uasset
│   │   ├── WBP_LivesDisplay.uasset
│   │   ├── WBP_CheckpointProgress.uasset
│   │   └── WBP_GameStatePanel.uasset
│   └── Utility/
│       ├── WBP_EventLog.uasset
│       └── WBP_MessageEntry.uasset
├── Textures/
│   ├── Icons/
│   │   ├── T_Icon_Move1.png
│   │   ├── T_Icon_Move2.png
│   │   ├── T_Icon_RotateRight.png
│   │   └── ... (all card action icons)
│   └── UI/
│       ├── T_CardBackground.png
│       ├── T_Heart_Full.png
│       ├── T_Heart_Empty.png
│       └── T_Flag.png
├── Materials/
│   ├── M_CardGradient_Blue.uasset
│   ├── M_CardGradient_Green.uasset
│   └── M_CardGradient_Orange.uasset
└── Fonts/
    └── F_RobotoMono.uasset (monospace for numbers)
```

### C++ Integration Points

#### ARobotRallyHUD.h
```cpp
// Add widget references
UPROPERTY(EditDefaultsOnly, Category="UI")
TSubclassOf<UUserWidget> MainHUDClass;

UPROPERTY()
UUserWidget* MainHUDWidget;

// Events to update UI
void UpdateHealthDisplay(int32 CurrentHealth, int32 MaxHealth);
void UpdateLivesDisplay(int32 RemainingLives);
void UpdateCheckpointProgress(int32 Collected, int32 Total);
void UpdateGameState(EGameState NewState);
void ShowProgrammingDeck(bool bShow);
void UpdateHandDisplay(const TArray<FRobotCard>& Hand);
void UpdateRegistersDisplay(const TArray<FRobotCard>& Registers);
void HighlightExecutingRegister(int32 RegisterIndex);
void AddEventLogMessage(const FString& Message, EMessageType Type);
```

#### ARobotRallyGameMode.h
```cpp
// UI update delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHandUpdated, const TArray<FRobotCard>&, Hand, int32, HandSize);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRegistersUpdated, const TArray<FRobotCard>&, Registers);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChanged, EGameState, NewState);

UPROPERTY(BlueprintAssignable)
FOnHandUpdated OnHandUpdated;

UPROPERTY(BlueprintAssignable)
FOnRegistersUpdated OnRegistersUpdated;

UPROPERTY(BlueprintAssignable)
FOnGameStateChanged OnGameStateChanged;
```

### Blueprint Data Binding

Use **Event-Driven Updates** (not Tick):
- GameMode broadcasts events when hand/registers/state changes
- Widgets bind to these events and update visuals
- More efficient than polling every frame

### Animation Blueprints

Create widget animations for:
- **Card Slide In**: 0.3s ease-out from hand to register
- **Card Slide Out**: 0.3s ease-out from register to hand
- **Health Bar Damage**: 0.2s flash red, then smooth transition
- **Checkpoint Collect**: 0.5s scale + glow burst
- **Deck Show/Hide**: 0.4s scale + fade
- **Message Fade**: 0.3s fade in, 5s hold, 0.3s fade out

---

## Implementation Phases

### Phase 5A: Core Card Widgets (Week 1)

**Goal**: Get cards visually represented

- [ ] Create WBP_CardSlot with all 7 card types
- [ ] Import/create card action icons
- [ ] Implement card states (default, hover, selected, disabled)
- [ ] Test card appearance at different resolutions
- [ ] Create card materials (blue/green/orange gradients)

**Success Criteria**:
- All 7 card types display correctly with icons and priorities
- Cards respond to hover (glow effect)
- Cards readable at 1920x1080 and 1280x720

### Phase 5B: Programming Deck (Week 1-2)

**Goal**: Functional card selection interface

- [ ] Create WBP_ProgrammingDeck layout
- [ ] Create WBP_HandArea with card slots
- [ ] Create WBP_RegisterArea with 5 slots
- [ ] Bind keyboard input (1-9, E, Backspace) to UI updates
- [ ] Implement card selection animations (slide from hand to register)
- [ ] Implement undo animations (slide from register to hand)
- [ ] Connect to GameMode data (Hand and Registers arrays)

**Success Criteria**:
- Pressing 1-9 moves cards from hand to registers visually
- Backspace returns cards to hand visually
- E key hides deck and starts execution
- Animations are smooth (60 FPS)

### Phase 5C: HUD Elements (Week 2)

**Goal**: Replace text HUD with visual widgets

- [ ] Create WBP_HealthDisplay with progress bar
- [ ] Create WBP_LivesDisplay with heart icons
- [ ] Create WBP_CheckpointProgress with flag icons
- [ ] Create WBP_GameStatePanel with phase indicator
- [ ] Create WBP_EventLog with message feed
- [ ] Bind all widgets to C++ data sources
- [ ] Implement damage flash effect
- [ ] Implement checkpoint collection animation

**Success Criteria**:
- Health bar updates in real-time
- Lives show correct count with heart icons
- Checkpoints show progress with flags
- Event log shows messages with icons and fading

### Phase 5D: Mouse Support (Week 3 - Optional)

**Goal**: Add mouse interaction

- [ ] Implement click-to-select for cards
- [ ] Implement button clicks (Execute, Undo, Clear All)
- [ ] Optional: Implement drag-and-drop
- [ ] Add hover tooltips for cards (show full action description)
- [ ] Test mouse + keyboard hybrid usage

**Success Criteria**:
- Can play game with mouse only
- Can play game with keyboard only
- Can mix mouse and keyboard seamlessly

### Phase 5E: Polish & Animation (Week 3-4)

**Goal**: Make UI feel polished

- [ ] Add all widget animations (card slides, health flash, etc.)
- [ ] Add sound effects for UI interactions (card select, execute, undo)
- [ ] Implement screen shake on damage
- [ ] Add particle effects (checkpoint burst, death explosion)
- [ ] Test at all resolutions (720p, 1080p, 4K)
- [ ] Accessibility pass (colorblind mode, UI scale options)

**Success Criteria**:
- All interactions have visual and audio feedback
- Animations smooth at 60 FPS
- UI readable at all tested resolutions
- Colorblind-friendly (shape/icon differentiation)

---

## Visual Design Mockups

### Color Palette

**Primary Colors**:
- Background: Dark gray (#2B2B2B)
- Panel: Medium gray (#3A3A3A)
- Text: White (#FFFFFF)
- Accent Blue: (#4A90E2) - Movement cards
- Accent Green: (#7ED321) - Rotation cards
- Accent Orange: (#F5A623) - Special cards

**Status Colors**:
- Health Good: Green (#7ED321)
- Health Warning: Yellow (#F5A623)
- Health Critical: Red (#D0021B)
- Success: Green (#7ED321)
- Warning: Yellow (#F5A623)
- Error: Red (#D0021B)

### Typography

- **Primary Font**: Roboto (clean, readable)
- **Numbers Font**: Roboto Mono (monospace for alignment)
- **Headers**: 24pt, Bold
- **Body**: 16pt, Regular
- **Small Text**: 12pt, Regular

### Icon Style

- **Line-based**: Simple, bold lines
- **Monochromatic**: Use color tinting for card types
- **Consistent Size**: All icons 64x64px base
- **High Contrast**: Readable on dark and light backgrounds

---

## Accessibility Considerations

### Colorblind Support

1. **Don't Rely on Color Alone**:
   - Movement cards: Blue + ↑ arrow icon
   - Rotation cards: Green + ↻ icon
   - Special cards: Orange + unique icon

2. **Icon Differentiation**:
   - Each card action has unique icon shape
   - Colorblind players can identify by shape alone

3. **High Contrast Mode** (optional):
   - Black text on white cards
   - Bold outlines
   - No gradient backgrounds

### Keyboard Accessibility

- All UI functions accessible via keyboard
- Tab navigation through UI elements
- Visual focus indicators
- Keyboard shortcuts shown in tooltips

### UI Scaling

- Support UI scale from 80% to 150%
- Test at multiple monitor sizes
- Ensure text remains readable when scaled

### Audio Alternatives

- Important events have sound effects
- Can play with sound off (visual feedback sufficient)
- Closed captions for audio cues (optional)

---

## Performance Considerations

### Widget Optimization

1. **Lazy Loading**: Only create widgets when needed
2. **Object Pooling**: Reuse card widgets instead of creating/destroying
3. **Tick Optimization**: Use event-driven updates, not Tick
4. **Texture Atlasing**: Combine small textures into atlases
5. **Material Instances**: Use material instances for card colors

### Profiling Targets

- **Widget Draw Calls**: <50 draw calls for entire UI
- **Frame Time**: UI updates should take <2ms
- **Memory**: UI textures <50MB total
- **Load Time**: All UI loads in <1 second

### Optimization Checklist

- [ ] Disable Tick on all widgets (use events)
- [ ] Use retainer boxes for static elements
- [ ] Compress textures (DXT5 for RGBA)
- [ ] Use simple materials (unlit, no complex nodes)
- [ ] Profile with `stat UMG` command

---

## Testing Plan

### Functional Testing

- [ ] All 7 card types display correctly
- [ ] Hand size scales with damage (9 → 1)
- [ ] Register programming works (1-9 keys)
- [ ] Undo works (Backspace)
- [ ] Execute works (E key)
- [ ] Health bar updates correctly
- [ ] Lives counter updates correctly
- [ ] Checkpoint progress updates correctly
- [ ] Event log shows messages
- [ ] UI hides/shows in correct game states

### Visual Testing

- [ ] Cards readable at 720p, 1080p, 4K
- [ ] Animations smooth (no stuttering)
- [ ] Colors accessible (colorblind test)
- [ ] Text has sufficient contrast
- [ ] Icons clear and recognizable
- [ ] Layout doesn't overlap or clip

### Performance Testing

- [ ] 60 FPS maintained with full UI
- [ ] No frame drops when showing/hiding deck
- [ ] Memory usage stable over time
- [ ] No texture streaming issues

### Usability Testing

- [ ] New players understand card system
- [ ] Can program 5 cards in <30 seconds
- [ ] Mistakes easy to undo
- [ ] Feedback clear for all actions
- [ ] No confusion about game state

---

## Success Criteria

This UI improvement plan is considered **COMPLETE** when:

- ✅ All Phase 5 implementation items checked off
- ✅ Visual card system implemented with all 7 card types
- ✅ Programming deck fully functional (hand + registers + buttons)
- ✅ Health, lives, checkpoint HUD elements working
- ✅ Event log displaying messages with fading
- ✅ All animations smooth at 60 FPS
- ✅ UI readable at 1280x720 minimum
- ✅ Keyboard controls still work perfectly
- ✅ Optional: Mouse support working
- ✅ No performance degradation from UI
- ✅ Passes all testing checklist items

---

## Future Enhancements (Post-Phase 5)

### Advanced Features (Phase 6+)

1. **Card Tooltips**: Hover to see detailed action description
2. **Animation Previews**: Show predicted robot path when hovering cards
3. **Undo History**: Multi-level undo (not just last card)
4. **Card Sorting**: Sort hand by priority, action type, etc.
5. **Deck Statistics**: Show card distribution, priority ranges
6. **Replay System**: Rewatch execution with UI timeline
7. **Themes**: Different visual themes (dark, light, colorblind)
8. **Modding Support**: Allow custom card icons/designs

### Polish Ideas

- Particle effects on card selection
- Sound effects for every UI interaction
- Camera zoom/focus on executing robot
- Slow-motion execution (hold Shift during execution)
- Card flip animation when dealt
- Register glow trail during execution
- Checkpoint beam/pillar of light

---

## Risk Mitigation

### Potential Issues

1. **Performance Overhead**: UMG can be heavy
   - **Mitigation**: Profile early, optimize aggressively, use retainer boxes

2. **Animation Timing**: Complex animations may feel sluggish
   - **Mitigation**: Keep animations <0.5s, allow skipping (hold key)

3. **Resolution Scaling**: UI may not scale well
   - **Mitigation**: Test at multiple resolutions early, use anchors properly

4. **Controller Support**: Future gamepad support may require redesign
   - **Mitigation**: Design with controller in mind (grid navigation)

5. **Localization**: Hard-coded text may limit future translations
   - **Mitigation**: Use string tables from the start

---

## Resources Needed

### Assets
- Card action icons (7 types) - can use free icon packs or Font Awesome
- Heart icons (filled/empty) - standard gaming icons
- Flag icons - checkpoint markers
- UI textures (backgrounds, borders, buttons) - UE5 defaults or free packs

### Tools
- **Photoshop/GIMP**: For icon creation/editing
- **Figma/Sketch**: For UI mockups and layout planning (optional)
- **UE5 UMG Designer**: Built-in widget editor

### References
- [Hearthstone UI](https://hearthstone.blizzard.com/) - Card-based interface inspiration
- [Slay the Spire UI](https://www.megacrit.com/) - Card game UI best practices
- [Robot Rally Board Game](https://www.avalongames.com/) - Original game UI elements

---

## Implementation Checklist

### Setup (Day 1)
- [ ] Create Content/RobotRally/UI/ folder structure
- [ ] Import placeholder icons
- [ ] Set up UI materials and fonts
- [ ] Create base widget classes (WBP_CardSlot, WBP_ProgrammingDeck)

### Core Widgets (Days 2-5)
- [ ] Implement WBP_CardSlot (all states and animations)
- [ ] Implement WBP_ProgrammingDeck (layout and containers)
- [ ] Connect keyboard input to visual updates
- [ ] Test card selection workflow

### HUD Elements (Days 6-8)
- [ ] Implement WBP_HealthDisplay
- [ ] Implement WBP_LivesDisplay
- [ ] Implement WBP_CheckpointProgress
- [ ] Implement WBP_GameStatePanel
- [ ] Implement WBP_EventLog

### Integration (Days 9-12)
- [ ] Connect all widgets to C++ data sources
- [ ] Implement event delegates in GameMode
- [ ] Test full gameplay loop with new UI
- [ ] Fix bugs and polish animations

### Testing & Polish (Days 13-15)
- [ ] Run full testing plan
- [ ] Fix visual issues
- [ ] Optimize performance
- [ ] Add final touches (sounds, particles, etc.)

---

## Conclusion

This UI improvement plan transforms RobotRally from a functional prototype into a visually polished game with intuitive card-based interaction. By implementing these widgets systematically and testing thoroughly, we'll create a UI that enhances the strategic gameplay while maintaining performance and accessibility.

**Estimated Time**: 3-4 weeks (1 developer, full-time)
**Priority**: High (significantly improves player experience)
**Dependencies**: Phase 1-4 complete (game logic must work first)

---

**Next Steps**:
1. Review and approve this plan
2. Create art asset list (icons, textures)
3. Begin Phase 5A implementation (Core Card Widgets)
4. Set up weekly UI review meetings

---

**Document Version**: 1.0
**Last Updated**: 2026-02-08
**Author**: Development Team
