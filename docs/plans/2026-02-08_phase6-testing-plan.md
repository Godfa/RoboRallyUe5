# Phase 6 Testing Plan — Content & Polish

**Created**: 2026-02-08
**Status**: Draft
**Complexity**: Complex

## Overview

This document outlines the testing procedures for Phase 6 of the RobotRally project, focusing on content creation including game levels, Blueprint implementations, data-driven systems, sound effects, and visual effects. Phase 6 transforms the functional prototype into a polished, content-rich game experience.

## Phase 6 Components Under Test

- **Game Maps** — Level design with varied grid layouts, hazards, and checkpoints
- **Blueprint Classes** — BP child classes from C++ for designer-friendly customization
- **Data Assets** — DataTables and DataAssets for data-driven grid/board configurations
- **Sound Effects** — Audio feedback for movement, damage, checkpoints, win/lose
- **Visual Effects** — Particle systems and VFX for movement, hazards, checkpoints
- **Polish & Juice** — Camera effects, screen shake, transitions, UI polish

---

## Testing Checklist

### 1. Game Maps (/Content/RobotRally/Maps/)

#### 1.1 Map Creation and Organization
- [ ] **Test**: Maps folder exists and is organized
  - **Steps**: Check Content/RobotRally/Maps/ in Content Browser
  - **Expected**: Folder exists with organized structure

- [ ] **Test**: MainMap is primary level
  - **Steps**: Check Project Settings → Maps & Modes → Default Maps
  - **Expected**: MainMap set as Editor Startup Map and Game Default Map

- [ ] **Test**: Maps follow naming convention
  - **Steps**: Check map names
  - **Expected**: Named "Map_TutorialLevel", "Map_EasyBoard", "Map_HardBoard", etc.

#### 1.2 Tutorial/Intro Level
- [ ] **Test**: Tutorial level exists (Map_Tutorial)
  - **Steps**: Open Map_Tutorial in editor
  - **Expected**: Level loads successfully

- [ ] **Test**: Tutorial has simple 5x5 or 6x6 grid
  - **Steps**: Check grid size in tutorial
  - **Expected**: Smaller grid for learning (not full 10x10)

- [ ] **Test**: Tutorial has few hazards
  - **Steps**: Count pits, lasers, conveyors
  - **Expected**: 1-2 of each type max, not overwhelming

- [ ] **Test**: Tutorial has 2-3 checkpoints
  - **Steps**: Count checkpoints
  - **Expected**: Short victory path for learning

- [ ] **Test**: Tutorial checkpoints form clear path
  - **Steps**: View checkpoint positions
  - **Expected**: Obvious sequential path (1→2→3)

#### 1.3 Easy Level
- [ ] **Test**: Easy level exists (Map_EasyBoard)
  - **Steps**: Open Map_EasyBoard in editor
  - **Expected**: Level loads successfully

- [ ] **Test**: Easy level has 8x8 or 10x10 grid
  - **Steps**: Check grid size
  - **Expected**: Medium-sized grid

- [ ] **Test**: Easy level has moderate hazards
  - **Steps**: Count hazards
  - **Expected**: 3-5 pits, 3-5 lasers, 3-5 conveyor chains

- [ ] **Test**: Easy level has 3-4 checkpoints
  - **Steps**: Count checkpoints
  - **Expected**: Medium victory path

- [ ] **Test**: Easy level winnable in 10-15 turns
  - **Steps**: Play level, count turns to win
  - **Expected**: Achievable with reasonable efficiency

#### 1.4 Medium Level
- [ ] **Test**: Medium level exists (Map_MediumBoard)
  - **Steps**: Open Map_MediumBoard in editor
  - **Expected**: Level loads successfully

- [ ] **Test**: Medium level has full 10x10 grid
  - **Steps**: Check grid size
  - **Expected**: Full-sized grid

- [ ] **Test**: Medium level has more hazards
  - **Steps**: Count hazards
  - **Expected**: 6-10 pits, 6-10 lasers, 5-8 conveyor chains

- [ ] **Test**: Medium level has 4-5 checkpoints
  - **Steps**: Count checkpoints
  - **Expected**: Longer victory path

- [ ] **Test**: Medium level requires strategic planning
  - **Steps**: Play level
  - **Expected**: Cannot win by moving randomly, requires thought

