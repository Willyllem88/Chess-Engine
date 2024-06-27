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
    EngineV1(std::shared_ptr<Board> myBoard, std::chrono::milliseconds timeSpan);
    ~EngineV1() override {}

    bool canMove() override;
    PieceMove getMove() override;

private:
    int evaluate();
    int search(std::shared_ptr<Board> board, int depth);
    int countMaterial(std::shared_ptr<Board> actualBoard, PieceColor myColor);

    
    static constexpr int MAX_DEPTH = 10;
    static constexpr int INF = INT_MAX;

    static constexpr int PAWN_VALUE = 100;
    static constexpr int KNIGHT_VALUE = 320;
    static constexpr int BISHOP_VALUE = 330;
    static constexpr int ROOK_VALUE = 500;
    static constexpr int QUEEN_VALUE = 900;
    static constexpr int KING_VALUE = 20000;

    std::chrono::milliseconds moveDelay;
};

#endif