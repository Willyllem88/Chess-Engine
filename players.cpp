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

int auxvar = -1;

PieceMove EngineV1::getMove() {
    if (auxvar < 3) {
        auxvar++;
        if (auxvar == 0) return PieceMove(6, 4, 4, 4);
        if (auxvar == 1) return PieceMove(7, 3, 3, 7);
        if (auxvar == 2) return PieceMove(7, 5, 4, 2);
    }
    std::set<PieceMove> s = board->getCurrentLegalMoves();
    PieceColor myColor = board->getMoveTurn();
    PieceMove bestMove;
    int bestValue = -INF;
    //Random move
    for (PieceMove move : s) {
        std::shared_ptr<Board> newBoard = std::make_shared<Board>(*board);
        newBoard->movePiece(move);
        /*int tempValue = getBoardScore(myColor, newBoard, 1);
        if (tempValue > bestValue) {
            bestValue = tempValue;
            bestMove = move;
        }*/
    }
    //FIX: temporal solution
    if (bestValue != -INF) return bestMove;
    else {
        int random = rand() % s.size();
        auto it = s.begin();
        std::advance(it, random);
        return *it;
    }
}

int EngineV1::evaluate() {
    int whiteEval = countMaterial(board, WHITE);
    int blackEval = countMaterial(board, BLACK);

    int evaluation = whiteEval - blackEval;

    int perspective = (board->getMoveTurn() == WHITE) ? 1 : -1;
    return evaluation * perspective;
}

int EngineV1::search(std::shared_ptr<Board> actualBoard, int depth) {
    if (depth == 0) return 0;
    if (actualBoard->getBoardResult() == CHECKMATE) return -INF; //If i'm checkmated, return -INF

    PieceColor myColor = actualBoard->getMoveTurn();
    int material = countMaterial(actualBoard, myColor);
    int value = material;
    
    for (PieceMove m : actualBoard->getCurrentLegalMoves()) {
        std::shared_ptr<Board> newBoard = std::make_shared<Board>(*actualBoard);
        newBoard->movePiece(m);
        int tempValue = search(newBoard, depth);
        value = std::max(value, tempValue);
    }
    return value;
}

int EngineV1::countMaterial(std::shared_ptr<Board> actualBoard, PieceColor myColor) {
    int material = 0;
    material += actualBoard->getPawnsCount(myColor) * PAWN_VALUE;
    material += actualBoard->getBishopsCount(myColor) * BISHOP_VALUE;
    material += actualBoard->getKnightsCount(myColor) * KNIGHT_VALUE;
    material += actualBoard->getRooksCount(myColor) * ROOK_VALUE;
    material += actualBoard->getQueensCount(myColor) * QUEEN_VALUE;
    material += actualBoard->getKingsCount(myColor) * KING_VALUE;
    return material;
}