#### 1.5 Hard Level
- [ ] **Test**: Hard level exists (Map_HardBoard)
  - **Steps**: Open Map_HardBoard in editor
  - **Expected**: Level loads successfully

- [ ] **Test**: Hard level has full 10x10 grid with dense hazards
  - **Steps**: Check grid and hazards
  - **Expected**: 10+ pits, 10+ lasers, complex conveyor networks

- [ ] **Test**: Hard level has 5+ checkpoints
  - **Steps**: Count checkpoints
  - **Expected**: Long victory path

- [ ] **Test**: Hard level has complex conveyor chains
  - **Steps**: Check conveyor patterns
  - **Expected**: 5-10 tile chains, intersecting paths

- [ ] **Test**: Hard level is challenging but fair
  - **Steps**: Play level
  - **Expected**: Difficult but winnable with good planning

#### 1.6 Custom/Puzzle Levels (Optional)
- [ ] **Test**: Puzzle level with specific solutions
  - **Steps**: Create level with exact card sequence needed
  - **Expected**: Only specific card combinations win

- [ ] **Test**: Maze level with tight corridors
  - **Steps**: Create level with narrow paths between pits
  - **Expected**: Requires precise movement

- [ ] **Test**: Conveyor gauntlet level
  - **Steps**: Create level dominated by conveyors
  - **Expected**: Conveyors are main challenge

#### 1.7 Level Design Validation
- [ ] **Test**: All levels have valid spawn position
  - **Steps**: Play each level, check robot spawn
  - **Expected**: Robot spawns on Normal tile, not hazard

- [ ] **Test**: All levels have reachable checkpoints
  - **Steps**: Verify path exists to each checkpoint
  - **Expected**: No unreachable checkpoints surrounded by pits

- [ ] **Test**: Checkpoint numbers are sequential (1→2→3)
  - **Steps**: Check checkpoint numbers in each level
  - **Expected**: No gaps (no 1→3 without 2)

- [ ] **Test**: Levels have balanced difficulty curve
  - **Steps**: Play Tutorial → Easy → Medium → Hard
  - **Expected**: Smooth progression, not sudden spikes

- [ ] **Test**: No softlock situations (unwinnable states)
  - **Steps**: Test various movement patterns
  - **Expected**: Robot can always continue or die (no getting stuck alive but unable to progress)

---

### 2. Blueprint Classes (/Content/RobotRally/Blueprints/)

#### 2.1 BP_RobotPawn
- [ ] **Test**: BP_RobotPawn exists and inherits from ARobotPawn
  - **Steps**: Check Content/RobotRally/Blueprints/BP_RobotPawn
  - **Expected**: Blueprint child class of ARobotPawn (C++)

- [ ] **Test**: BP_RobotPawn has customizable mesh references
  - **Steps**: Open BP_RobotPawn, check Details panel
  - **Expected**: Exposed properties: BodyMeshAsset, DirectionMeshAsset, BodyColor, DirectionColor

- [ ] **Test**: BP_RobotPawn default mesh values set
  - **Steps**: Check mesh properties
  - **Expected**: Cube and cone meshes assigned with default colors

- [ ] **Test**: BP_RobotPawn compiles without errors
  - **Steps**: Open Blueprint, click Compile
  - **Expected**: Green checkmark, no errors

- [ ] **Test**: BP_RobotPawn spawns correctly in level
  - **Steps**: Spawn BP_RobotPawn in level, play
  - **Expected**: Robot appears with custom meshes and colors

#### 2.2 BP_GridManager
- [ ] **Test**: BP_GridManager exists and inherits from AGridManager
  - **Steps**: Check Content/RobotRally/Blueprints/BP_GridManager
  - **Expected**: Blueprint child class of AGridManager (C++)

- [ ] **Test**: BP_GridManager has customizable tile properties
  - **Steps**: Open BP_GridManager, check Details panel
  - **Expected**: Exposed properties: TileMeshAsset, TileTypeMeshes (map), GridSize

- [ ] **Test**: BP_GridManager can use DataTable for grid layout
  - **Steps**: Add DataTable property for grid data
  - **Expected**: Can reference DT_GridLayout asset

