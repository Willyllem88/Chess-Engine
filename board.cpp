#include "board.hh"

Board::Board() {
    setDefaulValues();
}

Board::~Board() { }

void Board::setDefaulValues() {
    allPieces = 0xffff00000000ffff;
    whitePieces = 0xffff000000000000;
    blackPieces = 0x000000000000ffff;

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

void Board::movePiece(PieceMove& move) {
    uint64_t fromBit, toBit;
    xyToBit(move.from.x, move.from.y, fromBit);
    xyToBit(move.to.x, move.to.y, toBit);

    uint64_t *fromPieceBitMap, *toPieceBitMap;
    fromPieceBitMap = bitToPieceBitMap(fromBit);
    toPieceBitMap = bitToPieceBitMap(toBit);


    //FIX: POSTERIORLY, FIXABLE ADDING THE RULES OF THE GAME
    if (fromPieceBitMap == nullptr) {
        std::cout << "There is no piece in the origin\n";
        return;
    }

    removePiece(*fromPieceBitMap, fromBit);
    if (toPieceBitMap != nullptr) removePiece(*toPieceBitMap, toBit);    
    addPiece(*fromPieceBitMap, toBit); 
}

void Board::getPieceMatrix(PieceMatrix& b) {
    Board::bitBoardToMatrix(b);
}

void Board::printBoardApp(MyApp* a) {
    PieceMatrix pm (8, std::vector<PieceType>(8, NONE));
    bitBoardToMatrix(pm);
    a->render(pm);
    return;
}

void Board::xyToBit(int x, int y, uint64_t& bit) {
    bit = 0x8000000000000000;
    bit = bit >> (x + 8 * (7-y));
}

void Board::removePiece(uint64_t& targetBitMap, uint64_t& bit) {
    allPieces = allPieces & ~bit;
    if (targetBitMap & whitePieces)
        whitePieces = whitePieces & ~bit;
    else
        blackPieces = blackPieces & ~bit;
    targetBitMap = targetBitMap & ~bit;
}

void Board::addPiece(uint64_t& targetBitMap, uint64_t& bit) {
    allPieces = allPieces | bit;
    if (targetBitMap & whitePieces) 
        whitePieces = whitePieces | bit;
    else 
        blackPieces = blackPieces | bit;
    targetBitMap = targetBitMap | bit;
}

uint64_t* Board::bitToPieceBitMap(uint64_t bit) {
    if (bit & allPieces) { //If there is a piece
        if(bit & whitePieces) { //If it's a white piece
            if (bit & whitePawn) return &whitePawn;
            else if (bit & whiteBishop) return &whiteBishop;
            else if (bit & whiteKnight) return &whiteKnight;
            else if (bit & whiteRook) return &whiteRook;
            else if (bit & whiteQueen) return &whiteQueen;
            else if (bit & whiteKing) return &whiteKing;
        }
        else { //If it's a black piece
            if (bit & blackPawn) return &blackPawn;
            else if (bit & blackBishop) return &blackBishop;
            else if (bit & blackKnight) return &blackKnight;
            else if (bit & blackRook) return &blackRook;
            else if (bit & blackQueen) return &blackQueen;
            else if (bit & blackKing) return &blackKing;
        }
    }
    return nullptr;
}

PieceType Board::bitToPieceType(uint64_t bit) {
    if (bit & allPieces) { //If there is a piece
        if(bit & whitePieces) { //If it's a white piece
            if (bit & whitePawn) return WHITE_PAWN;
            else if (bit & whiteBishop) return WHITE_BISHOP;
            else if (bit & whiteKnight) return WHITE_KNIGHT;
            else if (bit & whiteRook) return WHITE_ROOK;
            else if (bit & whiteQueen) return WHITE_QUEEN;
            else if (bit & whiteKing) return WHITE_KING;
        }
        else { //If it's a black piece
            if (bit & blackPawn) return BLACK_PAWN;
            else if (bit & blackBishop) return BLACK_BISHOP;
            else if (bit & blackKnight) return BLACK_KNIGHT;
            else if (bit & blackRook) return BLACK_ROOK;
            else if (bit & blackQueen) return BLACK_QUEEN;
            else if (bit & blackKing) return BLACK_KING;
        }
    }
    return NONE;
}

void Board::bitBoardToMatrix(PieceMatrix& b) {
    uint64_t aux = 0x8000000000000000;
    for (int i = 7; i >= 0; --i) {
        for (int j = 0; j < 8; ++j) {
            //Could be optimized if doing similarly to binary search, bitmaps such as: allPieces -> colorPiece -> individuaPiece
            b[i][j] = bitToPieceType(aux);
            aux = aux >> 1;
        }
    }
}