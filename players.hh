#ifndef PLAYER_HH
#define PLAYER_HH

#include "utils.hh"

class MyApp;
class Board;

class Player {
public:
    Player() {}
    virtual ~Player() {}

    virtual bool canMove() = 0;
    virtual PieceMove getMove() = 0;

protected:
    std::shared_ptr<MyApp> app;
    std::shared_ptr<Board> board;
};

class HumanPlayer : public Player {
public:
    HumanPlayer(std::shared_ptr<MyApp> myApp);
    ~HumanPlayer() override {}

    bool canMove() override;
    PieceMove getMove() override;
};

class EngineV1 : public Player {
public:
    EngineV1(std::shared_ptr<Board> myBoard);
    ~EngineV1() override {}

    bool canMove() override;
    PieceMove getMove() override;
};

#endif