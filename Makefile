OPTIONS = -Wall -Wextra -Werror -Wno-sign-compare -std=c++20 -lSDL2 -lSDL2_image

all: main.cpp board.o
	g++ -o engine main.cpp *.o $(OPTIONS) 

board: board.cpp board.hh
	g++ -c board.o board.cpp $(OPTIONS)

clean:
	rm -f *.o engine