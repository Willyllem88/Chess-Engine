# Guillem's Chess Engine

This is a project made by Guillem Cabré. Its purpose is to learn how a Chess Engine works, doing so by using my algorithmic experience, also learning from other projects. To gain expertise in the computer science field.
This program showcases a simple GUI, that will show the user the match, and it will facilitate him its use. Playing with the cursor.

The project has been made with C++.

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

The default settings will be the player playing through the GUI agains the chess engine. He will start with white pieces and the engine will make its move when as soon as possible.
The options are specified with -smth format:
* `-b` or `--black`: the player will play with black pieces.
* `-w` or `--white`: the player will play with white pieces.
* `-c` or `--console-only`: when you just want performance. It will not use some aspects just designed for the GUI. The player will be able to write its moves through the console as well, no visual information will be given.
* `-e` or `--engine`: for having the engine play against itself. The user will just be a viewer. This option can't be used with `-b` or `w`.
* `-d` or `--debug`: it will output information about the match for a debugging purpouse. It will show the information through the console.
* `-t` or `--timespan`: the time (in seconds) that the engine will take for making it move after its opponent. It is highly recommendable when the engine is in `-e` mode.

