# C development rules

## Keep exercises approachable

- Keep the difficulty and learning objective of the exercise intact. Prefer
  straightforward control flow and standard-library facilities.
- Match local formatting and naming. Existing files use different brace styles
  and a mix of camelCase and snake_case; avoid repository-wide normalization.
- Explain non-obvious pointer, ownership, algorithm, or file-format decisions.
  Comments should clarify intent instead of restating statements.
- Keep each exercise independently buildable. Separate reusable logic from
  `main` when it enables useful tests; keep declarations in guarded headers.

## Handle C boundaries explicitly

- Check conversion results and handle invalid input and EOF deliberately. Bound
  string reads and ensure null termination. When changing input handling, avoid
  retry loops that cannot consume invalid input or terminate at EOF.
- Check indices and counts before accessing arrays. Consider empty input, full
  capacity, negative values, and allocation-size arithmetic where relevant.
- Check allocation results, document who frees returned memory, and release
  resources on every affected exit path. Use `size_t` for sizes and `%zu` when
  printing it.
- Check file opens, reads, writes, and replacement operations. Preserve existing
  data on failure and keep format changes explicit, particularly the binary
  student records and CSV-based bank/library data.
- Avoid introducing undefined behavior or relying on platform-specific pointer
  conversions. When a `misc/` experiment illustrates such a conversion, explain
  the limitation in context.

## Verify behavior proportionally

Use the commands in [build-and-test.md](../context/build-and-test.md). Keep demo
and test entry points separate. Add focused assertions for deterministic logic
when useful; a new framework is usually unnecessary. Keep tests for random or
probabilistic behavior tied to actual invariants, not one lucky output.

Compiler warnings identify work to investigate; they do not authorize unrelated
cleanup. Report existing warnings and confine fixes to the requested scope.
