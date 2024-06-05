#include "board.hh"

Board::Board() {
    setDefaulValues();
    calculateLegalMoves();
}

Board::~Board() { }

void Board::setDefaulValues() {
    moveTurn = WHITE;

    allPieces = 0xffff00000000ffff;
    enPassant = 0;
    castleBitmap = 0x2200000000000022;
    
    whitePieces = 0xffff000000000000;
    whiteTargetedSquares = 0; //Squares targeted by white pieces
    whitePinnedSquares = 0;
    blackPieces = 0x000000000000ffff;
    blackTargetedSquares = 0;
    blackPinnedSquares = 0;

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

    threefoldRepetition = false;
    moveCounter = 0;
}

void Board::movePiece(PieceMove& move) {
    //Checks if the move is legal
    if (legalMoves.find(move) == legalMoves.end()) {
        std::cout << "    Invalid Move!\n";
        return;
    }
    std::cout << "    Piece moved from: " << move.from.i << " " << move.from.j << ", to: " << move.to.i << " " << move.to.j << "\n";
    
    //If the moves is a pown that mvoes two squares, it updates the board info in order to let en passant.
    updateEnPassant(move);

    updateEnCastle(move);

    makeAMove(move);

    if (moveCounter > 0) registerState();

    if (threefoldRepetition) std::cout << "\n\n\n\n -------Threefold repetition\n";

    //Given the move, update the legal moves set
    calculateLegalMoves();

    moveTurn = (moveTurn == WHITE) ? BLACK : WHITE;
    ++moveCounter;

    calculateLegalMoves();
}

void Board::calculateLegalMoves() {
    //Updates the set with all the moves
    getAllPiecesMoves(legalMoves);
    //Detects if I'm checked. If so, eliminates eliminates those moves that don't free me from check
    manageCheck(legalMoves);
    //Eliminates those moves that put the king into check
    eliminatePinnedCheckMoves(legalMoves);
    //If there are no legal moves, it will print the result
    if (legalMoves.empty()) {
        std::cout << "No legal moves\n";
        if (whiteKing & whiteTargetedSquares || blackKing & blackTargetedSquares)
            std::cout <<    "======================================\n" <<
                            "* *  *  *  *  CHECKMATED  *  *  *  * *\n" <<
                            "======================================\n" <<
                            "- The winner is: " << ((moveTurn == WHITE) ? "BLACK" : "WHITE") << "\n";
        else 
            std::cout <<    "======================================\n" <<
                            "*  *  *  *     STALEMATE    *  *  *  *\n" <<
                            "======================================n" <<
                            "- The game is a draw\n";
    }
}

void Board::getAllPiecesMoves(std::set<PieceMove>& legalMoves) {
    legalMoves.clear();

    uint64_t *myPieces = (moveTurn == WHITE) ? &whitePieces : &blackPieces;
    uint64_t *oponentTargetedeSquares = (moveTurn == WHITE) ? &blackTargetedSquares : &whiteTargetedSquares; //new
    uint64_t *oponentPinnedSquares = (moveTurn == WHITE) ? &blackPinnedSquares : &whitePinnedSquares; //new
    *oponentTargetedeSquares = 0; *oponentPinnedSquares = 0;

    uint64_t bit = 0x8000000000000000;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (bit & *myPieces)
                getPieceMoves(bit, legalMoves);
            bit = bit >> 1;
        }
    }
}

void Board::getPieceMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves) {
    PieceType piece;
    piece = bitToPieceType(bit);
    switch(piece) {
        case WHITE_PAWN:
            getWhitePawnMoves(bit, pieceLegalMoves);
            break;
        case WHITE_BISHOP:
            getBishopMoves(bit, pieceLegalMoves);
            break;
        case WHITE_KNIGHT:
            getKnightMoves(bit, pieceLegalMoves);
            break;
        case WHITE_ROOK:
            getRookMoves(bit, pieceLegalMoves);
            break;
        case WHITE_QUEEN:
            getQueenMoves(bit, pieceLegalMoves);
            break;
        case WHITE_KING:
            getKingMoves(bit, pieceLegalMoves);
            break; 
        case BLACK_PAWN:
            getBlackPawnMoves(bit, pieceLegalMoves);
            break;
        case BLACK_BISHOP:
            getBishopMoves(bit, pieceLegalMoves);
            break;
        case BLACK_KNIGHT:
            getKnightMoves(bit, pieceLegalMoves);
            break;
        case BLACK_ROOK:
            getRookMoves(bit, pieceLegalMoves);
            break;
        case BLACK_QUEEN:
            getQueenMoves(bit, pieceLegalMoves);
            break;
        case BLACK_KING:
            getKingMoves(bit, pieceLegalMoves);
            break;
        default:;
    }
}