- [ ] **Test**: BP_GridManager compiles without errors
  - **Steps**: Open Blueprint, click Compile
  - **Expected**: Green checkmark, no errors

- [ ] **Test**: BP_GridManager generates grid correctly
  - **Steps**: Place BP_GridManager in level, play
  - **Expected**: Grid appears with custom tile meshes

#### 2.3 BP_RobotRallyGameMode
- [ ] **Test**: BP_RobotRallyGameMode exists and inherits from ARobotRallyGameMode
  - **Steps**: Check Content/RobotRally/Blueprints/BP_RobotRallyGameMode
  - **Expected**: Blueprint child class of ARobotRallyGameMode (C++)

- [ ] **Test**: BP_RobotRallyGameMode has exposed game parameters
  - **Steps**: Open Blueprint, check Details panel
  - **Expected**: Properties for starting lives, register count, hand size formula

- [ ] **Test**: BP_RobotRallyGameMode set as default GameMode
  - **Steps**: Check Project Settings → Maps & Modes
  - **Expected**: BP_RobotRallyGameMode set as Default GameMode

- [ ] **Test**: BP_RobotRallyGameMode compiles without errors
  - **Steps**: Open Blueprint, click Compile
  - **Expected**: Green checkmark, no errors

#### 2.4 BP_RobotController
- [ ] **Test**: BP_RobotController exists and inherits from ARobotController
  - **Steps**: Check Content/RobotRally/Blueprints/BP_RobotController
  - **Expected**: Blueprint child class of ARobotController (C++)

- [ ] **Test**: BP_RobotController compiles without errors
  - **Steps**: Open Blueprint, click Compile
  - **Expected**: Green checkmark, no errors

#### 2.5 Blueprint Organization
- [ ] **Test**: Blueprints folder structure is clean
  - **Steps**: Check /Content/RobotRally/Blueprints/
  - **Expected**: Organized subfolders: /Pawns/, /Game/, /UI/

- [ ] **Test**: All Blueprints follow naming convention
  - **Steps**: Check Blueprint names
  - **Expected**: BP_ prefix for all Blueprints

---

### 3. Data Assets (/Content/RobotRally/Data/)

#### 3.1 DataTable for Grid Layouts
- [ ] **Test**: DT_GridLayouts DataTable exists
  - **Steps**: Check Content/RobotRally/Data/DT_GridLayouts
  - **Expected**: DataTable asset created

- [ ] **Test**: DataTable has FGridLayoutRow struct
  - **Steps**: Check DataTable structure
  - **Expected**: Columns: RowName, GridSize, TileTypes (array), CheckpointPositions (array)

- [ ] **Test**: DataTable contains tutorial level layout
  - **Steps**: Check DataTable rows
  - **Expected**: Row "Tutorial" with 5x5 or 6x6 grid data

- [ ] **Test**: DataTable contains easy/medium/hard layouts
  - **Steps**: Check DataTable rows
  - **Expected**: Rows for "Easy", "Medium", "Hard" with grid data

- [ ] **Test**: GridManager can load layout from DataTable
  - **Steps**: Reference DataTable in GridManager, load row
  - **Expected**: Grid generates from DataTable data

#### 3.2 DataAsset for Card Definitions (Optional)
- [ ] **Test**: DA_CardDefinitions DataAsset exists
  - **Steps**: Check Content/RobotRally/Data/DA_CardDefinitions
  - **Expected**: DataAsset created with UCardDataAsset class

- [ ] **Test**: DataAsset defines all 7 card types
  - **Steps**: Check DataAsset contents
  - **Expected**: Move1/2/3, MoveBack, RotateRight/Left, UTurn defined

- [ ] **Test**: Each card has action, priority range, count
  - **Steps**: Check card properties
  - **Expected**: Action (enum), PriorityMin/Max (int), DeckCount (int)

- [ ] **Test**: GameMode can generate deck from DataAsset
  - **Steps**: Reference DataAsset in GameMode, generate deck
  - **Expected**: 84 cards generated based on DataAsset

#### 3.3 Data Organization
- [ ] **Test**: Data folder structure is clean
  - **Steps**: Check /Content/RobotRally/Data/
  - **Expected**: Organized: /Grids/, /Cards/, /Audio/, /VFX/

