# Guillem's Chess Engine

This is a project made by Guillem Cabré. Its purpose is to learn how a Chess Engine works, using algorithmic experience and learning from other projects to gain expertise in computer science. The program features a simple GUI for users to interact with the game using the cursor.

The project is implemented in C++.

![Intro Image](https://github.com/Willyllem88/images/blob/main/intro.png)

## Table of Contents
1. [Requirements](#requirements)
2. [Installing Dependencies on Ubuntu/Debian](#installing-dependencies-on-ubuntudebian)
3. [Compiling and Executing](#compiling-and-executing)
4. [Executing Options](#executing-options)
5. [Commands](#commands)

## Requirements

Before you begin, ensure you have met the following requirements:
* You have installed `g++`
* You have `libsdl2-dev` and `libsdl2-image-dev` installed

If these are not installed, please refer to the section 'Installing Dependencies on Ubuntu/Debian'.

### Installing Dependencies on Ubuntu/Debian

To install the necessary dependencies, run the following commands:

```sh
sudo apt-get update
sudo apt-get install libsdl2-dev libsdl2-image-dev
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
* `-b <PLAYER | <engine_name>>` or `--black <PLAYER | <engine_name>>`: Specifies who will play with the black pieces.
* `-w <PLAYER | <engine_name>>` or `--white <PLAYER | <engine_name>>`: Specifies who will play with the white pieces.
* `-c` or `--console-only`: Runs the program without the GUI, allowing moves to be entered via the console.
* `-t <seconds>` or `--timespan <seconds>`: Sets the time span (in seconds) that the engine will take to make a move after its opponent. Can handle decimals.

The default values are `--white PLAYER`, `--black PLAYER`, and `--timespan 0`.

#### Example Usage

To run the program with a player as white and an engine named "ChessBot" as black, with a timespan of 2.5 seconds, use:

```sh
./engine -w PLAYER -b ChessBot -t 2.5
```

To run the program in console-only mode, use:

```sh
./engine -c
```

## Commands

During the execution of the program, the user can enter the following commands:
* `u` or `undo`: Undoes the last move. The user can undo multiple moves until the initial board is reached.

---

This restructuring aims to make the README more user-friendly and easier to navigate.