void Board::updateEnPassant(PieceMove& move) {
    enPassant = 0;
    uint64_t aux;
    ijToBit(move.from.i, move.from.j, aux);
    PieceType p = bitToPieceType(aux);
    //White takes black pawn through en passant
    if (p == WHITE_PAWN && (move.from.i-move.to.i) == 2) {
        //If there is an opponent pawn that can take my en passant opportunity
        if (aux & ~H_FILE && (aux >> 17) & blackPawn)
            enPassant = aux >> 8;
        if (aux & ~A_FILE && (aux >> 15) & blackPawn)
            enPassant = aux >> 8;
    }   
    //Black takes white pawn through en passant
    else if (p == BLACK_PAWN && (move.to.i-move.from.i) == 2) {
        //If there is an opponent pawn that can take my en passant opportunity
        if (aux & ~H_FILE && (aux << 15) & whitePawn)
            enPassant = aux << 8;
        if (aux & ~A_FILE && (aux << 17) & whitePawn)
            enPassant = aux << 8;  
    }      
}

void Board::updateEnCastle(PieceMove& move) {
    uint64_t fromBit;
    ijToBit(move.from.i, move.from.j, fromBit);
    PieceType p = bitToPieceType(fromBit);
    switch(p) {
        case WHITE_KING:
            castleBitmap = castleBitmap & ~0x2200000000000000;
            break;
        case BLACK_KING:
            castleBitmap = castleBitmap & ~0x0000000000000022;
            break;
        case WHITE_ROOK:
            if (fromBit == 0x0100000000000000)
                castleBitmap = castleBitmap & ~0x0200000000000000;
            else if (fromBit == 0x8000000000000000)
                castleBitmap = castleBitmap & ~0x2000000000000000;
            break;
        case BLACK_ROOK:
            if (fromBit == 0x0000000000000001)
                castleBitmap = castleBitmap & ~0x0000000000000002;
            else if (fromBit == 0x0000000000000080)
                castleBitmap = castleBitmap & ~0x0000000000000020;
            break;
        default:;
    }
}

void Board::manageCheck(std::set<PieceMove> &legalMoves) {
    uint64_t *myKing = (moveTurn == WHITE) ? &whiteKing : &blackKing;
    uint64_t *myTargetedSquares = (moveTurn == WHITE) ? &whiteTargetedSquares : &blackTargetedSquares;
    if (*myKing & ~*myTargetedSquares)
        return;

    for (auto moveIterator = legalMoves.begin(); moveIterator != legalMoves.end(); ) {
        PieceMove move = *moveIterator;
        uint64_t fromBit;
        ijToBit(move.from.i, move.from.j, fromBit);

        //Explanation: if the move initial position is pinned, we'll create a copy of the class board (baux). Baux will calculate which squares will be targeted when the move is done. If the move puts the king in check. It will be removed from legalMoves set.
        Board baux = *this;
        baux.makeAMove(move);
        baux.moveTurn = (baux.moveTurn == WHITE) ? BLACK : WHITE;
        baux.getAllPiecesMoves(baux.legalMoves);

        //Check if the move puts the king in danger
        if (fromBit & whitePieces && baux.whiteKing & baux.whiteTargetedSquares)
            moveIterator = legalMoves.erase(moveIterator);
        else if (fromBit & blackPieces && baux.blackKing & baux.blackTargetedSquares)
            moveIterator = legalMoves.erase(moveIterator);
        else
            ++moveIterator;
    }
}

void Board::eliminatePinnedCheckMoves(std::set<PieceMove> &legalMoves) {
    uint64_t *myPinnedSquares = (moveTurn == WHITE) ? &whitePinnedSquares : &blackPinnedSquares;

    if (*myPinnedSquares == 0) return;

    for (auto moveIterator = legalMoves.begin(); moveIterator != legalMoves.end(); ) {
        PieceMove move = *moveIterator;
        uint64_t fromBit;
        ijToBit(move.from.i, move.from.j, fromBit);

        //Explanation: if the move initial position is pinned, we'll create a copy of the class board (baux). Baux will calculate which squares will be targeted when the move is done. If the move puts the king in check. It will be removed from legalMoves set.
        if (*myPinnedSquares & fromBit) {
            Board baux = *this;
            baux.makeAMove(move);
            baux.moveTurn = (baux.moveTurn == WHITE) ? BLACK : WHITE;
            baux.getAllPiecesMoves(baux.legalMoves);

            //Check if the move puts the king in danger
            if (fromBit & whitePieces && baux.whiteKing & baux.whiteTargetedSquares)
                moveIterator = legalMoves.erase(moveIterator);
            else if (fromBit & blackPieces && baux.blackKing & baux.blackTargetedSquares)
                moveIterator = legalMoves.erase(moveIterator);
            else
                ++moveIterator;
        }
        else ++moveIterator;
    }
}

