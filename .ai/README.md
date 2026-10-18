# Shared LLM resources

This directory holds project knowledge and reusable instructions independently of
an assistant vendor, editor, model, or plugin. The entry point is
[`AGENTS.md`](../AGENTS.md). It routes tasks here without requiring every resource
to be loaded into every session.

The organization is inspired by [ECC](https://github.com/affaan-m/ECC), which
separates skills, rules, workflows, and agent context. The contents here are
written for this C repository; no ECC runtime or installation is required.

## Layout

| Path | Purpose | When to read |
| --- | --- | --- |
| [context/repository-map.md](context/repository-map.md) | Exercise layout, interfaces, and data files | Finding the relevant code |
| [context/build-and-test.md](context/build-and-test.md) | Explicit compile, test, and smoke commands | Verifying work |
| [rules/c.md](rules/c.md) | Local C development conventions | Editing C code |
| [skills/add-c-exercise/SKILL.md](skills/add-c-exercise/SKILL.md) | Create an independent learning exercise | Adding a project |
| [skills/debug-c-program/SKILL.md](skills/debug-c-program/SKILL.md) | Reproduce, diagnose, and fix a C bug | Debugging a project |
| [workflows/review-change.md](workflows/review-change.md) | Review a patch against actual behavior | Reviewing changes |
| [prompts/explain-exercise.md](prompts/explain-exercise.md) | Reusable learning-session prompt | Explaining existing code |
| [templates/handoff.md](templates/handoff.md) | Session continuation notes | Recording unfinished work |

## Use with any assistant

If an assistant reads `AGENTS.md`, start there. Otherwise, include that file in
the session or ask the assistant to read it, then select a relevant resource:

```text
Read AGENTS.md and .ai/skills/debug-c-program/SKILL.md.
Reproduce and fix the reported bug in beginner/basic_calculator/.
```

Folder discovery and slash-command syntax depend on the host tool. This layout
does not install skills, enable hooks, configure MCP servers, or grant additional
permissions. All resources can be used by reading their files directly.

## Maintenance and memory

Keep rules concise, commands executable from their stated directory, and context
consistent with the code. A skill directory needs a `SKILL.md` with `name` and
`description` frontmatter; add scripts or references only when they support a real
workflow. Prefer links to a single maintained source over repeated instructions.

Use the handoff template for temporary session memory and save the filled note in
`.ai/local/`, which is ignored by Git. Move durable, verified project facts into
the appropriate context file. Keep credentials, private data, raw tool logs, and
machine-specific configuration out of committed guidance.
