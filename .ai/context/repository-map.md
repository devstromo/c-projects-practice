# Repository map

Each subdirectory is an independent exercise. There is no root application or
shared build configuration. Check the source again when changing an exercise;
this map describes the current layout.

## Beginner projects

| Directory under `beginner/` | Entry point | Notes |
| --- | --- | --- |
| `basic_calculator/` | `calculator_main.c` | Logic in `basic_calculator.c`, API in `basic_calculator.h`, assertions in `test_calculator.c`; link `-lm` |
| `guessing_game/` | `guessing_game.c` | Interactive, time-seeded random number, optional replay |
| `hangman_game/` | `hangman_game.c` | Reads `./words.csv`; tracked fixture exists despite the global `*.csv` ignore rule |
| `rock_paper_scissors/` | `rock_paper_scissors.c` | Interactive, time-seeded random opponent |
| `simple_tic_tac_toe/` | `main.c` | Board state, move validation, win detection |
| `student_record/` | `main.c` | Binary student records and sequence file |
| `temperature_converter/` | `main.c` | Menu-driven conversions |
| `to_do_list/` | `to_do_list.c` | Fixed-capacity, in-memory task map |

## Intermediate projects

| Directory under `intermediate/` | Entry point | Notes |
| --- | --- | --- |
| `bank_system/` | `main.c` | Accounts, balances, transfers, ledger, CSV persistence, sequence files |
| `bloom_filter/` | `main.c` | Demo uses `bloom_filter.c` and `bloom_filter.h`; separate `test_bloom_filter.c`; link `-lm` |
| `library_manager/` | `main.c` | Book CSV, history log, local PDF copies/downloads; uses shell commands and `curl` |
| `maze_solver/` | `main.c` | DFS, BFS, comparison mode; tracked `maze.txt` fixture; accepts `--file` or `--input` |

The calculator and Bloom filter currently have the only dedicated test source
files. Their demo and test programs each define `main` and must be built separately.

## Pointer experiments

`misc/pointer_increase/main.c` demonstrates pointer increment and dereferencing.
`misc/code_address_as_data/main.c` demonstrates function addresses and calls,
including conversions between function pointers and `void *` that are not
portable ISO C. Preserve the teaching context when discussing portability or
compiler diagnostics.

## Runtime files and fixtures

Relative paths are resolved from the working directory. Use disposable data for
verification, especially for operations that replace or delete records.

| Exercise | Files or directories relative to the working directory |
| --- | --- |
| Hangman | Reads `words.csv`; copy the tracked fixture into the test working directory |
| Student records | `student_record.record`, `last_id.sequence`, temporary `temp.record` |
| Bank system | `accounts.csv`, `ledger.csv`, `bank_account.seq`, `ledger_entry.seq`, temporary `temp_accounts.csv` |
| Library manager | `books.csv`, `history.log`, `bookstore/`, temporary `temp_books.csv` |
| Maze solver | Reads the path supplied after `--file` |

The root `.gitignore` excludes `/build/`, common compiled artifacts, CSV files,
records, sequence files, logs, and bookstore directories. It does not untrack
existing fixtures. When adding a CSV fixture, add a narrow ignore exception for
that fixture so it is reproducible from a fresh checkout.
