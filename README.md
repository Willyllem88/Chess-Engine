# Guillem's Chess Engine

This is a project made by Guillem Cabré. Its purpose is to learn how a Chess Engine works, doing so by using my algorithmic experience, also learning from other projects. To gain expertise in the computer science field.
This program showcases a simple GUI, that will show the user the match, and it will facilitate him its use. Playing with the cursor.

The project has been made with C++.

![Intro Image](https://github.com/Willyllem88/images/blob/main/intro.png)

## Requirements

Before you begin, ensure you have met the following requirements:
* You have installed `g++`
* You have `libsdl2-dev` and `libsdl2-image-dev` installed
If those are not installed, please go to the section 'Installing Dependencies on Ubuntu/Debian'.

### Installing Dependencies on Ubuntu/Debian

To install the necessary dependencies, run the following commands:

```sh
sudo apt-get update
sudo apt-get install libsdl2-dev libsdl2-image-dev
```

## Compiling and Executing

For compiling the is a Makefile, so just by doing a:
```sh
make
```
the program will be ready to use.
Also, for cleaning the directory of object files and executables, it might be useful to use the
```sh
make clean
```

The executable file will be the `engine` file.

### Executing options

The options are specified with -smth format:
* `-h` or `--help`: a usage message is displayed.
* `-b <PLAYER | <engine_name>>` or `--black <PLAYER | <engine_name>>`: specify who will play with the white pieces.
* `-w <PLAYER | <engine_name>>` or `--white <PLAYER | <engine_name>>`: specify who will play with the black pieces.
* `-c` or `--console-only`: when you just want performance. It will not use some aspects just designed for the GUI. The player will be able to write its moves through the console as well, no visual aid will be given.
* `-t seconds` or `--timespan seconds`: the time span (in seconds) that the engine will take for making it move after its opponent. Can handle decimals.

The default values will be --white PLAYER, --black PLAYER --timespan 0.

### Commands

During the execution of the program, the user will be able to write some commands. Those are:
* `u` or `undo`: will undo the last. User can undo multiple moves until the initial board.

