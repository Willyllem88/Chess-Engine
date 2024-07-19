#include "board.hh"
#include "myApp.hh"
#include "players.hh"

Player::Player() {
    interrupted = false;
}

void Player::interrupt() {
    interrupted = true;
}

bool Player::wasInterrupted() {
    return interrupted;
}

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
    //TODO: maybe not using detach, but join. But currently it works fine.
}

PieceMove EngineV1::getMove() {
    //Activate timer (FIX: 2s just for testing)
    iniTimer(moveDelay);

    PieceColor mateColor = NONE_COLOR;

    //Clear the transposition table before starting the search
    transpositionTable.clear();

    numBoards = 0;
    transpositionHits = 0;
    searchTimeExceeded = false;

    std::set <PieceMove> legalMoves = board->getCurrentLegalMoves();
    std::vector<PieceMove> orderedMoves(legalMoves.begin(), legalMoves.end());
    
    MoveEval bestMoveEval = {orderedMoves[0], -INF};

    
    int depth;
    for (depth = 1; depth <= MAX_DEPTH; depth++) {
        std::vector<MoveEval> actItEvaluatedMoves = firstSearch(orderedMoves, depth);
        
        //If the time limit is exceeded, the search will stop
        if (searchTimeExceeded) break;
        if (interrupted) break;
        
        //Sort the moves based on the evaluation, from worst to best. In the bext iteration, the moves will be examined in this order
        std::sort(actItEvaluatedMoves.begin(), actItEvaluatedMoves.end(), std::greater<MoveEval>());
        MoveEval actItBestMove = actItEvaluatedMoves.front();
        
        //Reorder the moves for the next iteration based on the current evaluation, orderedMoves will have moves evaluated in the last iteration ordered from best to worst
        orderedMoves.clear();
        for (MoveEval m : actItEvaluatedMoves) orderedMoves.push_back(m.move);

        //If a checkmate is detected, the search will stop
        if (actItBestMove.eval >= INF || actItBestMove.eval <= -INF){
            if (actItBestMove.eval >= INF) mateColor = board->getMoveTurn();
            else mateColor = board->getMoveTurn() == WHITE ? BLACK : WHITE;
            bestMoveEval = actItBestMove;
            break;
        }

        bestMoveEval = actItBestMove;
    }

    //Stop the timer, in case it hasn't stopped yet
    stopTimer = true;
    
    //Print some useful information about the search
    if (interrupted) std::cout << "[INFO] Search interrupted" << std::endl;
    std::cout << "[INFO] Depth reached: " << depth << std::endl;
    if (mateColor != NONE_COLOR) {
        int mateDepth = (depth+1)/2;
        if (mateColor == WHITE) 
            std::cout << "[INFO] Evaluation: +M" << mateDepth << std::endl;
        else 
            std::cout << "[INFO] Evaluation: -M" << mateDepth << std::endl;
    }
    else std::cout << "[INFO] Evaluation: " << bestMoveEval.eval << std::endl;
    std::cout << "[INFO] Number of boards: " << numBoards << std::endl;
    std::cout << "[INFO] Transposition hits: " << transpositionHits << std::endl;
    
    return bestMoveEval.move;
}

std::vector<EngineV1::MoveEval> EngineV1::firstSearch(const std::vector<PieceMove>& orderedMoves, int depth) {
    numBoards++;

    std::vector<EngineV1::MoveEval> evaluatedMoves;

    for (PieceMove move : orderedMoves) {
        board->movePiece(move);
        int score = -search(depth - 1, -INF, INF);
        board->undoMove();

        if (searchTimeExceeded) return evaluatedMoves;
        if (interrupted) return evaluatedMoves;

        evaluatedMoves.push_back({move, score});
    }

    return evaluatedMoves;
}

int EngineV1::search(int depth, int alpha, int beta) {
    numBoards++;
    if (board->getBoardResult() == CHECKMATE) return -INF; //If i'm checkmated, my evaluation is -INF
    if (board->getBoardResult() == STALE_MATE) return 0; //If it's a stalemate, the evaluation is 0
    if (board->getBoardResult() == THREEFOLD_REPETITION) return 0; //If it's a threefold repetition, the evaluation is 0

    //Transposition table handling: if the current board is already in the table, we will use the stored evaluation
    uint64_t currentHash = board->getZobristHash();
    if (transpositionTable.contains(currentHash)) {
        auto entry = transpositionTable.getEntry(currentHash);
        if (entry->depth >= depth) {
            ++transpositionHits;
            if (entry->nodeType == TranspositionTable::NT_EXACT) 
                return entry->score;
            else if (entry->nodeType == TranspositionTable::NT_UPPERBOUND && entry->score <= alpha) 
                alpha = entry->score;
            else if (entry->nodeType == TranspositionTable::NT_LOWERBOUND && entry->score >= beta)
                beta = entry->score;
            
            if (alpha >= beta) return entry->score;
        }
    }

    if (depth == 0) return quiescenceSearch(alpha, beta);

    int evalType = TranspositionTable::NT_UPPERBOUND;

    std::list<PieceMove> moveList;
    orderMoves(board->getCurrentLegalMoves(), moveList); //FIX: more accurate ordering

    for (PieceMove m : moveList) {
        board->movePiece(m);
        int score = -search(depth - 1, -beta, -alpha);
        board->undoMove();
        if (score >= beta) {
            transpositionTable.insert(currentHash, beta, depth, TranspositionTable::NT_LOWERBOUND);
            return beta;
        }
        if (score > alpha) {
            evalType = TranspositionTable::NT_EXACT;
            alpha = score;
        }
    }
    
    transpositionTable.insert(currentHash, alpha, depth, evalType);
    return alpha;
}

int EngineV1::quiescenceSearch(int alpha, int beta) {
    numBoards++;

    int score = evaluate();
    if (score >= beta) return beta;
    alpha = std::max(alpha, score);

    std::set<PieceMove> captureSet = {};
    board->getCurrentTakes(captureSet);
    if (captureSet.empty()) return score;

    uint64_t currentHash = board->getZobristHash();
    if (transpositionTable.contains(currentHash)) {
        ++transpositionHits;
        return transpositionTable.getEntry(currentHash)->score;
    }

    for (PieceMove capture : captureSet) {
        board->movePiece(capture);
        score = -quiescenceSearch(-beta, -alpha);
        board->undoMove();
        if (score >= beta) {
            transpositionTable.insert(currentHash, beta, 0, TranspositionTable::NT_QUIESCENCE);
            return beta;
        }
        alpha = std::max(alpha, score);
    }

    transpositionTable.insert(currentHash, alpha, 0, TranspositionTable::NT_QUIESCENCE);
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