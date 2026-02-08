# File Ownership Registry

## Purpose
Tracks which files are currently being modified by which agent to prevent merge conflicts.

## Rules
1. **Check this file BEFORE modifying any file**
2. **Claim files** by adding them to "Currently Claimed" section with your agent name
3. **Release files** within 24 hours or when work is complete
4. **Hot files** (listed below) require strict single-agent ownership
5. **Stale claims** (>48 hours) can be challenged - contact user

---

## Currently Claimed Files

### Agent-Name - 2026-02-08
**Branch**: `feature/example`
**Task**: TASK-000
**Claimed Until**: 2026-02-09
**Files**:
- `Source/RobotRally/ExampleFile.cpp` - Adding new feature
- `Source/RobotRally/ExampleFile.h` - Header updates

---

## Hot Files (High Conflict Risk)

These files are central to multiple systems and should only be modified by one agent at a time:

### Core Game Logic
- `Source/RobotRally/RobotRallyGameMode.h`
- `Source/RobotRally/RobotRallyGameMode.cpp`
- **Current Owner**: None
- **Why Hot**: Central game state, execution loop, all game phases

### Robot Behavior
- `Source/RobotRally/RobotPawn.h`
- `Source/RobotRally/RobotPawn.cpp`
- **Current Owner**: None
- **Why Hot**: Robot movement, health, card execution

### Grid System
- `Source/RobotRally/GridManager.h`
- `Source/RobotRally/GridManager.cpp`
- **Current Owner**: None
- **Why Hot**: Tile management, coordinate system, tile effects

### Movement System
- `Source/RobotRally/RobotMovementComponent.h`
- `Source/RobotRally/RobotMovementComponent.cpp`
- **Current Owner**: None
- **Why Hot**: Movement interpolation, grid navigation

---

## Safe for Parallel Modification

These files/categories are generally safe for multiple agents to work on simultaneously:

### Documentation
- All files in `docs/` folder (use descriptive commits)
- `README.md`
- `CHANGELOG.md`

### New Features
- New .cpp/.h files being created (not modifying existing)
- Blueprint assets (if added)
- Content/Materials/Meshes (if added)

### Configuration
- `Config/DefaultInput.ini` - Different sections can be modified
- `RobotRally.Build.cs` - Adding dependencies usually safe

---

## Claiming Process

To claim files:
1. Check "Currently Claimed Files" section
2. Check "Hot Files" section for current owner
3. If unclaimed, add entry under "Currently Claimed Files"
4. Commit the updated FILE_OWNERSHIP.md
5. Pull latest and verify no one else claimed in parallel
6. Proceed with work

To release files:
1. Complete work and merge to master OR
2. Remove your claim entry from this file
3. Commit the updated FILE_OWNERSHIP.md

---

## Claim Entry Template

```markdown
### Agent-Name - YYYY-MM-DD
**Branch**: `feature/branch-name`
**Task**: TASK-XXX
**Claimed Until**: YYYY-MM-DD (max 48 hours from claim)
**Files**:
- `path/to/file1.cpp` - Brief reason
- `path/to/file2.h` - Brief reason
```

---

**Last Updated**: 2026-02-08
