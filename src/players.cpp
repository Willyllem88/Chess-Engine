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

RandomEngine::RandomEngine(std::shared_ptr<Board> myBoard) {
    board = myBoard;
}

bool RandomEngine::canMove() {
    return true;
}

PieceMove RandomEngine::getMove() {
    interrupted = false;
    const std::set<PieceMove> s = board->getCurrentLegalMoves();
    int random = rand() % s.size();
    auto it = s.begin();
    std::advance(it, random);
    //IF NEEDED: Wait for the delay
    //std::this_thread::sleep_for(std::chrono::milliseconds(250));
    return *it;
}

HumanPlayer::HumanPlayer(std::shared_ptr<MyApp> myApp) {
    app = myApp;
}

bool HumanPlayer::canMove() {
    return app->isPieceMoveAvailable();
}

PieceMove HumanPlayer::getMove() {
    interrupted = false;
    return app->getMove();
}