# Agent Skills Map — QuickDeck

Which Cursor/Codex skills apply to this repo. Load the skill file before following its workflow.

## Always relevant

| Skill | Path | Use when |
|-------|------|----------|
| **writing-for-agents** | `~/.codex/skills/writing-for-agents/SKILL.md` | Editing `AGENTS.md`, docs, or project skills |
| **cpp-coding** | `~/.agents/skills/cpp-coding/SKILL.md` | New C++ features, refactors, non-trivial C++ review |
| **domain-modeling** | `~/.codex/skills/domain-modeling/SKILL.md` | Naming entities, updating `CONTEXT.md`, ADRs |
| **codebase-design** | `~/.codex/skills/codebase-design/SKILL.md` | Module seams, testability, deep-module refactors |

## Development workflow

| Skill | Use when |
|-------|----------|
| **tdd** | New service/repository logic with clear inputs/outputs |
| **diagnosing-bugs** | Runtime failures, performance regressions, flaky tests |
| **code-review** | Review branch/PR against standards + spec |
| **research** | Qt API, Windows shell, FTS — output to `docs/` |
| **prototype** | UI/layout experiments before committing QML |
| **implement** | Large multi-file feature with spec already written |

## Cursor-specific

| Skill | Use when |
|-------|----------|
| **review-bugbot** | Post-implementation diff review (standards) |
| **review-security** | Security review of local changes |
| **split-to-prs** | Splitting large phase work into reviewable PRs |
| **create-rule** | New `.cursor/rules` for QuickDeck conventions |
| **create-skill** | Project-specific skill under `.cursor/skills` |
| **canvas** | Architecture diagrams, UI mockups for user review |

## Git & repo

| Skill | Use when |
|-------|----------|
| **resolving-merge-conflicts** | Merge/rebase conflicts |
| **git-guardrails-claude-code** | Blocking dangerous git hooks (optional) |
| **new-repo / origin / share** | Remote setup — repo already on GitHub |

## Not applicable to QuickDeck V1

| Skill | Reason |
|-------|--------|
| migrate-to-shoehorn, scaffold-exercises | TypeScript/education |
| setup-pre-commit, setup-ts-deep-modules | Node/TS tooling |
| wechat-miniprogram-cloudbase, flutter | Wrong stack |
| sdk, automate, loop | Unless building Cursor automations for this project |
| grill-me, grilling, teach | Planning/discussion only — use manually |

## Five-Agent Phase Mode (project convention)

Used for Phase 2–3 delivery; document in `PHASE*.md`:

| # | Role | Skills to load |
|---|------|----------------|
| 1 | Planner | `domain-modeling`, `research`, this doc + `V1_SCOPE.md` |
| 2 | i18n/Theme | `I18N.md`, `UI_THEME.md`, `writing-for-agents` |
| 3 | Implementer | `cpp-coding`, `implement`, `CONTEXT.md` |
| 4 | Reviewer | `code-review`, `codebase-design`, ADRs |
| 5 | Tester | `tdd`, extend `tests/` |

### Anti-lazy rules

Copy from latest `PHASE3.md` — never skip i18n verification, never hardcode UI strings/colors, never stack all settings on one tab.

## Doc reading order for new agents

1. [AGENTS.md](../AGENTS.md) — conventions
2. [CONTEXT.md](../CONTEXT.md) — domain language
3. [ARCHITECTURE.md](ARCHITECTURE.md) — layers and debt
4. [PROJECT_STATUS.md](PROJECT_STATUS.md) — what's done
5. [V1_SCOPE.md](V1_SCOPE.md) — in/out of scope
6. Phase doc for current work (`PHASE2.md` / `PHASE3.md`)
7. [I18N.md](I18N.md) / [UI_THEME.md](UI_THEME.md) when touching UI
