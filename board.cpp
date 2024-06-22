#include "board.hh"
#include "myApp.hh"

Board::Board(std::shared_ptr<MyApp> a) {
    setDefaulValues();
    this->app = a;
}

Board::~Board() { }

void Board::setDefaulValues() {
    moveTurn = WHITE;

    boardResult = PLAYING;

    allPieces = 0xffff00000000ffff;
    enPassant = 0x0000000000000000;
    castleBitmap = 0x2200000000000022;
    
    whitePieces = 0xffff000000000000;
    whiteTargetedSquares = 0x0000000000000000;
    whitePinnedSquares = 0x0000000000000000;
    blackPieces = 0x000000000000ffff;
    blackTargetedSquares = 0x0000000000000000;
    blackPinnedSquares = 0x0000000000000000;

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

    //Calculates the first legal moves
    calculateLegalMoves();
}

PieceColor Board::getMoveTurn() {
    return moveTurn;
}

BoardResult Board::getBoardResult() {
    return boardResult;
}

void Board::movePiece(PieceMove& move) {
    //Checks if the move is legal
    //FIX: cout the PieceMove move
    if (legalMoves.find(move) == legalMoves.end()) {
        std::cout << "[ERROR] Invalid Move!\n";
        return;
    }

    PieceMatrix pm(8, std::vector<PieceType>(8, NONE));
    bitBoardToMatrix(pm);
    std::cout << "[INFO] Move: " << pieceMoveToAlgebraic(move, pm, legalMoves) << "\n";
    
    //If the moves is a pown that moves two squares, it updates the board info in order to let en passant
    updateEnPassant(move);

    //If the move is a castle move, it will move the rook
    updateCastle(move);

    //Makes the move
    makeAMove(move);
 
    if (moveCounter > 0) registerState();

    if (threefoldRepetition) boardResult = THREEFOLD_REPETITION;

    //Calculates the legal moves of the current player in order to update the targeted squares after the move
    //blackTargetedSquares = whiteTargetedSquares = 0// If the following line is calculateKegalMoves(),issues
    updateTargetedSquares(moveTurn);

    //Toggles the turn
    moveTurn = (moveTurn == WHITE) ? BLACK : WHITE;
    app->setMoveTurn(moveTurn);
    ++moveCounter;

    //Calculates the legal moves of the opponent
    calculateLegalMoves();
}

const std::set<PieceMove>& Board::getCurrentLegalMoves() {
    return legalMoves;
}

void Board::printBoardApp() {
    PieceMatrix pm (8, std::vector<PieceType>(8, NONE));

    bitBoardToMatrix(pm);
    app->printBoard(pm);
    return;
}

void Board::printResult() {
    switch(boardResult) {
        case WHITE_WINS:
            std::cout << "======================================\n" <<
                         "*  *  *  *  * WHITE WINS *  *  *  *  *\n" <<
                         "======================================\n";
            break;
        case BLACK_WINS:
            std::cout << "======================================\n" <<
                         "*  *  *  *  * BLACK WINS *  *  *  *  *\n" <<
                         "======================================\n";
            break;
        case STALE_MATE:
            std::cout << "======================================\n" <<
                         "*   *  *  *  *  STALEMATE  *  *  *   *\n" <<
                         "======================================\n";
            break;
        case THREEFOLD_REPETITION:
            std::cout << "======================================\n" <<
                         "*  *  *  THREEFOLD REPETITION  *  *  *\n" <<
                         "======================================\n";
            break;
        default:;
    }

}

