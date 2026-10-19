# Build and test

Run these commands from the repository root in a POSIX shell. Prerequisites are
a C compiler available as `cc` (Clang or GCC) and the standard C library. The
calculator and Bloom filter also use the math library. There are no third-party
C dependencies or shared build scripts. The token bucket's optional threaded
programs use POSIX threads and require `-pthread`.

Use C11 and warnings as a starting point; the repository does not currently
enforce a language standard or a zero-warning build. Existing warnings should be
reported separately from new ones. Do not add `-Werror` to baseline checks.

```sh
mkdir -p build
```

## A single-file program

```sh
cc -std=c11 -Wall -Wextra -Wpedantic \
  beginner/temperature_converter/main.c -o build/temperature_converter
printf '1\n0\n7\n' | ./build/temperature_converter
```

The smoke check should print `32.00` Fahrenheit, then exit. Most other projects
are single-file programs too; use the entry point in the repository map rather
than assuming every program is named `main.c`.

## Calculator demo and tests

```sh
cc -std=c11 -Wall -Wextra -Wpedantic \
  beginner/basic_calculator/basic_calculator.c \
  beginner/basic_calculator/calculator_main.c -lm -o build/calculator
printf '5\n3\n1\n' | ./build/calculator

cc -std=c11 -Wall -Wextra -Wpedantic \
  beginner/basic_calculator/basic_calculator.c \
  beginner/basic_calculator/test_calculator.c -lm -o build/test_calculator
./build/test_calculator
```

The demo should print `8.00`. The tests should exit successfully and print
`All tests passed!`. Keep assertions enabled: `-DNDEBUG` disables their checks.
Current tests capture division-by-zero and invalid-operation return values;
inspect both API and CLI behavior before changing either contract.

## Bloom filter demo and tests

```sh
cc -std=c11 -Wall -Wextra -Wpedantic \
  intermediate/bloom_filter/bloom_filter.c \
  intermediate/bloom_filter/main.c -lm -o build/bloom_filter
./build/bloom_filter --demo

cc -std=c11 -Wall -Wextra -Wpedantic \
  intermediate/bloom_filter/bloom_filter.c \
  intermediate/bloom_filter/test_bloom_filter.c -lm -o build/test_bloom_filter
./build/test_bloom_filter
```

The test runner currently reports 11 tests and returns nonzero if a test fails.
Invalid-parameter tests intentionally print error messages to stderr. A positive
membership result means an item *may* exist; tests must allow false positives.
Inserted items must not produce false negatives under successful operation.

## Token bucket demo and tests

```sh
cc -std=c11 -Wall -Wextra -Wpedantic \
  intermediate/token_bucket/token_bucket.c \
  intermediate/token_bucket/main.c -o build/token_bucket
./build/token_bucket
./build/token_bucket --live

cc -std=c11 -Wall -Wextra -Wpedantic \
  intermediate/token_bucket/token_bucket.c \
  intermediate/token_bucket/test_token_bucket.c -o build/test_token_bucket
./build/test_token_bucket
```

The default demo uses a deterministic timeline; `--live` uses POSIX monotonic time
and takes about three seconds on macOS/Linux. The core and tests use C11 without
POSIX dependencies. Keep assertions enabled. The tests report six groups covering
bursts, refill, capacity, invalid arguments, backward time, and large refills.
See the [exercise README](../../intermediate/token_bucket/README.md) for the flow
diagram, API contract, and use cases.

### Threaded token bucket

```sh
cc -std=c11 -Wall -Wextra -Wpedantic -pthread \
  intermediate/token_bucket/token_bucket.c \
  intermediate/token_bucket/threaded_token_bucket.c \
  intermediate/token_bucket/threaded_demo.c -o build/threaded_token_bucket
./build/threaded_token_bucket

cc -std=c11 -Wall -Wextra -Wpedantic -pthread \
  intermediate/token_bucket/token_bucket.c \
  intermediate/token_bucket/threaded_token_bucket.c \
  intermediate/token_bucket/test_threaded_token_bucket.c -o build/test_threaded_token_bucket
./build/test_threaded_token_bucket
```

The POSIX demo shares a mutex-protected bucket among four workers and finishes
after about 1.25 seconds; allowed/denied counts depend on scheduling. The four
test groups include deterministic contention rounds with eight workers and a
controlled clock, refill/capacity checks, validation/clock failure checks, and
a default-clock smoke check. Keep assertions enabled. Join all workers before
destroying a shared bucket.

## Maze solver fixture

```sh
cc -std=c11 -Wall -Wextra -Wpedantic \
  intermediate/maze_solver/main.c -o build/maze_solver
./build/maze_solver --file intermediate/maze_solver/maze.txt --bfs
./build/maze_solver --file intermediate/maze_solver/maze.txt --compare
```

These are smoke checks, not an automated correctness suite. For algorithm changes,
check an unreachable exit, input validation, and whether BFS finds a shortest path.

## Programs with persistent data

Build from the root, then run from a disposable working directory. For example,
this checks that the student-record menu can exit without touching existing data:

```sh
cc -std=c11 -Wall -Wextra -Wpedantic \
  beginner/student_record/main.c -o build/student_record
mkdir -p build/student_record-smoke
(cd build/student_record-smoke && printf '6\n' | ../student_record)
```

Use a fresh directory for independent persistence scenarios. For Hangman, copy
`beginner/hangman_game/words.csv` into that directory before running. Do not run
Hangman without a valid fixture: the current loader does not propagate a missing
file failure to `main`. Some interactive programs also do not handle EOF cleanly;
provide complete input and use a process timeout available in your environment.
Random games should be checked against possible outcomes instead of one exact
transcript. Exercise library-manager download/copy branches only when they are
relevant, using controlled input and paths.

## Optional runtime diagnostics

For memory or undefined-behavior investigations, rebuild the affected executable
with these additional flags when the compiler supports them:

```text
-g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer
```

Run that binary against the reproducer and relevant tests. Report unsupported
diagnostics explicitly; sanitizers supplement behavioral checks. Never link all
project sources together: independent exercises and test runners define their
own entry points.
