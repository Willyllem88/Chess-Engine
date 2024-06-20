# Compiler
CXX = g++

# Flags for the C++ compiler
CXXFLAGS = -Wall -Wextra -Wno-sign-compare -std=c++20 -O2

# Flags for the SDL2 library
LDFLAGS = -lSDL2 -lSDL2_image

# Source files
SRC = main.cpp myApp.cpp board.cpp utils.cpp legalMoves.cpp players.cpp

# Header files
HEADERS = myApp.hh board.hh utils.hh players.hh

# Object files
OBJ = $(SRC:.cpp=.o)

# Executable name
EXEC = engine



# Default target
all: $(EXEC)

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