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

PieceMove EngineV1::getMove() {
    numboards = 0; //FIX: only for testing
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
    if (depth == 0) {
        numboards++; //FIX: only for testing
        return evaluate();
    }
    if (board->getBoardResult() == CHECKMATE) return -INF; //If i'm checkmated, return -INF

    std::set<PieceMove> s = board->getCurrentLegalMoves();
    
    for (PieceMove m : s) {
        board->movePiece(m);
        int score = -search(depth - 1, -beta, -alpha);
        board->undoMove();
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

int EngineV1::evaluate() {
    int whiteEval = countMaterial(WHITE);
    int blackEval = countMaterial(BLACK);

    int evaluation = whiteEval - blackEval;

    int perspective = (board->getMoveTurn() == WHITE) ? 1 : -1;
    return evaluation * perspective;
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