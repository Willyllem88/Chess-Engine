#ifndef PLAYER_HH
#define PLAYER_HH

#include "utils.hpp"

class MyApp;
class Board;

class Player {
public:
    Player();
    virtual ~Player() {}

    //  Returns true if the player can make a move, false otherwise
    virtual bool canMove() = 0;

    //  Returns the move that the player wants to make
    virtual PieceMove getMove() = 0;

    virtual void interrupt();
    bool wasInterrupted();

protected:
    std::shared_ptr<MyApp> app; //Pointer to the application
    std::shared_ptr<Board> board; //Pointer to the board

    std::atomic<bool> interrupted; //True if the player has been interrupted
};

class HumanPlayer : public Player {
public:
    HumanPlayer(std::shared_ptr<MyApp> myApp);
    ~HumanPlayer() override {}

    bool canMove() override;
    PieceMove getMove() override;
};

class RandomEngine : public Player {
public:
    RandomEngine(std::shared_ptr<Board> myBoard);
    ~RandomEngine() override {}

    bool canMove() override;
    PieceMove getMove();
};

#endif