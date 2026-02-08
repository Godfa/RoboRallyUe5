# Parallel AI Development Coordination Strategy

## Context
Setting up a coordination system for multiple Claude AI assistants to work on the RobotRally UE5 project simultaneously. Goal is to prevent conflicts, duplicate work, and ensure smooth integration of parallel development streams.

## Requirements
- Support multiple AI assistants working on different features concurrently
- Clear task assignment to prevent duplicate work
- Documentation system for communicating changes between agents
- Git workflow that minimizes merge conflicts
- Code review process adapted for AI development

## Proposed Strategy

### 1. Task Assignment & Tracking System

**Create Task Registry File**: `docs/ACTIVE_TASKS.md`
- Simple markdown file listing all active development tasks
- Each task has: ID, Description, Assigned Agent, Status, Branch, Dependencies
- Agents MUST check and update this file before starting work

**Task Status Workflow**:
```
Available → Claimed (agent_name) → In Progress → Review → Complete
```

**Example Format**:
```markdown
## Active Tasks

### TASK-001: Implement multi-robot support [In Progress - Agent-Alpha - branch: feature/multi-robot]
- Owner: Agent-Alpha
- Started: 2026-02-08
- Dependencies: None
- Blocks: TASK-002
- Files: RobotRallyGameMode.cpp, RobotPawn.h

### TASK-002: Priority-based execution [Available]
- Dependencies: TASK-001
- Estimated files: RobotRallyGameMode.cpp
```

### 2. Git Workflow for Parallel AI Development

**Branch Strategy**:
- `master` - stable, always buildable
- `feature/<feature-name>` - one branch per major feature
- `fix/<fix-name>` - for bug fixes
- Agent creates branch immediately when claiming task

**Coordination Rules**:
1. Each agent works on own feature branch
2. Pull from master before starting work
3. Commit frequently with descriptive messages
4. Push to remote regularly (at least daily)
5. Never force-push to shared branches

**Merge Strategy**:
- Small changes: Direct merge to master (after review)
- Large features: Create PR, request review from user/another agent
- Conflicts: Always resolve in favor of master, then re-implement feature changes

### 3. Documentation & Communication System

**Change Log File**: `docs/AGENT_CHANGELOG.md`
- Each agent logs changes after each commit
- Format: Date, Agent ID, Files Changed, What Changed, Why

**Example**:
```markdown
## 2026-02-08 - Agent-Alpha
**Branch**: feature/multi-robot
**Commit**: a1b2c3d
**Files**: RobotRallyGameMode.cpp, RobotPawn.h
**Changes**: Added support for spawning multiple robots
**Rationale**: Phase 2 requirement for priority-based execution
**Impact**: Other agents working on GameMode should pull latest
```

**API Changes File**: `docs/API_CHANGES.md`
- Document all public API changes (function signatures, class interfaces)
- Critical for agents working on dependent systems

### 4. Code Review Process

**Review Requirements**:
- All changes reviewed before merge to master
- Two types: Quick Review (small changes) vs Full Review (major features)

**Quick Review Checklist** (for small changes):
- [ ] Code compiles without warnings
- [ ] Follows Unreal naming conventions
- [ ] No breaking changes to existing APIs
- [ ] ACTIVE_TASKS.md updated

**Full Review Checklist** (for major features):
- [ ] All Quick Review items
- [ ] Design aligns with project architecture
- [ ] No duplicate functionality
- [ ] Tests/verification steps documented
- [ ] Memory safety (no raw pointers, proper UPROPERTY usage)
- [ ] Documentation updated (README, CLAUDE.md)

**Review Process**:
1. Agent completes work, runs build verification
2. Agent creates review request in `docs/REVIEW_QUEUE.md`
3. User or designated reviewer checks code
4. Feedback provided via comments in review file
5. Agent addresses feedback, updates review request
6. Approved changes merged to master

### 5. Conflict Prevention

**File Ownership Registry**: `docs/FILE_OWNERSHIP.md`
- Lists which files are currently being modified by which agent
- Check before starting work on any file
- Update when claiming/releasing files

**Hot Files** (high conflict risk):
- `RobotRallyGameMode.cpp` - Core game logic
- `RobotPawn.cpp` - Robot behavior
- `GridManager.cpp` - Grid system

**Coordination for Hot Files**:
- Only one agent modifies at a time
- Must claim file in FILE_OWNERSHIP.md
- Complete work and merge within 24 hours or release claim
- Other agents work on different files or wait

### 6. Implementation Plan

**Phase 1: Setup Infrastructure** (30 minutes)
- Create coordination files:
  - `docs/ACTIVE_TASKS.md`
  - `docs/AGENT_CHANGELOG.md`
  - `docs/FILE_OWNERSHIP.md`
  - `docs/REVIEW_QUEUE.md`
  - `docs/API_CHANGES.md`
- Add templates and examples to each file
- Commit infrastructure to master

**Phase 2: Define Initial Tasks** (15 minutes)
- Break down remaining work into discrete tasks
- Add to ACTIVE_TASKS.md with dependencies
- Prioritize tasks by dependencies

**Phase 3: Test Workflow** (user validates)
- Simulate with 2 agents on separate small tasks
- Verify coordination files work as intended
- Adjust based on learnings

## Critical Files to Create
- `docs/ACTIVE_TASKS.md` - Task registry
- `docs/AGENT_CHANGELOG.md` - Change communications
- `docs/FILE_OWNERSHIP.md` - File lock registry
- `docs/REVIEW_QUEUE.md` - Review requests
- `docs/API_CHANGES.md` - API change log

## Verification Steps
1. Create all coordination files with templates
2. Add current outstanding tasks to ACTIVE_TASKS.md
3. Test with two AI agents:
   - Agent A claims TASK-001, creates branch
   - Agent B claims TASK-002 (dependent on TASK-001)
   - Agent A completes work, updates changelog
   - Agent B pulls changes, continues work
   - Both agents create review requests
4. Verify no merge conflicts when integrating
5. Check that all documentation files are kept current

## Notes
- Keep coordination files simple and text-based
- Agents should check ACTIVE_TASKS.md at start of every session
- User acts as final arbiter for conflicts/reviews
- System designed to be lightweight - avoid over-coordination
- Can scale up to 3-4 parallel agents comfortably