void Board::makeAMove(PieceMove& move) {
    uint64_t fromBit, toBit;
    uint64_t *fromPieceBitmap, *toPieceBitmap;
    ijToBit(move.from.i, move.from.j, fromBit);
    ijToBit(move.to.i, move.to.j, toBit);
    fromPieceBitmap = bitToPieceBitMap(fromBit);
    toPieceBitmap = bitToPieceBitMap(toBit);
    //If the move is a promotion, it will change the piece
    if (move.promoteTo != NONE) {
        if (toPieceBitmap != nullptr)
            removePiece(*toPieceBitmap, toBit);
        uint64_t *promoteToBotmap = pieceTypeToBitmap(move.promoteTo);
        addPiece(*promoteToBotmap, toBit);
        removePiece(*fromPieceBitmap, fromBit);
        return;
    }

    toPieceBitmap = bitToPieceBitMap(toBit);
    //Modifies the bitmaps in order to materialize the move
    if (toPieceBitmap != nullptr)
        removePiece(*toPieceBitmap, toBit);
    else {
        //Detects and manages the en passant move
        if ((*fromPieceBitmap & whitePawn) && (move.from.j != move.to.j)) {
            uint64_t aux = toBit << 8;
            removePiece(blackPawn, aux);
        }
        else if (*fromPieceBitmap & blackPawn && move.from.j != move.to.j) {
            uint64_t aux = toBit >> 8;
            removePiece(whitePawn, aux);
        }
        //Detects if a castle move is being done, if so, it will move the rook
        manageCastleMove(fromPieceBitmap, move);
    }    
    //Add the piece to its new location
    addPiece(*fromPieceBitmap, toBit);
    //Remove the piece from it last location
    removePiece(*fromPieceBitmap, fromBit);
}

void Board::manageCastleMove(uint64_t *fromPieceBitmap, PieceMove& move) {
    if (*fromPieceBitmap & whiteKing && move.from.j == 4 && move.to.j == 6) {
        uint64_t rookFrom = 0x0100000000000000;
        uint64_t rookTo = 0x0400000000000000;
        addPiece(whiteRook, rookTo);
        removePiece(whiteRook, rookFrom);
        castleBitmap = castleBitmap & ~0x2200000000000000;
    }
    else if (*fromPieceBitmap & whiteKing && move.from.j == 4 && move.to.j == 2) {
        uint64_t rookFrom = 0x8000000000000000;
        uint64_t rookTo = 0x1000000000000000;
        addPiece(whiteRook, rookTo);
        removePiece(whiteRook, rookFrom);
        castleBitmap = castleBitmap & ~0x220000000000000;
    }
    else if (*fromPieceBitmap & blackKing && move.from.j == 4 && move.to.j == 6) {
        uint64_t rookFrom = 0x0000000000000001;
        uint64_t rookTo = 0x0000000000000004;
        addPiece(blackRook, rookTo);
        removePiece(blackRook, rookFrom);
        castleBitmap = castleBitmap & ~0x0000000000000022;
    }
    else if (*fromPieceBitmap & blackKing && move.from.j == 4 && move.to.j == 2) {
        uint64_t rookFrom = 0x0000000000000080;
        uint64_t rookTo = 0x0000000000000010;
        addPiece(blackRook, rookTo);
        removePiece(blackRook, rookFrom);
        castleBitmap = castleBitmap & ~0x0000000000000022;
    }
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

void Board::getPieceMatrix(PieceMatrix& b) {
    Board::bitBoardToMatrix(b);
}

std::pair<uint16_t,uint16_t> Board::bitToij(uint64_t& bit) {
    //This creepy function is a hardware instruction that retruns the number of leading zeros
    int x = __builtin_clzll(bit);
    return std::make_pair(7 - x/8, x%8);    
}

void Board::ijToBit(int i, int j, uint64_t& bit) {
    bit = 0x8000000000000000;
    bit = bit >> (8 * (7-i) + j);
}

uint64_t* Board::pieceTypeToBitmap(PieceType pt) {
    switch(pt) {
        case WHITE_PAWN:
            return &whitePawn;
        case WHITE_BISHOP:
            return &whiteBishop;
        case WHITE_KNIGHT:
            return &whiteKnight;
        case WHITE_ROOK:
            return &whiteRook;
        case WHITE_QUEEN:
            return &whiteQueen;
        case WHITE_KING:
            return &whiteKing;
        case BLACK_PAWN:
            return &blackPawn;
        case BLACK_BISHOP:
            return &blackBishop;
        case BLACK_KNIGHT:
            return &blackKnight;
        case BLACK_ROOK:
            return &blackRook;
        case BLACK_QUEEN:
            return &blackQueen;
        case BLACK_KING:
            return &blackKing;
        default:
            return nullptr;
    }
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
            b[i][j] = bitToPieceType(aux);
            aux = aux >> 1;
        }
    }
}

void Board::registerState() {
    PieceMatrix pm(8, std::vector<PieceType>(8, NONE));
    bitBoardToMatrix(pm);
    BoardState bs(pm, enPassant, castleBitmap);
    ++boardStateLog[bs];
    if (boardStateLog[bs] == 3)
        threefoldRepetition = true;
    /*for (auto it = boardStateLog.begin(); it != boardStateLog.end(); ++it) {
        std::cout << "  <-------------->  " << it->second << " \n";
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                std::cout << pieceToString(it->first.pieceMatrix[i][j]) << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }*/
}

void Board::printBoardApp(MyApp* a) {
    PieceMatrix pm (8, std::vector<PieceType>(8, NONE));
    bitBoardToMatrix(pm);
    a->printBoard(pm);
    return;
}