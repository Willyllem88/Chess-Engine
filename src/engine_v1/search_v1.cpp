#include "engine_v1.hh"
#include "board.hh"

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
    if (interrupted) return 0;

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
    if (interrupted) return 0;
    
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
