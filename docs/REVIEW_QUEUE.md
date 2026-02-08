# Code Review Queue

## Purpose
Tracks code review requests for work ready to merge to master.

## Rules
1. **Create review request** when work is complete and builds successfully
2. **Choose review type**: Quick Review (small changes) or Full Review (major features)
3. **Address all feedback** before merging
4. **Update status** as review progresses
5. **User has final approval** for all merges to master

---

## Review Status Values
- `Pending` - Awaiting reviewer assignment
- `In Review` - Being reviewed
- `Changes Requested` - Feedback provided, waiting for fixes
- `Approved` - Ready to merge
- `Merged` - Completed and merged to master

---

## Active Review Requests

### REVIEW-001: Example feature [Pending - Quick Review]
**Submitted By**: Agent-Example
**Date**: 2026-02-08
**Task**: TASK-000
**Branch**: `feature/example`
**Review Type**: Quick Review
**Status**: Pending

**Changes**:
- Added new example function
- Updated documentation

**Files Modified**:
- `Source/RobotRally/ExampleFile.cpp`
- `Source/RobotRally/ExampleFile.h`

**Build Status**: ✅ Builds without warnings
**Quick Review Checklist**:
- [x] Code compiles without warnings
- [x] Follows Unreal naming conventions
- [x] No breaking changes to existing APIs
- [x] ACTIVE_TASKS.md updated

**Reviewer Notes**:
- (Reviewer adds feedback here)

---

## Completed Reviews

### REVIEW-000: Example completed review [Merged]
**Submitted By**: Agent-Example
**Reviewed By**: User
**Date Submitted**: 2026-02-07
**Date Approved**: 2026-02-07
**Merged in Commit**: `abc123def`
**Outcome**: Approved with minor suggestions

---

## Review Type Guidelines

### Quick Review (for small changes)
Use for:
- Bug fixes (<50 lines changed)
- Documentation updates
- Minor refactoring
- Single-file changes

**Checklist**:
- [ ] Code compiles without warnings
- [ ] Follows Unreal naming conventions (A/U/F/E prefixes)
- [ ] No breaking changes to existing APIs
- [ ] ACTIVE_TASKS.md updated
- [ ] No new memory safety issues

**Expected Review Time**: <30 minutes

---

### Full Review (for major features)
Use for:
- New systems or components
- Multi-file changes (>3 files)
- Architecture changes
- Public API modifications

**Checklist**:
- [ ] All Quick Review items
- [ ] Design aligns with project architecture (see CLAUDE.md)
- [ ] No duplicate functionality
- [ ] Tests/verification steps documented
- [ ] Memory safety verified (UPROPERTY usage, no raw pointers to UObjects)
- [ ] Documentation updated (README.md, CLAUDE.md, technical docs)
- [ ] CHANGELOG.md updated
- [ ] API_CHANGES.md updated (if applicable)
- [ ] Handles edge cases appropriately
- [ ] No performance regressions

**Expected Review Time**: 1-2 hours

---

## Creating a Review Request

### Quick Review Template

```markdown
### REVIEW-XXX: Brief description [Pending - Quick Review]
**Submitted By**: Agent-Name
**Date**: YYYY-MM-DD
**Task**: TASK-XXX
**Branch**: `feature/branch-name`
**Review Type**: Quick Review
**Status**: Pending

**Changes**:
- Change 1
- Change 2

**Files Modified**:
- `path/to/file1.cpp`
- `path/to/file2.h`

**Build Status**: ✅ Builds without warnings
**Quick Review Checklist**:
- [x] Code compiles without warnings
- [x] Follows Unreal naming conventions
- [x] No breaking changes to existing APIs
- [x] ACTIVE_TASKS.md updated

**Reviewer Notes**:
- (Reviewer adds feedback here)
```

### Full Review Template

```markdown
### REVIEW-XXX: Brief description [Pending - Full Review]
**Submitted By**: Agent-Name
**Date**: YYYY-MM-DD
**Task**: TASK-XXX
**Branch**: `feature/branch-name`
**Review Type**: Full Review
**Status**: Pending

**Changes**:
- Major change 1
- Major change 2

**Files Modified**:
- `path/to/file1.cpp` (150 lines changed)
- `path/to/file2.h` (new file)
- `path/to/file3.cpp` (50 lines changed)

**Build Status**: ✅ Builds without warnings

**Full Review Checklist**:
- [x] Code compiles without warnings
- [x] Follows Unreal naming conventions
- [x] No breaking changes to existing APIs
- [x] ACTIVE_TASKS.md updated
- [x] Design aligns with project architecture
- [x] No duplicate functionality
- [x] Tests/verification steps documented
- [x] Memory safety verified
- [x] Documentation updated
- [x] CHANGELOG.md updated
- [x] API_CHANGES.md updated (if applicable)

**Verification Steps**:
1. Step to verify feature works
2. Step to test edge cases
3. Step to confirm no regressions

**API Changes**:
- List any public API changes or mark "None"

**Reviewer Notes**:
- (Reviewer adds feedback here)
```

---

**Last Updated**: 2026-02-08
