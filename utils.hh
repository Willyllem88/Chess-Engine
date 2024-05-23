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
    uint16_t x;
    uint16_t y;
    inline PiecePos() : x(0), y(0) { };
    inline PiecePos(uint16_t x, uint16_t y) : x(x), y(y) { };
    inline PiecePos(std::pair<uint16_t, uint16_t> p) : x(p.first), y(p.second) { };
};

struct PieceMove {
    PiecePos from;
    PiecePos to;

    inline PieceMove() : from(PiecePos(0, 0)), to(PiecePos(0, 0)) { };

    bool operator<(const PieceMove& other) const {
        if (from.x != other.from.x) return from.x < other.from.x;
        if (from.y != other.from.y) return from.y < other.from.y;
        if (to.x != other.to.x) return to.x < other.to.x;
        return to.y < other.to.y;
    }
};

#endif