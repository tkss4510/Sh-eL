# Sh-eL

A small command-shell library written in C++20. Give it a line of text and it tokenizes, parses, validates and runs it. It has a hand-written lexer and a recursive-descent parser, and uses nothing outside the standard library.

## Features

- **Pipelining**: `|` sends the output of one command into the next, e.g. `read "notes.txt" | upper`.
- **Command substitution**: `( ... )` runs a command and uses its output as an argument, e.g. `upper (read "notes.txt")`. Substitutions can be nested.
- **Built-in arithmetic**: `( ... )` can also hold integer expressions with `+ - * /` and brackets, with overflow and division-by-zero detection.
- **File commands**: read, write, copy, move and delete files.
- **Validated before it runs**: the whole line is checked first, so a line with a mistake in it does nothing at all.
- **Easy to embed**: output goes to a function you provide, and errors are thrown as exceptions.

## Commands

| Command | What it does |
| --- | --- |
| `write "data" "file"` | write data to a file |
| `read "file"` | print a file's contents |
| `copy "src" "dest"` | copy a file |
| `move "src" "dest"` | move a file |
| `del "file"` | delete a file |
| `echo "text"` | print text |
| `upper "text"` | convert text to upper case |
| `lower "text"` | convert text to lower case |
| `count "text"` | count the lines in the text |

## Syntax

- Arguments are written in double quotes: `echo "hello"`.
- `|` sends the output of the left command into the right command as its first argument:
  `echo "hello" | upper` prints `HELLO`.
- `( ... )` runs a command and uses its output as an argument:
  `upper (echo "abc")` prints `ABC`.
- `( ... )` can also be integer arithmetic using `+ - * /` and brackets:
  `echo (1+2*3)` prints `7`. Overflow and division by zero are reported as errors.

```
write "hello" "a.txt"
read "a.txt"                 -> hello
count (read "a.txt")         -> 1
read "a.txt" | upper         -> HELLO
```

The whole line is checked before anything runs, so a line with a mistake in it does nothing at all.

## Using the library

```cpp
#include "Sh-eL.hpp"
#include <iostream>

int main()
{
    // the function you pass in receives everything the shell prints
    Shel shell([](std::string_view out) { std::cout << out << '\n'; });

    try
    {
        shell.RunCommand("echo \"hi\" | upper"); // prints HI
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
```

Errors are thrown as exceptions (`SyntaxException`, `TokenizerException`, `MathException`, `CommandException`).

## Building

You need a C++20 compiler and [premake5](https://premake.github.io/) (download it and put it on your PATH).

```
premake5 vs2022     # Visual Studio solution
premake5 gmake2     # Makefiles, then run: make config=release
```

## Tests

`Tests` is a second project in the same workspace. It checks that commands print the right output, that pipes and arithmetic work, and that bad input (division by zero, overflow, unknown commands, wrong argument counts) is rejected with the right error.

## Layout

```
include/   public headers
src/       lexer, parser, commands, math, and the Shel class
tests/     tests_main.cpp
premake5.lua
```
