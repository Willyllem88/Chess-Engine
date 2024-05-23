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

    //It'll return the legal moves of the pieces.
    std::set<PieceMove> getLegalMoves();

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

    //It will return the legal moves of the piece.
    void getPieceLegalMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves);

    void getWhitePawnLegalMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves);
    void getBlackPawnLegalMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves);
    void getBishopLegalMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves);
    

    //It will return the pieceType of the bit.
    //Also, it will return the pieceBitMap of the piece.
    uint64_t* bitToPieceBitMap(uint64_t bit);

    //It will return the pieceType of the bit.
    PieceType bitToPieceType(uint64_t bit);

    // Given an 8x8 matrix, it fills it with the pieceType of each cell, if there is no piece, pieceType = NONE.
    //  The returned matrix will be filled from the white player's view. PieceMatrix[0][0] = a-7, PieceMatrix[7][7] = h-0
    void bitBoardToMatrix(PieceMatrix& b);

    std::pair<uint16_t, uint16_t> bitToij(uint64_t& bit);

    //It will convert the x and y to a bit.
    void ijToBit(int i, int j, uint64_t& bit);

    PieceColor moveTurn;

    //Bitmaps: seeing from whites view the MSB (most significant bit) will be located in (a-1), and the LSB in (h-8)
    uint64_t allPieces;
    uint64_t targetPieces; //FIX: no code to handle this yet

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

    //Piece bitmaps, they can be mapped with the pieceType enum.
    std::vector<uint64_t*> vecPiecesBitmaps = {&whitePawn, &whiteBishop, &whiteKnight, &whiteRook, &whiteQueen, &whiteKing, &blackPawn, &blackBishop, &blackKnight, &blackRook, &blackQueen, &blackKing};

    //Board information bitmaps
    static constexpr uint64_t A_FILE = 0x8080808080808080;
    static constexpr uint64_t B_FILE = 0x4040404040404040;
    static constexpr uint64_t C_FILE = 0x2020202020202020;
    static constexpr uint64_t D_FILE = 0x1010101010101010;
    static constexpr uint64_t E_FILE = 0x0808080808080808;
    static constexpr uint64_t F_FILE = 0x0404040404040404;
    static constexpr uint64_t G_FILE = 0x0202020202020202;
    static constexpr uint64_t H_FILE = 0x0101010101010101;

    static constexpr uint64_t RANK_1 = 0xff00000000000000;
    static constexpr uint64_t RANK_2 = 0x00ff000000000000;
    static constexpr uint64_t RANK_3 = 0x0000ff0000000000;
    static constexpr uint64_t RANK_4 = 0x000000ff00000000;
    static constexpr uint64_t RANK_5 = 0x00000000ff000000;
    static constexpr uint64_t RANK_6 = 0x0000000000ff0000;
    static constexpr uint64_t RANK_7 = 0x000000000000ff00;
    static constexpr uint64_t RANK_8 = 0x00000000000000ff;
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
  |_a__b__c__d__e__f__g__h|
*/