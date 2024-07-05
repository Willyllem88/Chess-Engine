#ifndef PLAYER_HH
#define PLAYER_HH

#include "utils.hh"

class MyApp;
class Board;

class Player {
public:
    Player() {}
    virtual ~Player() {}

    //  Returns true if the player can make a move, false otherwise
    virtual bool canMove() = 0;

    //  Returns the move that the player wants to make
    virtual PieceMove getMove() = 0;

protected:
    std::shared_ptr<MyApp> app; //Pointer to the application
    std::shared_ptr<Board> board; //Pointer to the board
};

class HumanPlayer : public Player {
public:
    HumanPlayer(std::shared_ptr<MyApp> myApp);
    ~HumanPlayer() override {}

    bool canMove() override;
    PieceMove getMove() override;
};

class EngineRandom : public Player {
public:
    EngineRandom(std::shared_ptr<Board> myBoard) {
        board = myBoard;
    }
    ~EngineRandom() override {}

    bool canMove() override {
        return true;
    }
    PieceMove getMove() {
        const std::set<PieceMove> s = board->getCurrentLegalMoves();
        int random = rand() % s.size();
        auto it = s.begin();
        std::advance(it, random);
        //Wait for the delay
        //std::this_thread::sleep_for(std::chrono::milliseconds(250));
        return *it;
    }
};

class TranspositionTable {
public:
    struct transTableEntry {
        uint64_t zobristHash;
        uint32_t score; //Maybe uint16_t
        uint8_t depth;
        uint8_t nodeType;
    };

    static constexpr uint8_t NODE_TYPE_EXACT = 0;
    static constexpr uint8_t NODE_TYPE_LOWERBOUND = 1;
    static constexpr uint8_t NODE_TYPE_UPPERBOUND = 2;
    static constexpr uint8_t NODE_TYPE_QUIESCENCE = 3;


    bool contains(uint64_t zobristHash) {
        return transpositionTableBuffer[zobristHash % TRANSPOSITION_TABLE_MASK].zobristHash == zobristHash;
    }

    void insert(uint64_t zobristHash, uint32_t score, uint8_t depth, uint8_t node) {
        transpositionTableBuffer[zobristHash % TRANSPOSITION_TABLE_MASK] = {zobristHash, score, depth, node};
    }

    const transTableEntry* getEntry(uint64_t zobristHash) {
        return &transpositionTableBuffer[zobristHash % TRANSPOSITION_TABLE_MASK];
    }

private:
    static constexpr int TRANSPOSITION_TABLE_SIZE = 1 << 20;
    static constexpr int TRANSPOSITION_TABLE_MASK = TRANSPOSITION_TABLE_SIZE - 1;
    transTableEntry transpositionTableBuffer[TRANSPOSITION_TABLE_SIZE];
};

class EngineV1 : public Player {
public:
    EngineV1(std::shared_ptr<Board> myBoard, std::chrono::milliseconds timeSpan);
    ~EngineV1() override {}

    bool canMove() override;
    PieceMove getMove() override;

private:
    struct MoveEval {
        PieceMove move;
        int eval;
    };

    TranspositionTable transpositionTable;

    std::chrono::milliseconds moveDelay;

    bool searchTimeExceeded;

    EngineV1::MoveEval firstSearch(int depth);

    //  Recursive function that searches for the best move. Depth is the current depth of the search, alfa and beta are the bounds of the search.
    //  Negamax algorithm with alpha-beta pruning. For more information, visit:
    //      Negamax: [https://www.chessprogramming.org/Negamax]
    //      Alpha Beta Pruning: [https://www.chessprogramming.org/Alpha-Beta]
    int search(int depth, int alfa, int beta);

    //  Searches for a quiet position. A quiet position is a position where no captures are possible. Returns the value of the position.
    int quiescenceSearch(int alfa, int beta);

    //  Orders the moves in the list from best to worst. The ordering is done by the evaluation of the move. Helps the alpha-beta pruning.
    void orderMoves(const std::set<PieceMove>& moves, std::list<PieceMove>& orderedMoves);

    //  Evaluates the board. Returns the value of the board from white's perspective. Heuristic function.
    int evaluate();

    //  Returns the material value of the board from the perspective of the player with the color myColor
    int countMaterial(PieceColor myColor);

    //  Returns the positional value of the board from white's perspective
    int countPositionalValue();

    //  Returns true if the game is in the endgame, false otherwise
    bool isEndGame();

    
    static constexpr int MAX_DEPTH = 25;
    static constexpr int MAX_ITERATIONS = 1000000;
    static constexpr int INF = INT_MAX;

    static constexpr int PAWN_VALUE = 100;
    static constexpr int KNIGHT_VALUE = 320;
    static constexpr int BISHOP_VALUE = 330;
    static constexpr int ROOK_VALUE = 500;
    static constexpr int QUEEN_VALUE = 900;
    static constexpr int KING_VALUE = 20000;

    const int pawnEvals[8][8] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 25, 25, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0};
    const int knightEvals[8][8] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50};
    const int bishopEvals[8][8] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20};
    const int rookEvals[8][8] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        0,  0,  0,  5,  5,  0,  0,  0};  
    const int queenEvals[8][8] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -5,  0,  5,  5,  5,  5,  0, -5,
        0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20};
    const int kingEvalsMidGame[8][8] = {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        20, 20,  0,  0,  0,  0, 20, 20,
        20, 30, 10,  0,  0, 10, 30, 20
    };
    const int kingEvalEndGame[8][8] {
        -50,-40,-30,-20,-20,-30,-40,-50,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-30,  0,  0,  0,  0,-30,-30,
        -50,-30,-30,-30,-30,-30,-30,-50
    };          
};

#endif