# Compiler
CXX = g++

# Flags for the C++ compiler
CXXFLAGS = -Wall -Wextra -Wno-sign-compare -std=c++20 -O2 $(shell pkg-config --cflags sdl2 SDL2_image SDL2_mixer)

# Flags for the SDL2 library
LDFLAGS = $(shell pkg-config --libs sdl2 SDL2_image SDL2_mixer)

# Source files
SRC = main.cpp game.cpp myApp.cpp board.cpp utils.cpp legalMoves.cpp players.cpp

# Header files
HEADERS = game.hh myApp.hh board.hh utils.hh players.hh

# Object files
OBJ = $(SRC:.cpp=.o)

# Executable name
EXEC = engine

# Default target
all: check_dependencies $(EXEC)

# Check all dependencies
check_dependencies:
	@printf "Checking SDL2 dependencies... "

	@pkg-config --exists sdl2 SDL2_image SDL2_mixer || \
		(printf "ERROR\n" && \
		( ! pkg-config --exists sdl2 && printf "  - SDL2.pc not found. Please install SDL2. (sudo apt-get install libsdl2-dev)\n" ) && \
		( ! pkg-config --exists SDL2_image && printf "  - SDL2_image.pc not found. Please install SDL2_image. (sudo apt-get install libsdl2-image-dev)\n" ) && \
		( ! pkg-config --exists SDL2_mixer && printf "  - SDL2_mixer.pc not found. Please install SDL2_mixer. (sudo apt-get install libsdl2-mixer-dev)\n" ) && \
		exit 1 )

	@printf "OK\n"

# Linking
$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# Compilation
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rebuild objects if headers change
$(OBJ): $(HEADERS)

# Cleaning
clean:
	rm -f $(OBJ) $(EXEC)