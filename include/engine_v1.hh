#ifndef ENGINEV1_HH
#define ENGINEV1_HH

#include "players.hh"

class TranspositionTable {
public:
    struct transTableEntry {
        uint64_t zobristHash;
        uint32_t score; //Maybe uint16_t
        uint8_t depth;
        uint8_t nodeType;
    };

    static constexpr uint8_t NT_EXACT = 0;
    static constexpr uint8_t NT_LOWERBOUND = 1;
    static constexpr uint8_t NT_UPPERBOUND = 2;
    static constexpr uint8_t NT_QUIESCENCE = 3;

    void clear() {
        memset(transpositionTableBuffer, 0, sizeof(transpositionTableBuffer));
    }


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

        bool operator<(const MoveEval& other) const {
            return eval < other.eval;
        }
        bool operator>(const MoveEval& other) const {
            return eval > other.eval;
        }
    };

    TranspositionTable transpositionTable;

    //  Time related variables, the more time the engine has, the better the move it will make
    //      The atomic variables are needed because they will be accessed by different threads
    std::chrono::milliseconds moveDelay; //The time the engine has to make a move
    std::atomic<bool> searchTimeExceeded; //True if the time has exceeded
    std::atomic<bool> stopTimer; //True if the timer has to stop

    //  The purpose of these variables is to keep information of a search.
    int numBoards; //Number of boards evaluated in the search
    int transpositionHits; //Number of transposition table hits

    //  For each depth in iterative deepening, it will search for the best move. Returns the best move and its evaluation. This is the first search for the different depths. This function will call the search function.
    //      Iterative Deepening: [https://en.wikipedia.org/wiki/Iterative_deepening_depth-first_search]
    std::vector<EngineV1::MoveEval> firstSearch(const std::vector<PieceMove>& orderedMoves, int depth);

    //  Recursive function that searches for the best move. Depth is the current depth of the search, alfa and beta are the bounds of the search.
    //  Negamax algorithm with alpha-beta pruning. For more information, visit:
    //      - Negamax: [https://www.chessprogramming.org/Negamax]
    //      - Alpha Beta Pruning: [https://www.chessprogramming.org/Alpha-Beta]
    int search(int depth, int alfa, int beta);

    //  Searches for a quiet position. A quiet position is a position where no captures are possible. Returns the value of the position.
    int quiescenceSearch(int alfa, int beta);

    //  Orders the moves in the list from best to worst. The ordering is done by the evaluation of the move. Helps the alpha-beta pruning.
    void orderMoves(const std::set<PieceMove>& moves, std::list<PieceMove>& orderedMoves);

    //  Evaluates the board. Returns the value of the board from white's perspective. Heuristic function.
    int evaluate();

    //  Returns a value between 0 and 1 that represents the current endgame phase of the player with the color myColor. The closer to 1, the closer to the endgame.
    float currentEndGamePhaseWeight(PieceColor myColor);

    //  Returns the material value of the board from the perspective of the player with the color myColor
    int countMaterial(PieceColor myColor);

    //  Returns the positional value of the board from white's perspective
    int countPositionalValue(PieceColor myColor, float myEndGamePhase);

    void iniTimer(std::chrono::milliseconds timeSpan);

    
    static constexpr int MAX_DEPTH = 50;
    static constexpr int INF = 1000000;

    static constexpr int PAWN_VALUE = 100;
    static constexpr int KNIGHT_VALUE = 320;
    static constexpr int BISHOP_VALUE = 330;
    static constexpr int ROOK_VALUE = 500;
    static constexpr int QUEEN_VALUE = 900;

    //  The start of the endgame is when the material is less than endgameMaterialStart
    static constexpr int endgameMaterialStart = 1650; // 1650 = 2*ROOK + KINGHT + BISHOP

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
    const int kingEvalsEndGame[8][8] {
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