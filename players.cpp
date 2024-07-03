#include "board.hh"
#include "myApp.hh"
#include "players.hh"

HumanPlayer::HumanPlayer(std::shared_ptr<MyApp> myApp) {
    app = myApp;
}

bool HumanPlayer::canMove() {
    return app->isPieceMoveAvailable();
}

PieceMove HumanPlayer::getMove() {
    return app->getMove();
}

EngineV1::EngineV1(std::shared_ptr<Board> myBoard, std::chrono::milliseconds timeSpan) {
    board = myBoard;
    moveDelay = timeSpan;
}

bool EngineV1::canMove() {
    return true;
}

int numboards; //FIX: only for testing
int transpositionHits;

PieceMove EngineV1::getMove() {
    numboards = 0; //FIX: only for testingç
    transpositionHits = 0;
    std::set<PieceMove> s = board->getCurrentLegalMoves();
    PieceMove bestMove;
    int bestValue = -INF;
    //Random move
    for (PieceMove move : s) {
        board->movePiece(move);
        int tempValue = -search(MAX_DEPTH - 1, -INF, INF);
        if (tempValue > bestValue) {
            bestValue = tempValue;
            bestMove = move;
        }
        board->undoMove();
    }
    std::cout << "Number of boards: " << numboards << std::endl;
    std::cout << "Transposition hits: " << transpositionHits << std::endl;
    //FIX: temporal solution
    if (bestValue != -INF) return bestMove;
    else {
        int random = rand() % s.size();
        auto it = s.begin();
        std::advance(it, random);
        return *it;
    }
}

int EngineV1::search(int depth, int alpha, int beta) {
    numboards++; //FIX: only for testing

    uint64_t currentHash = board->getZobristHash();
    /*if (transpositionTable.contains(currentHash) && transpositionTable.getDepth(currentHash) >= depth) {
        ++transpositionHits;
        return transpositionTable.getScore(currentHash);
    }*/

    if (depth == 0) return quiescenceSearch(alpha, beta);
    
    if (board->getBoardResult() == CHECKMATE) return -INF; //If i'm checkmated, return -INF

    transpositionTable.insert(currentHash, alpha, depth, 0);  

    std::list<PieceMove> moveList;
    orderMoves(board->getCurrentLegalMoves(), moveList);

    for (PieceMove m : moveList) {
        board->movePiece(m);
        int score = -search(depth - 1, -beta, -alpha);
        board->undoMove();
        if (score >= beta) return beta;
        alpha = std::max(alpha, score);
    }
    
    //transpositionTable.insert(currentHash, alpha, depth, 0);
    return alpha;
}

int EngineV1::quiescenceSearch(int alpha, int beta) {
    numboards++; //FIX: only for testing

    uint64_t currentHash = board->getZobristHash();
    if (transpositionTable.contains(currentHash)) {
        ++transpositionHits;
        auto entry = transpositionTable.getEntry(currentHash);
        return entry->score;
    }

    int score = evaluate();
    if (score >= beta) return beta;
    alpha = std::max(alpha, score);

    std::set<PieceMove> captureSet;
    board->getCurrentTakes(captureSet);

    for (PieceMove capture : captureSet) {
        board->movePiece(capture);
        score = -quiescenceSearch(-beta, -alpha);
        board->undoMove();
        if (score >= beta) {
            transpositionTable.insert(currentHash, beta, 0, 0);
            return beta;
        }
        alpha = std::max(alpha, score);
    }

    transpositionTable.insert(currentHash, alpha, 0, 0);
    return alpha;
}

void EngineV1::orderMoves(const std::set<PieceMove>& moves, std::list<PieceMove>& orderedMoves) {
    //The order of the moves is: promotions, captures, other, targeted
    std::list<PieceMove> captures, promotions, targeted, other;
    for (PieceMove move : moves) {
        if (board->isPromotion(move)) promotions.push_back(move);
        else if (board->isCapture(move)) captures.push_back(move);
        else if (board->isTargeted(move)) targeted.push_back(move);
        else other.push_back(move);
    }
    orderedMoves.splice(orderedMoves.end(), promotions);
    orderedMoves.splice(orderedMoves.end(), captures);
    orderedMoves.splice(orderedMoves.end(), other);
    orderedMoves.splice(orderedMoves.end(), targeted);
}

int EngineV1::evaluate() {
    int whiteEval = countMaterial(WHITE);
    int blackEval = countMaterial(BLACK);

    int evaluation = whiteEval - blackEval + countPositionalValue();

    int perspective = (board->getMoveTurn() == WHITE) ? 1 : -1;
    return evaluation * perspective;
}

int EngineV1::countPositionalValue() {
    int positionalValue = 0;
    PieceMatrix pm = board->getPieceMatrix();
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            PieceType p = pm[i][j];
            switch (p) {
                case WHITE_PAWN:
                    positionalValue += pawnEvals[i][j];
                    break;
                case BLACK_PAWN:
                    positionalValue -= pawnEvals[7-i][7-j];
                    break;
                case WHITE_KNIGHT:
                    positionalValue += knightEvals[i][j];
                    break;
                case BLACK_KNIGHT:
                    positionalValue -= knightEvals[7-i][7-j];
                    break;
                case WHITE_BISHOP:
                    positionalValue += bishopEvals[i][j];
                    break;
                case BLACK_BISHOP:
                    positionalValue -= bishopEvals[7-i][7-j];
                    break;
                case WHITE_ROOK:
                    positionalValue += rookEvals[i][j];
                    break;
                case BLACK_ROOK:
                    positionalValue -= rookEvals[7-i][7-j];
                    break;
                case WHITE_QUEEN:
                    positionalValue += queenEvals[i][j];
                    break;
                case BLACK_QUEEN:
                    positionalValue -= queenEvals[7-i][7-j];
                    break;
                case WHITE_KING:
                    positionalValue += kingEvalsMidGame[i][j];
                    break;
                case BLACK_KING:
                    positionalValue -= kingEvalsMidGame[7-i][7-j];
                    break;
                default:
                    break;
            }
        }
    }

    return positionalValue;
}

int EngineV1::countMaterial(PieceColor myColor) {
    int material = 0;
    material += board->getPawnsCount(myColor) * PAWN_VALUE;
    material += board->getBishopsCount(myColor) * BISHOP_VALUE;
    material += board->getKnightsCount(myColor) * KNIGHT_VALUE;
    material += board->getRooksCount(myColor) * ROOK_VALUE;
    material += board->getQueensCount(myColor) * QUEEN_VALUE;
    material += board->getKingsCount(myColor) * KING_VALUE;
    return material;
}