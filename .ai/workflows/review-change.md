# Review a C change

Use this workflow when asked to review a diff. Read `AGENTS.md` and the relevant
sources, callers, tests, and fixtures. Review against the requested behavior and
actual patch scope; use the working-tree diff unless another base is specified.
Reviewing does not itself call for editing the code.

1. Identify affected executables and interfaces. Check explicit source lists,
   separate demo/test entry points, math-library linkage, and runtime paths using
   [the build guide](../context/build-and-test.md).
2. Trace changed behavior through callers. Check input conversion and EOF,
   array/string bounds, allocation ownership, arithmetic edge cases, file errors,
   and compatibility of persistent formats where touched.
3. Check exercise-specific invariants. For example, a Bloom filter may return
   false positives; BFS should find a shortest path; bank mutations must preserve
   the intended balance and ledger behavior. Confirm the precise contract in code
   and the task before judging a change.
4. Run relevant tests or a bounded reproducer if available. Use disposable data
   for persistence checks. Distinguish a passing compile from runtime validation.
5. Report actionable findings with file/line references, a concrete trigger, and
   the observed or supported consequence. Prioritize correctness over formatting.
   Label uncertainty and distinguish existing bugs from regressions.

If no actionable issue is found, say so and identify checks performed and material
coverage gaps. Do not turn a small review into an unsolicited repository rewrite.
