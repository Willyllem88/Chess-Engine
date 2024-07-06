# Guillem's Chess Engine

This is a project made by Guillem Cabré. Its purpose is to learn how a Chess Engine works, using algorithmic experience and learning from other projects to gain expertise in computer science. The program features a simple GUI for users to interact with the game using the cursor.

The project is implemented in C++.

![Intro Image](https://github.com/Willyllem88/images/blob/main/intro.png)

## Table of Contents

1. [Requirements](#requirements)
    - [Installing Dependencies on Ubuntu/Debian](#installing-dependencies-on-ubuntudebian)
2. [Compiling and Executing](#compiling-and-executing)
    - [Executing Options](#executing-options)
    - [Commands](#commands)
3. [References](#references)

---

## Requirements

Before you begin, ensure you have met the following requirements:
* You have installed `g++`
* `make` is installed
* You have `libsdl2-dev` and `libsdl2-image-dev` installed

If these are not installed, please refer to the section [Installing Dependencies on Ubuntu/Debian](#installing-dependencies-on-ubuntudebian).

### Installing Dependencies on Ubuntu/Debian

To install the necessary dependencies, run the following commands:

```sh
sudo apt-get update
sudo apt-get install g++ make libsdl2-dev libsdl2-image-dev
```

## Compiling and Executing

The program uses a Makefile for compilation. To compile the program, run:

```sh
make
```

To clean the directory of object files and executables, use:

```sh
make clean
```

The executable file will be named `engine`.

### Executing Options

The options are specified using the `-option` format:
* `-h` or `--help`: Displays a usage message.
* `-b <Player | <engine_name>>` or `--black <Player | <engine_name>>`: Specifies who will play with the black pieces.
* `-w <Player | <engine_name>>` or `--white <Player | <engine_name>>`: Specifies who will play with the white pieces.
* `-c` or `--console-only`: Runs the program without the GUI, allowing moves to be entered via the console.
* `-t <seconds>` or `--timespan <seconds>`: Sets the time span (in seconds) that the engine will take to make a move after its opponent. Can handle decimals.

The default values are `--white Player`, `--black Player`, and `--timespan 2`.

#### Example Usage

To run the program with a player as white and an engine named "EngineV1" as black, with a timespan of 2.5 seconds, and displaying it through the console, use:

```sh
./engine -w Player -b EngineV1 -t 2.5 -c
```

### Commands

During the execution of the program, the user can enter the following commands:
* `u` or `undo`: Undoes the last move. The user can undo multiple moves until the initial board is reached.
* `<algebraic-move-notation>`: if it's a valid move, it will perform it.

## References

* [Negamax algorithm](https://www.chessprogramming.org/Negamax)
* [Alpha Beta Pruning](https://www.chessprogramming.org/Alpha-Beta)
* [Transposition Table](https://en.wikipedia.org/wiki/Transposition_table)
* [Iterative Deepening](https://en.wikipedia.org/wiki/Iterative_deepening_depth-first_search)
* Bartek Spitza: [The Fascinating Programming of a Chess Engine](https://youtu.be/w4FFX_otR-4?si=0K_oibyMV4wW9HrT)
* Sebastian Lague: [Coding Adventure: Chess](https://youtu.be/U4ogK0MIzqk?si=jJONHwy9YOQYsrb2)

---