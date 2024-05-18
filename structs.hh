#ifndef STRUCTS_HH
#define STRUCTS_HH

#include <iostream>
#include <string>
#include <vector>
#include <set>

//This will only be used when displaying the info of the board
enum pieceType{
    WHITE_PAWN,
    WHITE_BISHOP,
    WHITE_KNIGHT,
    WHITE_ROOK,
    WHITE_KING,
    WHITE_QUEEN,

    BLACK_PAWN,
    BLACK_BISHOP,
    BLACK_KNIGHT,
    BLACK_ROOK,
    BLACK_KING,
    BLACK_QUEEN,

    NONE
};

typedef std::vector<std::vector<pieceType>> PieceMatrix;

struct Pos {
    int x, y;

    inline Pos() : x(0), y(0) { };

    inline Pos(int x, int y) : x(x), y(y) { };
};

#endif