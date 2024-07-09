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
    searchTimeExceeded = false;
}

bool EngineV1::canMove() {
    return true;
}

void EngineV1::iniTimer(std::chrono::milliseconds timeSpan) {
    stopTimer = false;
    //Create a thread that will stop the timer when the timeSpan is reached. If the timer is stopped, the thread will return.
    std::thread([this, timeSpan]() {
        auto end_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeSpan);
        while (std::chrono::steady_clock::now() < end_time) {
            if (stopTimer) return;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        searchTimeExceeded = true;
    }).detach();
}

PieceMove EngineV1::getMove() {
    //Activate timer (FIX: 2s just for testing)
    iniTimer(moveDelay);

    numBoards = 0; //FIX: only for testingç
    transpositionHits = 0;
    searchTimeExceeded = false;

    MoveEval pastItBestMove; //The best move in the actual iteration and the best move in the past iteration
    MoveEval bestMoveEval;
    
    pastItBestMove.move = bestMoveEval.move = *board->getCurrentLegalMoves().begin();
    pastItBestMove.eval = bestMoveEval.eval = -INF;

    
    int depth;
    for (depth = 1; depth <= MAX_DEPTH; depth++) {
        if (searchTimeExceeded) break;

        MoveEval actItMoveEval = firstSearch(depth);

        //If a checkmate is detected, the search will stop
        if (actItMoveEval.eval >= INF){
            std::cout << "--> Checkmate in " << depth << " moves." << std::endl;
            bestMoveEval = actItMoveEval;
            break;
        }

        //If the evaluation of the actual iteration is better than the past iteration, the best move will be updated
        if (actItMoveEval.eval > pastItBestMove.eval) bestMoveEval = actItMoveEval;
        else bestMoveEval = pastItBestMove;

        pastItBestMove = actItMoveEval;
    }

    stopTimer = true;
    
    std::cout << "[INFO] Depth reached: " << depth << std::endl;
    std::cout << "[INFO] Evaluation: " << bestMoveEval.eval << std::endl;
    std::cout << "[INFO] Number of boards: " << numBoards << std::endl;
    std::cout << "[INFO] Transposition hits: " << transpositionHits << std::endl;
    //FIX: temporal solution
    return bestMoveEval.move;
}

EngineV1::MoveEval EngineV1::firstSearch(int depth) {
    numBoards++; //FIX: only for testing

    PieceMove bestMove = *board->getCurrentLegalMoves().begin();
    int bestEval = -INF;

    std::list<PieceMove> orderedMoves;
    orderMoves(board->getCurrentLegalMoves(), orderedMoves);

    for (PieceMove move : orderedMoves) {
        if (searchTimeExceeded) return {bestMove, bestEval};

        board->movePiece(move);
        int score = -search(depth - 1, -INF, INF); //FIX: maybe change the bounds
        board->undoMove();

        if (score >= INF) return {move, score};

        if (score > bestEval) {
            bestEval = score;
            bestMove = move;
        }
    }

    return {bestMove, bestEval};
}

int EngineV1::search(int depth, int alpha, int beta) {
    numBoards++; //FIX: only for testing

    /*uint64_t currentHash = board->getZobristHash();
    if (transpositionTable.contains(currentHash) && transpositionTable.getDepth(currentHash) >= depth) {
        ++transpositionHits;
        return transpositionTable.getScore(currentHash);
    }*/
    
    if (board->getBoardResult() == CHECKMATE) return -INF; //If i'm checkmated, my evaluation is -INF
    if (board->getBoardResult() == STALE_MATE) return 0; //If it's a stalemate, the evaluation is 0
    if (board->getBoardResult() == THREEFOLD_REPETITION) return 0; //If it's a threefold repetition, the evaluation is 0

    if (depth == 0) return quiescenceSearch(alpha, beta);

    //transpositionTable.insert(currentHash, alpha, depth, 0);  

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
    numBoards++;

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


float EngineV1::currentEndGamePhaseWeight(PieceColor myColor) {
    //Based on heuristics: the less major pieces, the more endgame it is, we exclude the pawns. We will consider the endgame when the material is less than endgameMaterialStart (2 Rooks + Knight + Bishop)
    const float multiplier = 1.0f / static_cast<float>(endgameMaterialStart);
    int materialWithoutPawns = countMaterial(myColor) - board->getPawnsCount(myColor) * PAWN_VALUE;
    return 1.0f - std::min(1.0f, multiplier * materialWithoutPawns);
}

int EngineV1::evaluate() {
    int whiteEval = countMaterial(WHITE);
    int blackEval = countMaterial(BLACK);

    float whiteEndGamePhase = currentEndGamePhaseWeight(WHITE);
    float blackEndGamePhase = currentEndGamePhaseWeight(BLACK);

    int whitePositionalValue = countPositionalValue(WHITE, whiteEndGamePhase);
    int blackPositionalValue = countPositionalValue(BLACK, blackEndGamePhase);

    int evaluation = whiteEval - blackEval + whitePositionalValue - blackPositionalValue;

    int perspective = (board->getMoveTurn() == WHITE) ? 1 : -1;
    return evaluation * perspective;
}

int EngineV1::countPositionalValue(PieceColor myColor, float myEndGamePhase) {
    int positionalValue = 0;
    PieceMatrix pm = board->getPieceMatrix();
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            PieceType p = pm[i][j];

            //If the piece is not mine, we continue
            if (p == NONE || pieceColor(p) != myColor) continue;

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
                    positionalValue += kingEvalsMidGame[i][j]*(1.0f-myEndGamePhase) + kingEvalsEndGame[i][j]*myEndGamePhase;
                    break;
                case BLACK_KING:
                    positionalValue -= kingEvalsMidGame[7-i][7-j]*(1.0f-myEndGamePhase) + kingEvalsEndGame[7-i][7-j]*myEndGamePhase;
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
    return material;
}