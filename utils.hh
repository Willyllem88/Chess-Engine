#ifndef STRUCTS_HH
#define STRUCTS_HH

#include <unistd.h>

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
    WHITE_QUEEN,
    WHITE_KING,

    BLACK_PAWN,
    BLACK_BISHOP,
    BLACK_KNIGHT,
    BLACK_ROOK,
    BLACK_QUEEN,
    BLACK_KING,

    NONE
};

enum PieceColor {
    WHITE,
    BLACK
};

typedef std::vector<std::vector<PieceType>> PieceMatrix;

std::string pieceToString(PieceType p);

std::string pieceColorToString(PieceColor p);

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
    uint16_t i, j;
    inline PiecePos() : i(0), j(0) { };
    inline PiecePos(uint16_t x, uint16_t y) : i(x), j(y) { };
    inline PiecePos(std::pair<uint16_t, uint16_t> p) : i(p.first), j(p.second) { };
};

struct PieceMove {
    PiecePos from, to;

    inline PieceMove() : from(PiecePos(0, 0)), to(PiecePos(0, 0)) { };

    bool operator<(const PieceMove& other) const {
        if (from.i != other.from.i) return from.i < other.from.i;
        if (from.j != other.from.j) return from.j < other.from.j;
        if (to.i != other.to.i) return to.i < other.to.i;
        return to.j < other.to.j;
    }
};

#endif