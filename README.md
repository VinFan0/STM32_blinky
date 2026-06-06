# STM32 Blinky
This repository serves as a starting point for bare-metal C projects. The directory structure,
build system, guidelines, and other specifics are inspired by the [Embedded System Project Series](https://www.youtube.com/watch?v=g9KbXJydf8I&list=PLS_iNJJVTtiRV0DZRDcTHnvAuDrKGPN40) from [Artful Bytes](artfulbytes.com).

## Directory Structure
The directory structure is based on the
[pitchfork layout](https://github.com/vector-of-bool/pitchfork).

| Directory     | Description                                                   |
|---------------|---------------------------------------------------------------|
| build/        | Build output (object files + executable)                      |
| docs/         | Documentation (e.g., coding guidelines, images)               |
| src/          | Source files (.c/.h)                                          |
| src/app/      | Source files for the application layer (see SW architecture)  |
| src/common/  | Source files for code used across the project                  |
| src/drivers/ | Source files for the driver layer (see SW architecture)        |
| src/test/    | Source files related to test code                              |
| external/    | External dependencies (as git submodules if possible)          |
| tools/       | Scripts, configs, binaries                                     |
| .github/     | Configuration file for GitHub actions                          |

## Build
The project is intended to be built using command-line tools (i.e. make), but
and IDE could also be used for step debugging.

## make (Makefile)
The code targets the STM32L476RGTx (Nucleo-L476RG development board) and must be built
with a cross-toolchain. The toolchain used for this project is arm-none-eabi-gcc, which
is included in the repository under 'tools/'. The toolchain can also be found
on [Arm's website](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain)

There is a _Makefile_ to build the code with _make_ from the command-line.

## Tests
**TODO**

## Pushing a new change
These are teh typical steps take for each change

1. Create a local branch
2. Make the code changes
3. Build the code
4. Flash and test the code on the target
5. Static analyse the code
6. Format the code
7. Commit the code
8. Push the branch to GitHub
9. Open a pull-request
10. Merge the pull request

## Commit Message
Commit messages should follow the specification laid out by [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/).

## **TODO** Code Formatter
The codebase follows certain formatting rules, which are enforced by the code formatter **clang-format**. These rules are specified in teh **.clang-format** located in the root directory. There is a rule in the **Makefile** to format all source files with one command (requires clang-format to be installed).
``` Bash
make format
```

Sometimes it's desirable to ignore these formatting rules, and this can be achieved with special comments.

`` C
// clang-format off
<code to ignore>
// clang-format on
```

## **TODO** Coding guidelines
Apart from the basic formatting rules, the codebase also follows certain coding guidelines.
These are described in **docs/coding_guidelines.md**.

## Static Analysis
To catch coding mistakes early on (in addition to the ones the compiler catches), I use a static analyzer, **cppcheck**. There is a rule in the **Makefile** to analyse all files with **cppcheck**.

``` Bash
make cppcheck
```

## Things to include per project
Each project should include several items of documentation, diagrams, and outlines. These items include hardware schematics, software architecture, block diagrams, and state machines.
