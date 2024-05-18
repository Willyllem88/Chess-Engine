#ifndef STRUCTS_HH
#define STRUCTS_HH

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <iomanip>

//This will only be used when displaying the info of the board
enum PieceType{
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

typedef std::vector<std::vector<PieceType>> PieceMatrix;

std::string pieceToString(PieceType p);

//Structs for the mouse and the pieces moves

struct MousePos {
    int x;
    int y;

    inline MousePos() : x(0), y(0) { };
    inline MousePos(int x, int y) : x(x), y(y) { };
};

struct MouseMove {
    MousePos from;
    MousePos to;

    inline MouseMove() : from(MousePos(0, 0)), to(MousePos(0, 0)) { };
};

struct PiecePos {
    uint16_t x;
    uint16_t y;
    inline PiecePos() : x(0), y(0) { };
    inline PiecePos(uint16_t x, uint16_t y) : x(x), y(y) { };
};

struct PieceMove {
    PiecePos from;
    PiecePos to;

    inline PieceMove() : from(PiecePos(0, 0)), to(PiecePos(0, 0)) { };
};

#endif