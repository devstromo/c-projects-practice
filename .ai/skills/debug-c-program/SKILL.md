---
name: debug-c-program
description: Reproduce and fix compiler, input-handling, memory, or runtime bugs in this repository's C exercises, with verification scoped to the affected program.
---

# Debug a C program

Read root `AGENTS.md` and the affected source, header, fixture, and test files.
Consult [the repository map](../../context/repository-map.md) for runtime data
and [the build guide](../../context/build-and-test.md) for the exact executable's
source list. Check for unrelated local changes before editing.

Reproduce the reported behavior with the smallest useful input. Record the
compiler command, working directory, expected result, actual result, and exit
status. Check compiler diagnostics before treating a runtime symptom as the
cause. Use a timeout for interactive reproducers that may loop at EOF.

For persistent-data bugs, use an isolated directory and synthetic data. Copy
required fixtures there. For memory or undefined-behavior symptoms, use the
optional sanitizer flags in the build guide if available. Do not infer permission
to use real records or invoke download commands from a debugging request.

Trace the failing value through input conversion, bounds, ownership, arithmetic,
or file operations as relevant. Check both the public API and its callers before
changing sentinel values, return types, or storage formats. Apply a focused fix
following [the C rules](../../rules/c.md).

Run the original reproducer and relevant existing tests. Add a focused regression
case when practical; for a standalone interactive program, a repeatable input
transcript with expected behavior may be sufficient. Report the cause, behavioral
change, validation results, and any remaining uncertainty. Separate pre-existing
warnings from newly introduced diagnostics.
