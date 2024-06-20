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

EngineV1::EngineV1(std::shared_ptr<Board> myBoard) {
    board = myBoard;
}

bool EngineV1::canMove() {
    return true;
}

PieceMove EngineV1::getMove() {
    std::set<PieceMove> s = board->getCurrentLegalMoves();
    return *s.begin();
}