- [ ] **Test**: All data assets follow naming convention
  - **Steps**: Check asset names
  - **Expected**: DT_ prefix for DataTables, DA_ prefix for DataAssets

---

### 4. Sound Effects (/Content/RobotRally/Audio/)

#### 4.1 Movement Sounds
- [ ] **Test**: Sound for robot movement exists (SFX_RobotMove)
  - **Steps**: Check Content/RobotRally/Audio/SFX_RobotMove
  - **Expected**: Sound cue or wave file exists

- [ ] **Test**: Movement sound plays when robot moves
  - **Steps**: Move robot with WASD or card
  - **Expected**: Sound plays on each tile moved

- [ ] **Test**: Movement sound pitch/volume appropriate
  - **Steps**: Listen to movement sound
  - **Expected**: Not too loud, not distracting, fits robot theme

- [ ] **Test**: Movement sound doesn't overlap excessively
  - **Steps**: Move 3 tiles quickly (Move3 card)
  - **Expected**: Sounds either blend or play sequentially without cacophony

#### 4.2 Rotation Sounds
- [ ] **Test**: Sound for robot rotation exists (SFX_RobotRotate)
  - **Steps**: Check Content/RobotRally/Audio/SFX_RobotRotate
  - **Expected**: Sound cue or wave file exists

- [ ] **Test**: Rotation sound plays when robot rotates
  - **Steps**: Press A or D, or execute RotateRight/Left card
  - **Expected**: Sound plays on rotation

- [ ] **Test**: Rotation sound distinct from movement
  - **Steps**: Listen to both sounds
  - **Expected**: Different sounds, rotation has mechanical/servo sound

#### 4.3 Damage Sounds
- [ ] **Test**: Sound for laser damage exists (SFX_LaserDamage)
  - **Steps**: Check Content/RobotRally/Audio/SFX_LaserDamage
  - **Expected**: Sound cue or wave file exists

- [ ] **Test**: Laser sound plays when taking damage
  - **Steps**: Stand on laser tile
  - **Expected**: Zap/beam sound plays

- [ ] **Test**: Sound for general damage exists (SFX_Damage)
  - **Steps**: Check Content/RobotRally/Audio/SFX_Damage
  - **Expected**: Sound cue or wave file exists

- [ ] **Test**: Damage sound plays when health decreases
  - **Steps**: Take damage from any source
  - **Expected**: Impact/hurt sound plays

#### 4.4 Death and Respawn Sounds
- [ ] **Test**: Sound for robot death exists (SFX_RobotDeath)
  - **Steps**: Check Content/RobotRally/Audio/SFX_RobotDeath
  - **Expected**: Sound cue or wave file exists

- [ ] **Test**: Death sound plays on robot destruction
  - **Steps**: Die by falling in pit or 10 damage
  - **Expected**: Explosion/destruction sound plays

- [ ] **Test**: Sound for respawn exists (SFX_Respawn)
  - **Steps**: Check Content/RobotRally/Audio/SFX_Respawn
  - **Expected**: Sound cue or wave file exists

- [ ] **Test**: Respawn sound plays on respawn
  - **Steps**: Die and respawn
  - **Expected**: Teleport/spawn sound plays

#### 4.5 Checkpoint Sounds
- [ ] **Test**: Sound for checkpoint collection exists (SFX_CheckpointCollected)
  - **Steps**: Check Content/RobotRally/Audio/SFX_CheckpointCollected
  - **Expected**: Sound cue or wave file exists

- [ ] **Test**: Checkpoint sound plays on collection
  - **Steps**: Move onto correct checkpoint
  - **Expected**: Positive/success sound plays

- [ ] **Test**: Sound for wrong checkpoint exists (SFX_CheckpointWrong)
  - **Steps**: Check Content/RobotRally/Audio/SFX_CheckpointWrong
  - **Expected**: Sound cue or wave file exists

- [ ] **Test**: Wrong checkpoint sound plays on wrong order
  - **Steps**: Try to collect checkpoint 2 before 1
  - **Expected**: Negative/error sound plays

#### 4.6 Win/Lose Sounds
- [ ] **Test**: Sound for victory exists (SFX_Victory)
  - **Steps**: Check Content/RobotRally/Audio/SFX_Victory
  - **Expected**: Sound cue or wave file exists

