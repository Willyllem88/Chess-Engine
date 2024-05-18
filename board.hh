#ifndef BOARD_HH
#define BOARD_HH

#include </usr/include/SDL2/SDL.h>
#include </usr/include/SDL2/SDL_image.h>

#include "myApp.hh"
#include "utils.hh"

class Board {
public:
    Board();
    ~Board();

    //It'll set the pieces in the initial position.
    void setDefaulValues();

    //It'll move the piece in the board.
    void movePiece(PieceMove& move);


    //Pre: b must be a PieceMatrix size 8x8.
    //Post: b will contain all de pieces in a matrix.
    void getPieceMatrix(PieceMatrix& b);

    //It will print the board, through the app
    void printBoardApp(MyApp* a);

private:
    //It will remove the bit from the targetBitMap.
    void removePiece(uint64_t& targetBitMap, uint64_t& bit);

    //It will add the bit to the targetBitMap.
    void addPiece(uint64_t& targetBitMap, uint64_t& bit);

    //It will return the pieceType of the bit.
    //Also, it will return the pieceBitMap of the piece.
    uint64_t* bitToPieceBitMap(uint64_t bit);

    //It will return the pieceType of the bit.
    PieceType bitToPieceType(uint64_t bit);

    // Given an 8x8 matrix, it fills it with the pieceType of each cell, if there is no piece, pieceType = NONE.
    //  The returned matrix will be filled from the white player's view. PieceMatrix[0][0] = a-7, PieceMatrix[7][7] = h-0
    void bitBoardToMatrix(PieceMatrix& b);

    //It will convert the x and y to a bit.
    void xyToBit(int i, int j, uint64_t& bit);



    //Bitmaps: seeing from whites view the MSB (most significant bit) will be located in (a-1), and the LSB in (h-8)
    uint64_t allPieces;

    //White pieces
    uint64_t whitePawn;
    uint64_t whiteBishop;
    uint64_t whiteKnight;
    uint64_t whiteRook;
    uint64_t whiteQueen;
    uint64_t whiteKing;

    uint64_t whitePieces;
    
    //Black pieces
    uint64_t blackPawn;
    uint64_t blackBishop;
    uint64_t blackKnight;
    uint64_t blackRook;
    uint64_t blackQueen;
    uint64_t blackKing;

    uint64_t blackPieces;
};

#endif

/* BOARD:
  _________________________
  |8|                     |
  |7|                     |
  |6|                     |
  |5|                     |
  |4|                     |
  |3|                     |
  |2|                     |
  |1|_____________________|
  |_a  b  c  d  e  f  g  h|
*/