void Board::calculateLegalMoves() { 
    auto start = std::chrono::high_resolution_clock::now(); //DELETE: ONLY FOR TESTING

    //Updates the set with all the moves
    getAllPiecesMoves(legalMoves);

    //Detects if I'm checked. If so, eliminates eliminates those moves that don't free me from check
    manageCheck(legalMoves);

    //Eliminates those moves that put the king into check
    eliminatePinnedCheckMoves(legalMoves);

    //If there are no legal moves, it will print the result
    if (legalMoves.empty()) {
        updateTargetedSquares(moveTurn);
        if (whiteKing & whiteTargetedSquares || blackKing & blackTargetedSquares)
            boardResult = (moveTurn == WHITE) ? BLACK_WINS : WHITE_WINS;
        else
            boardResult = STALE_MATE;
    }

    
    //auto end = std::chrono::high_resolution_clock::now(); //DELETE: ONLY FOR TESTING
    //std::chrono::duration<double> elapsed = end - start; //DELETE: ONLY FOR TESTING
    //std::cout << "  [INFO] Calculction of all legal moves took: " << elapsed.count()*1000.0f << " ms\n"; //DELETE: ONLY FOR TESTING
    //std::cout << "  [INFO] Number of legal moves: " << legalMoves.size() << "\n"; //DELETE: ONLY FOR TESTING
}

void Board::getAllPiecesMoves(std::set<PieceMove>& legalMoves) {
    //Clears the set
    legalMoves.clear();

    uint64_t *myPieces = (moveTurn == WHITE) ? &whitePieces : &blackPieces;
    //uint64_t *oponentTargetedeSquares = (moveTurn == WHITE) ? &blackTargetedSquares : &whiteTargetedSquares;
    //uint64_t *oponentPinnedSquares = (moveTurn == WHITE) ? &blackPinnedSquares : &whitePinnedSquares;
    //*oponentTargetedeSquares = 0; *oponentPinnedSquares = 0;

    //For each piece, it will get their possible moves
    uint64_t bit = 0x8000000000000000;
    for (int i = 0; i < 64; ++i) {
        if (bit & *myPieces)
            getPieceMoves(bit, legalMoves);
        bit = bit >> 1;
    }
}