- [ ] **Test**: Victory sound plays on win
  - **Steps**: Collect all checkpoints
  - **Expected**: Fanfare/success jingle plays

- [ ] **Test**: Sound for game over exists (SFX_GameOver)
  - **Steps**: Check Content/RobotRally/Audio/SFX_GameOver
  - **Expected**: Sound cue or wave file exists

- [ ] **Test**: Game over sound plays on lose
  - **Steps**: Use all 3 lives
  - **Expected**: Defeat/failure sound plays

#### 4.7 UI Sounds
- [ ] **Test**: Sound for card selection exists (SFX_CardSelect)
  - **Steps**: Check Content/RobotRally/Audio/SFX_CardSelect
  - **Expected**: Sound cue or wave file exists

- [ ] **Test**: Card selection sound plays on 1-9 key press
  - **Steps**: Press 1-9 during Programming
  - **Expected**: Card flip/select sound plays

- [ ] **Test**: Sound for execution start exists (SFX_ExecuteStart)
  - **Steps**: Check Content/RobotRally/Audio/SFX_ExecuteStart
  - **Expected**: Sound cue or wave file exists

- [ ] **Test**: Execution sound plays when pressing E
  - **Steps**: Press E to execute cards
  - **Expected**: Start/engage sound plays

#### 4.8 Audio Integration
- [ ] **Test**: All sounds integrated in C++ classes
  - **Steps**: Check RobotPawn, GridManager, GameMode code
  - **Expected**: PlaySound2D() or SpawnSoundAttached() calls present

- [ ] **Test**: Sound volume balanced across all effects
  - **Steps**: Play game with all sounds
  - **Expected**: No sound excessively loud or quiet

- [ ] **Test**: Sounds can be adjusted in settings (optional)
  - **Steps**: Check audio settings
  - **Expected**: Master volume, SFX volume sliders

---

### 5. Visual Effects (/Content/RobotRally/VFX/)

#### 5.1 Movement VFX
- [ ] **Test**: Particle system for movement trail exists (VFX_MovementTrail)
  - **Steps**: Check Content/RobotRally/VFX/VFX_MovementTrail
  - **Expected**: Niagara or Cascade particle system exists

- [ ] **Test**: Movement trail plays during robot movement
  - **Steps**: Move robot, observe
  - **Expected**: Trail of particles follows robot

- [ ] **Test**: Movement trail despawns after movement ends
  - **Steps**: Stop robot, observe
  - **Expected**: Trail fades away after 1-2 seconds

#### 5.2 Damage VFX
- [ ] **Test**: Particle system for laser hit exists (VFX_LaserHit)
  - **Steps**: Check Content/RobotRally/VFX/VFX_LaserHit
  - **Expected**: Niagara or Cascade particle system exists

- [ ] **Test**: Laser VFX plays when taking laser damage
  - **Steps**: Stand on laser tile
  - **Expected**: Sparks/energy effect on robot

- [ ] **Test**: Particle system for general damage exists (VFX_Damage)
  - **Steps**: Check Content/RobotRally/VFX/VFX_Damage
  - **Expected**: Niagara or Cascade particle system exists

- [ ] **Test**: Damage VFX plays when taking any damage
  - **Steps**: Take damage
  - **Expected**: Impact/spark effect on robot

#### 5.3 Death VFX
- [ ] **Test**: Particle system for robot death exists (VFX_Death)
  - **Steps**: Check Content/RobotRally/VFX/VFX_Death
  - **Expected**: Niagara or Cascade particle system exists

- [ ] **Test**: Death VFX plays on robot destruction
  - **Steps**: Die by pit or 10 damage
  - **Expected**: Explosion/destruction effect

- [ ] **Test**: Death VFX is visually impactful
  - **Steps**: Observe death effect
  - **Expected**: Significant explosion, debris, smoke

#### 5.4 Respawn VFX
- [ ] **Test**: Particle system for respawn exists (VFX_Respawn)
  - **Steps**: Check Content/RobotRally/VFX/VFX_Respawn
  - **Expected**: Niagara or Cascade particle system exists

