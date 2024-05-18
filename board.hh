#ifndef BOARD_HH
#define BOARD_HH

#include </usr/include/SDL2/SDL.h>
#include </usr/include/SDL2/SDL_image.h>

#include "myApp.hh"
#include "structs.hh"

class Board {
public:
    Board();
    ~Board();

    //It'll set the pieces in the initial position.
    void setDefaulValues();


    //Pre: b must be a PieceMatrix size 8x8.
    //Post: b will contain all de pieces in a matrix.
    void getPieceMatrix(PieceMatrix& b);

    //It will print the board, through the app
    void printBoardApp(MyApp* a);

private:
    // Given an 8x8 matrix, it fills it with the pieceType of each cell, if there is no piece, pieceType = NONE.
    //  The returned matrix will be filled from the white player's view. PieceMatrix[0][0] = a-7, PieceMatrix[7][7] = h-0
    void bitBoardToMatrix(PieceMatrix& b);

    //MyApp* myApp;

    //Bitmaps: seeing from whites view the MSB (most significant bit) will be located in (a-1), and the LSB in (h-8)
    //White pieces
    uint64_t whitePawn;
    uint64_t whiteBishop;
    uint64_t whiteKnight;
    uint64_t whiteRook;
    uint64_t whiteQueen;
    uint64_t whiteKing;
    
    //Black pieces
    uint64_t blackPawn;
    uint64_t blackBishop;
    uint64_t blackKnight;
    uint64_t blackRook;
    uint64_t blackQueen;
    uint64_t blackKing;
    

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