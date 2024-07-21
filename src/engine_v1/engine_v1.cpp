#include "board.hh"
#include "engine_v1.hh"

EngineV1::EngineV1(std::shared_ptr<Board> myBoard, std::chrono::milliseconds timeSpan) {
    board = myBoard;
    moveDelay = timeSpan;
    searchTimeExceeded = false;
}

bool EngineV1::canMove() {
    return true;
}

PieceMove EngineV1::getMove() {
    interrupted = false;
    
    //Activate timer
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