#include "board.hh"

Board::Board() {
    setDefaulValues();
}

Board::~Board() { }

void Board::setDefaulValues() {
    moveTurn = WHITE;

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
    ijToBit(move.from.i, move.from.j, fromBit);
    ijToBit(move.to.i, move.to.j, toBit);
    std::cout << "Piece moved from: " << move.from.i << " " << move.from.j << ", to: " << move.to.i << " " << move.to.j << "\n";
    uint64_t *fromPieceBitMap, *toPieceBitMap;
    fromPieceBitMap = bitToPieceBitMap(fromBit);
    toPieceBitMap = bitToPieceBitMap(toBit);


    if (toPieceBitMap != nullptr) removePiece(*toPieceBitMap, toBit);
    addPiece(*fromPieceBitMap, toBit);
    removePiece(*fromPieceBitMap, fromBit);
    moveTurn = (moveTurn == WHITE) ? BLACK : WHITE;
}

std::set<PieceMove> Board::getLegalMoves() {
    std::set<PieceMove> legalMoves;
    uint64_t bit = 0x8000000000000000;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            getPieceLegalMoves(bit, legalMoves);
            bit = bit >> 1;
        }
    }
    //FIX: add the castling moves
    //FIX: add the en passant moves
    
    //FIX: eliminate those moves from the king that put the king in check
    //FIX: filter the moves that put the king in check
    return legalMoves;
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

//FIX: highly probable to do it in a more efficient way
std::pair<uint16_t,uint16_t> Board::bitToij(uint64_t& bit) {
    uint64_t aux = 0x8000000000000000;
    for (int i = 0; i < 64; ++i) {
        if (aux == bit)
            return std::make_pair(7 - i/8, i%8);
        aux = aux >> 1;
    }
    return std::make_pair(0, 0);    
}

void Board::ijToBit(int i, int j, uint64_t& bit) {
    bit = 0x8000000000000000;
    bit = bit >> (8 * (7-i) + j);
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

//BEHAVIOUR: it will add the legal moves of the piece to the set
void Board::getPieceLegalMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves) {
    //CORRECT: if its pinned, return
    PieceType piece;
    piece = bitToPieceType(bit);
    if (moveTurn == WHITE) {
        switch(piece) {
            case WHITE_PAWN:
                getWhitePawnLegalMoves(bit, pieceLegalMoves);
                break;
            case WHITE_KNIGHT:
                getKnightLegalMoves(bit, pieceLegalMoves);
                break;
            case WHITE_BISHOP:
                getBishopLegalMoves(bit, pieceLegalMoves);
                break;
            case WHITE_ROOK:
                getRookLegalMoves(bit, pieceLegalMoves);
                break;
            case WHITE_QUEEN:
                getQueenLegalMoves(bit, pieceLegalMoves);
                break;  
            default:;
        }
    }
    else {
        switch(piece) {
            case BLACK_PAWN:
                getBlackPawnLegalMoves(bit, pieceLegalMoves);
                break;
            case BLACK_BISHOP:
                getBishopLegalMoves(bit, pieceLegalMoves);
                break;  
            case BLACK_ROOK:
                getRookLegalMoves(bit, pieceLegalMoves);
                break; 
            case BLACK_QUEEN:
                getQueenLegalMoves(bit, pieceLegalMoves);
                break;         
            default:;
        }
    }
}

uint64_t* Board::bitToPieceBitMap(uint64_t bit) {
    PieceType piece = bitToPieceType(bit);
    if (piece == NONE) return nullptr;
    else return vecPiecesBitmaps[piece];
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
            b[i][j] = bitToPieceType(aux);
            aux = aux >> 1;
        }
    }
}