- [ ] **Test**: Respawn VFX plays on robot respawn
  - **Steps**: Die and respawn, observe
  - **Expected**: Teleport/spawn effect at respawn position

- [ ] **Test**: Respawn VFX clearly indicates new position
  - **Steps**: Respawn, observe
  - **Expected**: Bright flash or energy ring at respawn point

#### 5.5 Checkpoint VFX
- [ ] **Test**: Particle system for checkpoint idle exists (VFX_CheckpointIdle)
  - **Steps**: Check Content/RobotRally/VFX/VFX_CheckpointIdle
  - **Expected**: Niagara or Cascade particle system exists

- [ ] **Test**: Checkpoint idle VFX loops on uncollected checkpoints
  - **Steps**: View uncollected checkpoint
  - **Expected**: Glowing/pulsing effect on checkpoint tile

- [ ] **Test**: Particle system for checkpoint collection exists (VFX_CheckpointCollected)
  - **Steps**: Check Content/RobotRally/VFX/VFX_CheckpointCollected
  - **Expected**: Niagara or Cascade particle system exists

- [ ] **Test**: Checkpoint collection VFX plays on collection
  - **Steps**: Collect checkpoint
  - **Expected**: Burst/flash effect, positive feedback

- [ ] **Test**: Checkpoint idle VFX stops after collection
  - **Steps**: Collect checkpoint, observe
  - **Expected**: Idle glow disappears

#### 5.6 Conveyor VFX
- [ ] **Test**: Particle system for conveyor movement exists (VFX_ConveyorPush)
  - **Steps**: Check Content/RobotRally/VFX/VFX_ConveyorPush
  - **Expected**: Niagara or Cascade particle system exists

- [ ] **Test**: Conveyor VFX plays when robot pushed
  - **Steps**: Stand on conveyor, observe
  - **Expected**: Force/push effect as robot moves

#### 5.7 VFX Integration
- [ ] **Test**: All VFX integrated in C++ classes
  - **Steps**: Check RobotPawn, GridManager code
  - **Expected**: SpawnEmitterAtLocation() calls present

- [ ] **Test**: VFX doesn't cause performance drops
  - **Steps**: Trigger multiple VFX simultaneously
  - **Expected**: Maintains 60 FPS

- [ ] **Test**: VFX visible but not obstructive
  - **Steps**: Play game with all VFX
  - **Expected**: Effects add feedback without blocking view

---

### 6. Polish & Juice

#### 6.1 Camera Effects
- [ ] **Test**: Camera shake on death
  - **Steps**: Die, observe camera
  - **Expected**: Camera shakes violently on death

- [ ] **Test**: Camera shake on damage
  - **Steps**: Take damage, observe camera
  - **Expected**: Minor camera shake on hit

- [ ] **Test**: Camera smoothly follows robot
  - **Steps**: Move robot around grid
  - **Expected**: Camera tracks robot smoothly, not jittery

#### 6.2 Screen Effects
- [ ] **Test**: Screen flash on damage (optional)
  - **Steps**: Take damage, observe screen
  - **Expected**: Red flash or vignette on damage

- [ ] **Test**: Screen fade on death/respawn
  - **Steps**: Die and respawn, observe
  - **Expected**: Fade to black and back on death/respawn

- [ ] **Test**: Victory screen effect
  - **Steps**: Win game, observe
  - **Expected**: Confetti, particles, or celebratory effect

#### 6.3 UI Polish
- [ ] **Test**: UI animations are smooth
  - **Steps**: Select cards, watch animations
  - **Expected**: Smooth 60 FPS animations

- [ ] **Test**: UI transitions are polished
  - **Steps**: Change game states
  - **Expected**: Fade/slide transitions between states

- [ ] **Test**: Button hover effects (if mouse UI)
  - **Steps**: Hover over buttons
  - **Expected**: Buttons highlight or scale on hover

#### 6.4 Timing and Pacing
- [ ] **Test**: Card execution speed is appropriate
  - **Steps**: Execute 5-card sequence, time it
  - **Expected**: Not too fast (hard to follow) or too slow (boring)

- [ ] **Test**: Death/respawn timing feels right
  - **Steps**: Die, time respawn delay
  - **Expected**: ~2-3 seconds delay (enough to register, not too long)

