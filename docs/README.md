# RobotRally Documentation Index

Welcome to the RobotRally UE5 project documentation!

---

## Documentation Overview

### 📘 For Players

**[User Guide](USER_GUIDE.md)** - Start here if you want to play the game
- Game rules and objectives
- Keyboard controls
- Tile types and card explanations
- Tips and strategies
- Troubleshooting common issues

### 🏗️ For Developers

**[Technical Documentation](TECHNICAL_DOCUMENTATION.md)** - Architecture and system design
- System overview and implementation status
- Class hierarchy and data flow
- Detailed class reference (GameMode, Robot, Grid, etc.)
- Game systems explained (cards, movement, tiles, health)
- Complete game flow walkthrough
- Testing guide and known limitations

**[API Reference](API_REFERENCE.md)** - Quick function and property lookup
- Core class APIs with code examples
- Enum and struct definitions
- Common implementation patterns
- Debugging helpers
- Performance tips
- Blueprint integration guide

**[Testing Plans](plans/README.md)** - Comprehensive testing documentation
- 740+ test cases across all 6 phases
- Phase-by-phase testing checklists
- Manual and automated testing procedures
- Performance and integration testing
- Success criteria and validation

### 📝 Project Management

**[CHANGELOG.md](../CHANGELOG.md)** - Version history
- Detailed changes per version
- Implementation status tracker
- Known issues and upcoming features

**[CLAUDE.md](../CLAUDE.md)** - AI assistant instructions
- Project overview for AI assistants
- Code conventions and patterns
- Build instructions
- Assistant roles (Architect, Developer, Reviewer)

---

## Quick Links

### I want to...

**...play the game**
→ [User Guide - Getting Started](USER_GUIDE.md#getting-started)

**...understand how the code works**
→ [Technical Documentation - Architecture](TECHNICAL_DOCUMENTATION.md#architecture)

**...add a new feature**
→ [API Reference - Common Patterns](API_REFERENCE.md#common-patterns)

**...see what's been implemented**
→ [CHANGELOG.md - Implementation Status](../CHANGELOG.md#implementation-status)

**...fix a bug**
→ [Technical Documentation - Known Limitations](TECHNICAL_DOCUMENTATION.md#known-limitations)

**...understand the card system**
→ [Technical Documentation - Card System](TECHNICAL_DOCUMENTATION.md#1-card-system)

**...learn keyboard controls**
→ [User Guide - Controls](USER_GUIDE.md#controls)

**...use the GridManager API**
→ [API Reference - AGridManager](API_REFERENCE.md#agridmanager)

**...understand movement interpolation**
→ [Technical Documentation - Movement System](TECHNICAL_DOCUMENTATION.md#2-movement-system)

**...see the class hierarchy**
→ [Technical Documentation - Architecture](TECHNICAL_DOCUMENTATION.md#class-hierarchy)

**...test a specific phase**
→ [Testing Plans - Phase Index](plans/README.md)

**...verify game quality**
→ [Testing Plans - Success Criteria](plans/README.md#success-criteria-overall-project)

---

## Documentation Structure

```
docs/
├── README.md                      ← You are here (index)
├── USER_GUIDE.md                  ← Player-facing guide
├── TECHNICAL_DOCUMENTATION.md     ← Architecture deep-dive
├── API_REFERENCE.md               ← Developer API quick reference
└── plans/                         ← Testing plans and feature designs
    ├── README.md                  ← Testing plans index
    ├── 2026-02-08_phase1-testing-plan.md
    ├── 2026-02-08_phase2-testing-plan.md
    ├── 2026-02-08_phase3-testing-plan.md
    ├── 2026-02-08_phase4-testing-plan.md
    ├── 2026-02-08_phase5-testing-plan.md
    └── 2026-02-08_phase6-testing-plan.md
```

---

## Document Conventions

### Code Examples

```cpp
// C++ code examples use this style
void Example()
{
    // Tab indentation, braces on same line for control flow
}
```

### UI Elements

- **Bold** for buttons, keys, or important terms
- `Code style` for file paths, class names, variables
- → Arrow for navigation instructions

### Status Indicators

- ✅ Complete
- ⚠️ Partial / In Progress
- ❌ Not Started
- 🐛 Known Bug

---

## Contributing to Documentation

### When to Update Docs

- **After implementing a feature**: Update CHANGELOG.md and relevant docs
- **When API changes**: Update API_REFERENCE.md
- **When fixing bugs**: Update Technical Documentation known issues
- **When adding controls**: Update User Guide controls section

### Documentation Quality Standards

1. **Code examples must compile** - Test all example code
2. **Keep examples simple** - Focus on one concept per example
3. **Use real class/function names** - Match actual codebase
4. **Update cross-references** - Fix broken links when moving content
5. **Version changes** - Add version tags to major updates

### Style Guide

- **Headings**: Sentence case (not Title Case)
- **Code**: Use \`backticks\` for inline code
- **Links**: Use relative paths (e.g., `[link](OTHER.md)`)
- **Tables**: Use for structured data (specs, comparisons)
- **Lists**: Use for steps, features, or unordered items

---

## Version Information

- **Current Version**: 0.4.0 (Phase 3 Complete)
- **Last Documentation Update**: 2026-02-08
- **Unreal Engine**: 5.7
- **Documentation Format**: GitHub-Flavored Markdown

---

## External Resources

### Unreal Engine Documentation
- [UE5 C++ API Reference](https://docs.unrealengine.com/5.7/en-US/API/)
- [Actor Lifecycle](https://docs.unrealengine.com/5.7/en-US/unreal-engine-actor-lifecycle/)
- [Component Documentation](https://docs.unrealengine.com/5.7/en-US/components-in-unreal-engine/)

### Robot Rally Game Rules
- [Official Robot Rally Rules (PDF)](https://www.avalongames.com) - Original board game rules
- [BoardGameGeek: RobotRally](https://boardgamegeek.com/boardgame/18/roborally) - Community discussions

### C++ and Unreal Best Practices
- [Unreal Engine Coding Standard](https://docs.unrealengine.com/5.7/en-US/epic-cplusplus-coding-standard-for-unreal-engine/)
- [Game Programming Patterns](https://gameprogrammingpatterns.com/) - Design patterns for games

---

## Getting Help

### For Players
- Check [User Guide - Troubleshooting](USER_GUIDE.md#troubleshooting)
- Review [Technical Documentation - Known Limitations](TECHNICAL_DOCUMENTATION.md#known-limitations)

### For Developers
- Read [API Reference](API_REFERENCE.md) for usage examples
- Check [CHANGELOG.md](../CHANGELOG.md) for recent changes
- Search GitHub Issues (if available)

### Report Issues
- Check existing issues first
- Include error messages and logs
- Describe steps to reproduce
- Mention your UE version and OS

---

**Happy developing!** 🤖✨

*Documentation maintained by the Robot Rally Team*
