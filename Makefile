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

# Check dependencies
check_dependencies:
	@echo "Checking SDL2 dependencies..."
	@pkg-config --exists sdl2 || (echo "Error: sdl2.pc not found. Please install SDL2. (sudo apt-get install libsdl2-dev)" && exit 1)
	@pkg-config --exists SDL2_image || (echo "Error: SDL2_image.pc not found. Please install SDL2_image. (sudo apt-get install libsdl2-image-dev)" && exit 1)
	@pkg-config --exists SDL2_mixer || (echo "Error: SDL2_mixer.pc not found. Please install SDL2_mixer. (sudo apt-get install libsdl2-mixer-dev)" && exit 1)
	@echo "All SDL2 dependencies are satisfied."

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