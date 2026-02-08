# Agent Development Changelog

## Purpose
This file logs all changes made by AI agents to communicate work between parallel development sessions.

## Rules
1. **Add entry AFTER each commit**
2. **Include enough detail** for other agents to understand changes
3. **Highlight breaking changes** or API modifications
4. **Reference related task ID** from ACTIVE_TASKS.md
5. **Keep entries chronological** (newest first)

---

## 2026-02-08 - Agent-Example
**Task**: TASK-000
**Branch**: `feature/example`
**Commit**: `abc123def456`
**Files Modified**:
- `Source/RobotRally/ExampleClass.h`
- `Source/RobotRally/ExampleClass.cpp`

**Changes**:
- Added new function `CalculateExample()` to ExampleClass
- Modified constructor to initialize new member variable
- Updated header documentation

**Rationale**:
Example entry showing expected format

**Impact**:
- No breaking changes
- Other agents can safely pull this branch
- No API changes

**Notes**:
This is a template entry for reference

---

## Changelog Entry Template

```markdown
## YYYY-MM-DD - Agent-Name
**Task**: TASK-XXX
**Branch**: `feature/branch-name`
**Commit**: `git-commit-hash`
**Files Modified**:
- `path/to/file1.cpp`
- `path/to/file2.h`

**Changes**:
- Bullet point of change 1
- Bullet point of change 2

**Rationale**:
Why these changes were made

**Impact**:
- Breaking changes? (YES/NO - describe if yes)
- Which systems affected?
- Should other agents pull these changes?

**Notes**:
Any additional context or warnings
```

---

**Last Updated**: 2026-02-08
