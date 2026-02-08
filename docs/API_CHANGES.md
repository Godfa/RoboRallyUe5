# API Change Log

## Purpose
Documents all public API changes to help agents working on dependent systems stay synchronized.

## Rules
1. **Log ALL public API changes** (function signatures, class interfaces, exposed properties)
2. **Include migration guide** for breaking changes
3. **Reference the task and commit** that made the change
4. **Keep entries chronological** (newest first)

---

## 2026-02-08 - Example API Change

### ARobotPawn - Added Example Function
**Task**: TASK-000
**Commit**: `abc123def`
**Agent**: Agent-Example
**Type**: Addition (Non-breaking)

**Added**:
```cpp
// Header: RobotPawn.h
UFUNCTION(BlueprintCallable, Category = "Robot")
int32 GetExampleValue() const;
```

**Impact**:
- No breaking changes
- Other systems can optionally use this new function
- Returns example value for demonstration

**Migration**: N/A (addition only)

---

## API Change Entry Template

```markdown
## YYYY-MM-DD - Brief Description

### ClassName - What Changed
**Task**: TASK-XXX
**Commit**: `git-hash`
**Agent**: Agent-Name
**Type**: Addition / Modification / Removal / Breaking Change

**Before** (if modification/removal):
```cpp
// Old API
OldFunctionSignature();
```

**After** (if addition/modification):
```cpp
// New API
NewFunctionSignature();
```

**Impact**:
- Who is affected?
- What systems need updates?
- Is this breaking?

**Migration** (if breaking):
- Step 1: What to change
- Step 2: How to update calling code
- Step 3: Any data migration needed
```

---

## Breaking Changes

### Summary of All Breaking Changes

No breaking changes yet. This section will list all breaking API changes for quick reference.

---

## Change Categories

### Additions (Safe)
New functions, properties, or classes that don't affect existing code.

### Modifications (Potentially Breaking)
Changes to existing function signatures, parameter types, return values.

### Removals (Breaking)
Deleted functions, classes, or properties.

### Deprecations
APIs marked for future removal (provide alternative).

---

**Last Updated**: 2026-02-08
