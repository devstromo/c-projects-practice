---
name: add-c-exercise
description: Add an independent C learning project to this repository, with an appropriate difficulty, build instructions, and focused verification.
---

# Add a C exercise

Read the root `AGENTS.md`, [repository map](../../context/repository-map.md), and
[C rules](../../rules/c.md). Use the requested problem and difficulty; infer a
reasonable location from similar exercises if the user has not specified one.
Current categories are `beginner/`, `intermediate/`, and `misc/`.

Create a descriptively named `snake_case` project directory. Use `main.c` for a
new standalone program unless the task supplies another convention. Keep the
project independent of other exercises. Introduce an implementation/header pair
when there is reusable logic worth testing separately, following the calculator
or Bloom filter pattern.

Keep the solution readable at its intended difficulty. Define input constraints,
exit behavior, and resource ownership as needed. For file-based projects, document
the runtime working directory and fixture format. Ensure required fixtures are
tracked; the root ignore rules currently exclude new CSV files by default.

Compile and run using [the build guide](../../context/build-and-test.md). Choose
verification that demonstrates the main behavior and a meaningful boundary case.
Use a separate test executable for extracted logic; never link its `main` with
the demo's `main`. Keep generated files under `build/` or in temporary storage.

Update the repository map and add any project-specific compile/run command needed
to reproduce the result. Report the learning objective, files added, commands run,
and outcomes. Avoid adding a shared framework or build system unless requested.