- [ ] **Test**: Conveyor movement speed is appropriate
  - **Steps**: Watch conveyor push robot
  - **Expected**: Fast enough to be reactive, slow enough to see

---

### 7. Content Integration Testing

#### 7.1 Full Gameplay Loop
- [ ] **Test**: Tutorial level playable start to finish
  - **Steps**: Play tutorial from spawn to victory
  - **Expected**: All systems work, sounds/VFX play, win condition triggers

- [ ] **Test**: Easy level playable start to finish
  - **Steps**: Play easy level completely
  - **Expected**: Balanced difficulty, no bugs

- [ ] **Test**: Medium level playable start to finish
  - **Steps**: Play medium level completely
  - **Expected**: Challenging but fair

- [ ] **Test**: Hard level playable start to finish
  - **Steps**: Play hard level completely
  - **Expected**: Very challenging but winnable

#### 7.2 Content Variety
- [ ] **Test**: Each level feels distinct
  - **Steps**: Play all levels consecutively
  - **Expected**: Each level has unique layout and challenge

- [ ] **Test**: Levels use different hazard combinations
  - **Steps**: Compare levels
  - **Expected**: Variety in pit/laser/conveyor placements

#### 7.3 Performance with Full Content
- [ ] **Test**: Game maintains 60 FPS with all content
  - **Steps**: Play with all VFX, sounds, UI active
  - **Expected**: Stable 60 FPS

- [ ] **Test**: Load times are acceptable
  - **Steps**: Load each level, time it
  - **Expected**: <5 seconds to load level

- [ ] **Test**: Memory usage is reasonable
  - **Steps**: Play for 30 minutes, check memory
  - **Expected**: <2GB RAM usage, no memory leaks

---

### 8. Accessibility and Usability

#### 8.1 Visual Accessibility
- [ ] **Test**: Colorblind mode options (if implemented)
  - **Steps**: Enable colorblind filters
  - **Expected**: Hazards distinguishable by shape/icon, not just color

- [ ] **Test**: Tile types distinguishable in grayscale
  - **Steps**: Take screenshot, convert to grayscale
  - **Expected**: Can identify pit/laser/conveyor/checkpoint by shape

#### 8.2 Audio Accessibility
- [ ] **Test**: Game playable with sound off
  - **Steps**: Mute audio, play game
  - **Expected**: Still receive visual feedback for all events

- [ ] **Test**: Important audio has visual equivalent
  - **Steps**: Check damage, checkpoint, win/lose
  - **Expected**: Screen flash, UI message, or VFX accompanies sound

#### 8.3 Difficulty Accessibility
- [ ] **Test**: Tutorial explains all mechanics
  - **Steps**: Play tutorial as new player
  - **Expected**: Clear instructions for WASD, cards, hazards, checkpoints

- [ ] **Test**: Difficulty curve is smooth
  - **Steps**: Play levels in order
  - **Expected**: Each level slightly harder than previous

---

### 9. Content Organization and Maintenance

#### 9.1 Asset Organization
- [ ] **Test**: All assets follow project structure
  - **Steps**: Check Content Browser
  - **Expected**: /Maps/, /Blueprints/, /Data/, /Audio/, /VFX/ folders organized

- [ ] **Test**: All assets follow naming convention
  - **Steps**: Check asset names
  - **Expected**: Consistent prefixes (Map_, BP_, DT_, DA_, SFX_, VFX_)

- [ ] **Test**: No unused/orphaned assets
  - **Steps**: Use Reference Viewer, check for unused assets
  - **Expected**: All assets referenced by project

- [ ] **Test**: Asset thumbnails are clear
  - **Steps**: View assets in Content Browser
  - **Expected**: Can identify assets by thumbnail

#### 9.2 Documentation
- [ ] **Test**: Level design documentation exists
  - **Steps**: Check /docs/LEVEL_DESIGN.md
  - **Expected**: Document explaining level creation process

- [ ] **Test**: Asset guidelines documented
  - **Steps**: Check /docs/ASSET_GUIDELINES.md
  - **Expected**: Document explaining naming conventions, folder structure

- [ ] **Test**: Sound/VFX integration documented
  - **Steps**: Check code comments or docs
  - **Expected**: Clear instructions for adding new sounds/VFX

