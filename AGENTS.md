# Repository guidance for coding agents

## Purpose and scope

This is a collection of small C learning projects, not a single application.
Keep solutions understandable to someone learning C. Favor focused changes and
explain the C concept or bug involved. Preserve the exercise's intent and existing
behavior unless the task calls for changing them.

## Start here

- Inspect the relevant exercise, its headers, fixtures, and tests before editing.
- Check `git status --short`; preserve unrelated edits and local files.
- Use the actual directories: `beginner/`, `intermediate/`, and `misc/`.
- Build each exercise independently. Many files define `main`; do not link all
  sources together or combine a demo entry point with a test entry point.
- There is currently no shared build system, dependency manager, CI workflow, or
  repository-wide test runner. Do not assume `make`, CMake, or npm commands exist.

Read [the repository map](.ai/context/repository-map.md) for project-specific
context and [build and test commands](.ai/context/build-and-test.md) before
compiling or running a program.

## Implementation conventions

Apply [the C rules](.ai/rules/c.md) when changing C code. Match the surrounding
file's brace style and naming; styles currently vary between exercises. Prefer
standard-library solutions and small functions. Add headers and implementation
files when separating reusable logic from an executable is useful, following the
calculator and Bloom filter examples.

For new or changed code, check input conversion, bounds, allocation, and file I/O
results. Define ownership of allocated memory and close files on error paths.
Keep existing public interfaces and on-disk formats compatible unless their change
is part of the requested task. Avoid unrelated reformatting or framework changes.

## Verification

- Compile the affected executable with `cc -std=c11 -Wall -Wextra -Wpedantic` as
  a starting point. These are recommended flags, not an existing enforced policy.
- Use explicit source lists and put binaries under the ignored root `build/`.
- Link `-lm` after sources for the calculator and Bloom filter.
- Run the relevant existing tests. Calculator tests use `assert`, so keep
  assertions enabled; do not compile them with `-DNDEBUG`.
- For exercises without tests, run a bounded smoke check with complete input,
  including the exit choice. Add focused regression coverage for behavior fixes
  when practical; documentation-only work needs no new C tests.
- Run programs that create data in an isolated working directory under `build/`
  or a temporary directory. Relative filenames resolve against the process's
  working directory, not the source or binary directory.
- Report commands run, outcomes, and relevant warnings or checks not performed.
  Distinguish existing issues from regressions; compilation alone is not proof
  that input handling or runtime behavior is correct.

## Shared LLM resources

[`.ai/README.md`](.ai/README.md) indexes the tool-agnostic resources. Load only what
the current task needs:

- [Add a C exercise](.ai/skills/add-c-exercise/SKILL.md) for new projects.
- [Debug a C program](.ai/skills/debug-c-program/SKILL.md) for reproducing and
  fixing compile errors or runtime bugs.
- [Review a change](.ai/workflows/review-change.md) for code reviews.
- [Explain an exercise](.ai/prompts/explain-exercise.md) for learning sessions.
- [Handoff template](.ai/templates/handoff.md) for continuing work across sessions.

The skills are plain Markdown with `name` and `description` frontmatter. They can
be read directly even when the current assistant does not discover this directory
automatically. Keep shared instructions in `.ai/`; any future tool-specific
adapter should reference this file and the shared resources instead of duplicating
them. User instructions take precedence over this repository guidance.
