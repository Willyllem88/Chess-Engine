CXX = g++

CXXFLAGS = -Wall -Wextra -Wno-sign-compare -std=c++20

LDFLAGS = -lSDL2 -lSDL2_image

SRC = main.cpp myApp.cpp board.cpp utils.cpp

OBJ = $(SRC:.cpp=.o)

EXEC = engine



all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)