---

## Testing Procedures

### Manual Testing Steps

1. **Setup**: Open RobotRally.uproject in UE5 Editor
2. **Test Each Level**: Load Map_Tutorial, Map_EasyBoard, Map_MediumBoard, Map_HardBoard
3. **Play Full Sessions**: Play each level start to finish
4. **Check Audio**: Listen for all sound effects during gameplay
5. **Check VFX**: Observe all particle effects during gameplay
6. **Test Performance**: Monitor FPS with stat FPS command

### Testing Workflow

#### Test New Level
1. Open level in editor
2. Check grid size and layout
3. Count hazards (pits, lasers, conveyors)
4. Count checkpoints
5. Play in PIE
6. Verify spawn position
7. Verify checkpoint path
8. Play to victory
9. Check for bugs or softlocks

#### Test Sound Integration
1. Enable audio
2. Move robot → check movement sound
3. Rotate robot → check rotation sound
4. Take damage → check damage sound
5. Collect checkpoint → check checkpoint sound
6. Die → check death sound
7. Respawn → check respawn sound
8. Win → check victory sound

#### Test VFX Integration
1. Move robot → check movement trail
2. Take laser damage → check laser hit VFX
3. Die → check death explosion VFX
4. Respawn → check respawn teleport VFX
5. Collect checkpoint → check collection burst VFX
6. Stand on conveyor → check conveyor push VFX

### Performance Testing
```
// In-game console commands:
stat FPS          // Show framerate
stat Unit         // Show frame time breakdown
stat Memory       // Show memory usage
stat Particles    // Show particle counts
```

---

## Success Criteria

Phase 6 testing is considered **COMPLETE** when:

- ✅ All 200+ test cases above pass
- ✅ 4+ playable levels (Tutorial, Easy, Medium, Hard)
- ✅ All levels have valid layouts with reachable checkpoints
- ✅ Blueprint classes created for all major C++ classes
- ✅ DataTable/DataAsset system functional for grid layouts
- ✅ 15+ sound effects integrated and balanced
- ✅ 10+ visual effects integrated and performant
- ✅ Camera shake, screen effects, UI polish complete
- ✅ Game maintains 60 FPS with all content active
- ✅ Content organized in proper folder structure
- ✅ All assets follow naming conventions
- ✅ Game is fun, polished, and feels complete

---

## Known Issues & Limitations

No known limitations for Phase 6 — all items are pending implementation.

### Content Creation Guidelines

- **Sound Effects**: Use royalty-free SFX libraries (Freesound.org, OpenGameArt.org) or create with tools like Audacity, BFXR
- **Visual Effects**: Use UE5's Niagara system for modern particle effects, Cascade for simple effects
- **Level Design**: Start simple (tutorial), gradually increase complexity
- **Performance**: Profile regularly, optimize heavy VFX
- **Accessibility**: Use shape/icon differentiation, not just color

---

## Next Steps After Phase 6 Testing

1. Fix any bugs found during testing
2. Balance difficulty curve based on playtest feedback
3. Consider additional content: more levels, robot skins, alternate game modes
4. Prepare for release: packaging, distribution, marketing
5. Post-release support: bug fixes, content updates

---

## References

- [README.md](../../README.md) — Phase 6 implementation checklist
- [CLAUDE.md](../../CLAUDE.md) — Architecture and conventions
- [Phase 1-5 Testing Plans](./2026-02-08_phase1-testing-plan.md) — Previous phase tests
- Unreal Documentation:
  - Level Design: https://docs.unrealengine.com/5.7/en-US/level-design-in-unreal-engine/
  - Niagara VFX: https://docs.unrealengine.com/5.7/en-US/creating-visual-effects-in-niagara-for-unreal-engine/
  - Audio: https://docs.unrealengine.com/5.7/en-US/audio-in-unreal-engine/
  - DataTables: https://docs.unrealengine.com/5.7/en-US/using-data-tables-and-curves-in-unreal-engine/
- Content Folders:
  - `/Content/RobotRally/Maps/`
  - `/Content/RobotRally/Blueprints/`
  - `/Content/RobotRally/Data/`
  - `/Content/RobotRally/Audio/`
  - `/Content/RobotRally/VFX/`
