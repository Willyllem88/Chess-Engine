#ifndef STRUCTS_HH
#define STRUCTS_HH

#include <unistd.h>
#include <sys/select.h>
#include <ctype.h>
#include <getopt.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <climits>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <thread>
#include <vector>

//  The color of the pieces
enum PieceColor : uint8_t{
    WHITE,
    BLACK
};

//  This will only be used when displaying the info of the board
enum PieceType : uint8_t{
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

typedef std::vector<std::vector<PieceType>> PieceMatrix;

//  For error handling
void errorAndExit(const std::string& errorMessage);

//  Returns a random uint64_t number
uint64_t rand_uint64();

//  Returns the color a piece
PieceColor pieceColor(PieceType p);

//  Returns the name of a piece as a string
std::string pieceToString(PieceType p);

//  Returns the name of a color as a string
std::string pieceColorToString(PieceColor p);

//  Struct for the position of a piece, with i being the row and j the column. The origin is the top left corner of the board. a7 is (0, 0), h1 is (7, 7)
struct PiecePos {
    uint16_t i, j;
    inline PiecePos() : i(0), j(0) { };
    inline PiecePos(uint16_t x, uint16_t y) : i(x), j(y) { };
    inline PiecePos(std::pair<uint16_t, uint16_t> p) : i(p.first), j(p.second) { };
    bool operator<(const PiecePos& other) const {
        if (i != other.i) return i < other.i;
        return j < other.j;
    }
    bool operator==(const PiecePos& other) const {
        return i == other.i and j == other.j;
    }
};

//  Struct for a piece move, with from being the position of the piece to move, and to being the position to move the piece to. If the move is a promotion, promoteTo will be the piece to promote to. If not, it will be NONE
struct PieceMove {
    PiecePos from, to;
    //If the move is a promotion, this will be the piece to promote to. If not, it will be NONE
    PieceType promoteTo;

    inline PieceMove() : from(PiecePos(0, 0)), to(PiecePos(0, 0)), promoteTo(NONE) { };
    inline PieceMove(PiecePos f, PiecePos t) : from(f), to(t), promoteTo(NONE) { };
    inline PieceMove(uint16_t fi, uint16_t fj, uint16_t ti, uint16_t tj) : from(PiecePos(fi, fj)), to(PiecePos(ti, tj)), promoteTo(NONE) { };

    bool operator<(const PieceMove& other) const {
        if (from.i != other.from.i) return from.i < other.from.i;
        if (from.j != other.from.j) return from.j < other.from.j;
        if (to.i != other.to.i) return to.i < other.to.i;
        if (to.j != other.to.j) return to.j < other.to.j;
        return promoteTo < other.promoteTo;
    }

    #define invalidMove PieceMove(0,0,0,0)
};

//FIX: more space efficient, also add moveturn
//  A struct that represents the state of the board. It will store the pieceMatrix, the enPassant square, and the castle rights. Used if need to compare two different board states.
struct BoardState{
    PieceMatrix pieceMatrix;
    uint64_t enPassant;
    uint64_t castleRights;

    inline BoardState(PieceMatrix pm, uint64_t ep, uint64_t castleRights) : pieceMatrix(pm), enPassant(ep), castleRights(castleRights) { };

    bool operator<(const BoardState& other) const {
        if (pieceMatrix != other.pieceMatrix) return pieceMatrix < other.pieceMatrix;
        if (enPassant != other.enPassant) return enPassant < other.enPassant;
        return castleRights < other.castleRights;
    }
};

//  The result of the board. It will be used to determine if the game is over, and if so, how it ended.
enum BoardResult : uint8_t {
    PLAYING,
    CHECKMATE,
    STALE_MATE,
    THREEFOLD_REPETITION,
};

//It will return true or false depending on if the user has entered a string, and it will store the string in the str variable
bool readStringFromConsole(std::string& str);

//Returns the pieceMove represented by the algebraic notation string. The funciton will nead the legalMoves set to check if the move is legal, the PieceMatrix to check the current state of the board, and the turn of the player that is making the move.
PieceMove algebraicToPieceMove(std::string& str, const std::set<PieceMove>& legalMoves, const PieceMatrix& pm, PieceColor turn);

//Returns the algebraic notation of the move. The function will need the PieceMove, the PieceMatrix to check the current state of the board, and the legalMoves set to check if the move is legal.
std::string pieceMoveToAlgebraic(const PieceMove& move, const PieceMatrix& pm, const std::set<PieceMove>& legalMoves);

#endif