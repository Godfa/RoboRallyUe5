# Active Tasks Registry

## Purpose
This file tracks all active development tasks to prevent duplicate work and coordinate parallel AI development efforts.

## Rules
1. **Check this file BEFORE starting any work**
2. **Claim a task** by changing status to "Claimed" and adding your agent name
3. **Update status** as work progresses
4. **Mark complete** when merged to master
5. **Add new tasks** as they are identified

## Task Status Values
- `Available` - Ready to start, dependencies met
- `Blocked` - Waiting on dependencies
- `Claimed (Agent-Name)` - Reserved by an agent
- `In Progress (Agent-Name)` - Active development
- `Review` - Awaiting code review
- `Complete` - Merged to master

---

## Active Tasks

### TASK-001: Implement multi-robot support [Available]
- **Owner**: Unassigned
- **Priority**: High
- **Status**: Available
- **Branch**: N/A
- **Dependencies**: None
- **Blocks**: TASK-002
- **Estimated Files**: `RobotRallyGameMode.h`, `RobotRallyGameMode.cpp`, `RobotPawn.h`
- **Description**: Extend GameMode to manage multiple robot actors. Add robot spawning system, track active robots in TArray, update game loop to iterate over all robots.
- **Acceptance Criteria**:
  - [ ] GameMode can spawn N robots at specified grid positions
  - [ ] All robots tracked in central collection
  - [ ] Game loop processes all robots
  - [ ] Builds without warnings

### TASK-002: Priority-based card execution [Blocked]
- **Owner**: Unassigned
- **Priority**: High
- **Status**: Blocked
- **Branch**: N/A
- **Dependencies**: TASK-001
- **Blocks**: None
- **Estimated Files**: `RobotRallyGameMode.cpp`
- **Description**: Implement Phase 2 priority-based execution. Sort all robot cards by priority, execute in order, handle card resolution conflicts.
- **Acceptance Criteria**:
  - [ ] Cards execute in priority order across all robots
  - [ ] Higher priority cards resolve first
  - [ ] Execution visualized correctly
  - [ ] Builds without warnings

### TASK-003: Implement rotating laser tiles [Available]
- **Owner**: Unassigned
- **Priority**: Medium
- **Status**: Available
- **Branch**: N/A
- **Dependencies**: None
- **Blocks**: None
- **Estimated Files**: `GridManager.h`, `GridManager.cpp`, `RobotRallyGameMode.cpp`
- **Description**: Add rotating laser tiles that fire in a specific direction, damage robots in line of sight until hitting wall or robot.
- **Acceptance Criteria**:
  - [ ] New ETileType values for directional lasers
  - [ ] Laser raycast logic implemented
  - [ ] Visual representation of laser beams
  - [ ] Damage applied correctly
  - [ ] Builds without warnings

---

## Completed Tasks

### TASK-000: Example completed task [Complete]
- **Owner**: Agent-Example
- **Completed**: 2026-02-08
- **Branch**: `feature/example`
- **Merged in commit**: `abc123d`
- **Description**: Example of a completed task for reference

---

## How to Add a New Task

```markdown
### TASK-XXX: Short descriptive title [Status]
- **Owner**: Agent-Name or Unassigned
- **Priority**: High/Medium/Low
- **Status**: Available/Blocked/Claimed/In Progress/Review/Complete
- **Branch**: feature/branch-name or N/A
- **Dependencies**: TASK-001, TASK-002 or None
- **Blocks**: TASK-005 or None
- **Estimated Files**: File1.cpp, File2.h
- **Description**: Detailed description of what needs to be done
- **Acceptance Criteria**:
  - [ ] Criterion 1
  - [ ] Criterion 2
  - [ ] Builds without warnings
```

---

**Last Updated**: 2026-02-08
