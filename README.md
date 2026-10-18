# C Programming Projects

## Overview

Welcome to my C programming projects repository! The primary objective of this repository is to learn C programming, as I'm currently a beginner. I'll progressively work on solving problems ranging from beginner to advanced levels, aiming to strengthen my understanding of the language and problem-solving skills.

## Goals

* **Learn C fundamentals:** I'll start with simple programs to understand the basics of C, such as data types, loops, and functions.
* **Intermediate challenges:** Once I'm comfortable with the basics, I'll tackle more complex problems, including file handling, data structures, and algorithms.
* **Advanced topics:** In the later stages, I plan to work on harder problems that involve memory management, pointers, and multi-threading.

## Project Structure

Each project is categorized by its difficulty level:

* `beginner/`: Simple programs covering basic syntax, input/output operations, loops, and conditionals.
* `intermediate/`: Projects involving algorithms, data structures, and file handling, including a maze solver, Bloom filter, bank system, and library manager.
* `misc/`: Small experiments with pointers and function addresses.

Advanced projects may be added as the repository grows.

## How to Run the Projects

1. Clone the repository:

    ```bash
    git clone https://github.com/devstromo/c-projects-practice.git
    cd c-projects-practice
    ```

2. Navigate to the project folder of your choice:

    ```bash
    cd beginner/temperature_converter
    ```

3. Compile the code using a C compiler:

    ```bash
    cc -std=c11 -Wall -Wextra -Wpedantic main.c -o /tmp/temperature_converter
    ```

4. Run the compiled executable:

    ```bash
    /tmp/temperature_converter
    ```

Projects with multiple source files need explicit source lists. See the [build and test guide](.ai/context/build-and-test.md) for calculator, Bloom filter, and other examples.

## Working with AI Assistants

[AGENTS.md](AGENTS.md) describes repository conventions and how to verify changes. The tool-agnostic [.ai/](.ai/README.md) directory contains reusable skills, rules, project context, workflows, prompts, and a session handoff template. Any assistant can read these Markdown files directly; no plugin installation is required.

## Future Plans

* Expand the repository with additional beginner to advanced problems.
* Explore data structures like linked lists, trees, and graphs.
* Work on real-world problems that involve system programming.

## Contributions

As I'm still learning, contributions are welcome! Feel free to open issues or submit pull requests if you have suggestions or improvements.