void Board::getPieceMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves) {
    PieceType piece;
    piece = bitToPieceType(bit);

    //Detects the piece type and calls the respective function
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

void Board::updateTargetedSquares(PieceColor col) {
    uint64_t *myPieces = (col == WHITE) ? &whitePieces : &blackPieces;
    uint64_t *opponentTargetedSquares = (col == WHITE) ? &blackTargetedSquares : &whiteTargetedSquares;
    uint64_t *oponentPinnedSquares = (col == WHITE) ? &blackPinnedSquares : &whitePinnedSquares;    
    *opponentTargetedSquares = 0;
    *oponentPinnedSquares = 0;

    uint64_t bit = 0x8000000000000000;
    for (int i = 0; i < 64; ++i) {
        if (bit & *myPieces)
            getPieceTargetedSquares(bit);
        bit = bit >> 1;
    }
}

void Board::getPieceTargetedSquares(uint64_t bit) {
    PieceType piece;
    piece = bitToPieceType(bit);

    //Detects the piece type and calls the respective function
    switch(piece) {
        case WHITE_PAWN:
            targetedByWhitePawn(bit);
            break;
        case WHITE_BISHOP:
            targetedByBishop(bit);
            break;
        case WHITE_KNIGHT:
            targetedByKnight(bit);
            break;
        case WHITE_ROOK:
            targetedByRook(bit);
            break;
        case WHITE_QUEEN:
            targetedByQueen(bit);
            break;
        case WHITE_KING:
            targetedByKing(bit);
            break; 
        case BLACK_PAWN:
            targetedByBlackPawn(bit);
            break;
        case BLACK_BISHOP:
            targetedByBishop(bit);
            break;
        case BLACK_KNIGHT:
            targetedByKnight(bit);
            break;
        case BLACK_ROOK:
            targetedByRook(bit);
            break;
        case BLACK_QUEEN:
            targetedByQueen(bit);
            break;
        case BLACK_KING:
            targetedByKing(bit);
            break;
        default:;
    }
}

void Board::updateEnPassant(PieceMove& move) {
    enPassant = 0;
    uint64_t fromBit;
    ijToBit(move.from.i, move.from.j, fromBit);
    PieceType p = bitToPieceType(fromBit);

    //If the move is a white pawn that moves two squares
    if (p == WHITE_PAWN && (move.from.i-move.to.i) == 2) {
        //If there is an opponent pawn that can take my en passant opportunity
        if (fromBit & ~H_FILE && (fromBit >> 17) & blackPawn)
            enPassant = fromBit >> 8;
        else if (fromBit & ~A_FILE && (fromBit >> 15) & blackPawn)
            enPassant = fromBit >> 8;
    }
    //If the move is a black pawn that moves two squares
    else if (p == BLACK_PAWN && (move.to.i-move.from.i) == 2) {
        //If there is an opponent pawn that can take my en passant opportunity
        if (fromBit & ~H_FILE && (fromBit << 15) & whitePawn)
            enPassant = fromBit << 8;
        else if (fromBit & ~A_FILE && (fromBit << 17) & whitePawn)
            enPassant = fromBit << 8;  
    }      
}

void Board::updateCastle(PieceMove& move) {
    uint64_t fromBit;
    ijToBit(move.from.i, move.from.j, fromBit);
    PieceType p = bitToPieceType(fromBit);
    //If the move is a king or a rook, it will update the castleBitmap
    switch(p) {
        case WHITE_KING:
            castleBitmap = castleBitmap & ~0x2200000000000000;
            break;
        case BLACK_KING:
            castleBitmap = castleBitmap & ~0x0000000000000022;
            break;
        case WHITE_ROOK:
            //If the rook moves from its initial position, it will disable his side castling
            if (fromBit == 0x0100000000000000)
                castleBitmap = castleBitmap & ~0x0200000000000000;
            else if (fromBit == 0x8000000000000000)
                castleBitmap = castleBitmap & ~0x2000000000000000;
            break;
        case BLACK_ROOK:
            //If the rook moves from its initial position, it will disable his side castling
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

    //If tis not in check, it will return
    if (*myKing & ~*myTargetedSquares)
        return;

    //If the king is targeted, it will eliminate those moves that don't free the king from check
    for (auto moveIterator = legalMoves.begin(); moveIterator != legalMoves.end(); ) {
        PieceMove move = *moveIterator;
        uint64_t fromBit;
        ijToBit(move.from.i, move.from.j, fromBit);

        //Explanation: a copy of the class board (baux) is created. Baux will make the move, and if the king is still targeted, the move will be removed from the legalMoves set.
        Board baux = *this;
        baux.makeAMove(move);
        baux.moveTurn = (baux.moveTurn == WHITE) ? BLACK : WHITE;
        baux.updateTargetedSquares(baux.moveTurn);

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

    //If there are no pinned squares, it will return
    if (*myPinnedSquares == 0) return;

    //If the king is pinned, it will eliminate those moves that put the king in check
    for (auto moveIterator = legalMoves.begin(); moveIterator != legalMoves.end(); ) {
        PieceMove move = *moveIterator;
        uint64_t fromBit;
        ijToBit(move.from.i, move.from.j, fromBit);

        //Explanation: a copy of the class board (baux) is created. Baux will make the move, and if the king is still targeted, the move will be removed from the legalMoves set.
        if (*myPinnedSquares & fromBit) {
            Board baux = *this;
            baux.makeAMove(move);
            baux.moveTurn = (baux.moveTurn == WHITE) ? BLACK : WHITE;
            baux.updateTargetedSquares(baux.moveTurn);

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

void Board::makeAMove(const PieceMove& move) {
    uint64_t fromBit, toBit;
    uint64_t *fromPieceBitmap, *toPieceBitmap;
    ijToBit(move.from.i, move.from.j, fromBit);
    ijToBit(move.to.i, move.to.j, toBit);
    fromPieceBitmap = bitToPieceBitmap(fromBit);
    toPieceBitmap = bitToPieceBitmap(toBit);

    //If the move is a promotion, it will change the piece
    if (move.promoteTo != NONE) {
        if (toPieceBitmap != nullptr)
            removePiece(*toPieceBitmap, toBit);
        uint64_t *promoteToBitmap = pieceTypeToBitmap(move.promoteTo);
        addPiece(*promoteToBitmap, moveTurn, toBit);
        removePiece(*fromPieceBitmap, fromBit);
        return;
    }

    //If the move is a capture, it will remove the piece from the target location
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
        manageCastleMove(*fromPieceBitmap, move);
    }

    //Add the piece to its new location
    addPiece(*fromPieceBitmap, moveTurn, toBit);

    //Remove the piece from it last location
    removePiece(*fromPieceBitmap, fromBit);
}

void Board::removePiece(uint64_t& targetBitMap, uint64_t bit) {
    allPieces = allPieces & ~bit;
    if (targetBitMap & whitePieces)
        whitePieces = whitePieces & ~bit;
    else
        blackPieces = blackPieces & ~bit;
    targetBitMap = targetBitMap & ~bit;
}

void Board::addPiece(uint64_t& targetBitMap, PieceColor col, uint64_t bit) {
    allPieces = allPieces | bit;
    if (col == WHITE) 
        whitePieces = whitePieces | bit;
    else 
        blackPieces = blackPieces | bit;
    targetBitMap = targetBitMap | bit;
}

void Board::manageCastleMove(uint64_t fromPieceBitmap, const PieceMove& move) {
    //If the move is a castle move, it will move the rook, and disable the castling
    if (fromPieceBitmap & whiteKing && move.from.j == 4 && move.to.j == 6) {
        uint64_t rookFrom = 0x0100000000000000;
        uint64_t rookTo = 0x0400000000000000;
        addPiece(whiteRook, moveTurn, rookTo);
        removePiece(whiteRook, rookFrom);
        castleBitmap = castleBitmap & ~0x2200000000000000;
    }
    else if (fromPieceBitmap & whiteKing && move.from.j == 4 && move.to.j == 2) {
        uint64_t rookFrom = 0x8000000000000000;
        uint64_t rookTo = 0x1000000000000000;
        addPiece(whiteRook, moveTurn, rookTo);
        removePiece(whiteRook, rookFrom);
        castleBitmap = castleBitmap & ~0x220000000000000;
    }
    else if (fromPieceBitmap & blackKing && move.from.j == 4 && move.to.j == 6) {
        uint64_t rookFrom = 0x0000000000000001;
        uint64_t rookTo = 0x0000000000000004;
        addPiece(blackRook, moveTurn, rookTo);
        removePiece(blackRook, rookFrom);
        castleBitmap = castleBitmap & ~0x0000000000000022;
    }
    else if (fromPieceBitmap & blackKing && move.from.j == 4 && move.to.j == 2) {
        uint64_t rookFrom = 0x0000000000000080;
        uint64_t rookTo = 0x0000000000000010;
        addPiece(blackRook, moveTurn, rookTo);
        removePiece(blackRook, rookFrom);
        castleBitmap = castleBitmap & ~0x0000000000000022;
    }
}

void Board::registerState() {
    PieceMatrix pm(8, std::vector<PieceType>(8, NONE));
    bitBoardToMatrix(pm);
    BoardState bs(pm, enPassant, castleBitmap);
    boardStateVector.push_back(bs);
    ++boardStateLog[bs];
    if (boardStateLog[bs] == 3)
        threefoldRepetition = true;
}

std::pair<uint16_t,uint16_t> Board::bitToij(uint64_t bit) const {
    //This creepy function is a hardware instruction that retruns the number of leading zeros
    int x = __builtin_clzll(bit);
    return std::make_pair(7 - x/8, x%8);    
}

void Board::ijToBit(int i, int j, uint64_t& bit) const {
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

uint64_t* Board::bitToPieceBitmap(uint64_t bit) {
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

PieceType Board::bitToPieceType(uint64_t bit) const {
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

void Board::bitBoardToMatrix(PieceMatrix& b) const {
    uint64_t aux = 0x8000000000000000;
    for (int i = 7; i >= 0; --i) {
        for (int j = 0; j < 8; ++j) {
            b[i][j] = bitToPieceType(aux);
            aux = aux >> 1;
        }
    }
}