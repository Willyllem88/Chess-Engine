# Compiler
CXX = g++

# Flags for the C++ compiler
CXXFLAGS = -Wall -Wextra -Wno-sign-compare -std=c++20 -O2 $(shell pkg-config --cflags sdl2 SDL2_image SDL2_mixer)

# Flags for debugging
DEBUG_CXXFLAGS = -g

# Flags for the linker
LDFLAGS = $(shell pkg-config --libs sdl2 SDL2_image SDL2_mixer)

# Directories
HEADER_DIR = include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source, header and object files
SRC = $(wildcard $(SRC_DIR)/*.cpp)
HEADERS = $(wildcard $(HEADER_DIR)/*.hpp)
OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

# Executable name
EXEC = $(BIN_DIR)/engine

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
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# Compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rebuild objects if headers change
$(OBJ): $(HEADERS)

# Phony targets
.PHONY: all check_dependencies clean debug

# Cleaning
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Additional targets for debugging
debug: CXXFLAGS += $(DEBUG_CXXFLAGS)
debug: all