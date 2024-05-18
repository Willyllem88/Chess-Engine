#include "board.hh"


//FIX: maybe put this into another file
std::string pieceToString(pieceType p) {
    switch (p) {
        case (WHITE_PAWN):
            return "White Pawn";
        case (BLACK_PAWN):
            return "Black Pawn";
        // (...)
        default:
            return "";
    }
}

Board::Board() {
    setDefaulValues();
}

Board::~Board() { }

void Board::setDefaulValues() {
    whitePawn = 0x00ff000000000000;
    whiteBishop = 0x2400000000000000;
    whiteKnight = 0x4200000000000000;
    whiteRook = 0x8100000000000000;
    whiteQueen = 0x1000000000000000;
    whiteKing = 0x0800000000000000;

    blackPawn = 0x000000000000ff00;
    blackBishop = 0x0000000000000024;
    blackKnight = 0x0000000000000042;
    blackRook = 0x0000000000000081;
    blackQueen = 0x0000000000000010;
    blackKing = 0x0000000000000008;
}   

void Board::getPieceMatrix(PieceMatrix& b) {
    Board::bitBoardToMatrix(b);
}

void Board::printBoardApp(MyApp* a) {
    PieceMatrix pm (8, std::vector<pieceType>(8, NONE));
    bitBoardToMatrix(pm);
    a->render(pm);
    return;
}

void Board::bitBoardToMatrix(PieceMatrix& b) {
    uint64_t aux = 0x8000000000000000;
    for (int i = 7; i >= 0; --i) {
        for (int j = 0; j < 8; ++j) {
            //Could be optimized if doing similarly to binary search, bitmaps such as: allPieces -> colorPiece -> individuaPiece
            if (whitePawn & aux) b[i][j] = WHITE_PAWN;
            else if (whiteBishop & aux) b[i][j] = WHITE_BISHOP;
            else if (whiteKnight & aux) b[i][j] = WHITE_KNIGHT;
            else if (whiteRook & aux) b[i][j] = WHITE_ROOK;
            else if (whiteQueen & aux) b[i][j] = WHITE_QUEEN;
            else if (whiteKing & aux) b[i][j] = WHITE_KING;   

            else if (blackPawn & aux) b[i][j] = BLACK_PAWN;
            else if (blackBishop & aux) b[i][j] = BLACK_BISHOP;
            else if (blackKnight & aux) b[i][j] = BLACK_KNIGHT;
            else if (blackRook & aux) b[i][j] = BLACK_ROOK;
            else if (blackQueen & aux) b[i][j] = BLACK_QUEEN;
            else if (blackKing & aux) b[i][j] = BLACK_KING;

            else b[i][j] = pieceType::NONE;
            
            aux = aux >> 1;
        }
    }
}