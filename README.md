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

The default settings will be the player playing through the GUI agains itself.
The options are specified with -smth format:
* `-h` or `--help`: a usage message is displayed.
* `-b <PLAYER | <engine_name>>` or `--black <PLAYER | <engine_name>>`: specify who will play with the white pieces.
* `-w <PLAYER | <engine_name>>` or `--white <PLAYER | <engine_name>>`: specify who will play with the black pieces.
* `-c` or `--console-only`: when you just want performance. It will not use some aspects just designed for the GUI. The player will be able to write its moves through the console as well, no visual aid will be given.
* `-t seconds` or `--timespan seconds`: the time span (in seconds) that the engine will take for making it move after its opponent. 0 seconds if not specified. Useful with using `-e`.

