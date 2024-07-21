#include "utils.hh"

void errorAndExit(const std::string& errorMessage) {
    std::cerr << errorMessage << std::endl;
    std::exit(1);
}

bool isWhitePiece(PieceType p) {
    return p >= WHITE_PAWN && p <= WHITE_KING;
}

bool isBlackPiece(PieceType p) {
    return p >= BLACK_PAWN && p <= BLACK_KING;
}

uint64_t rand_uint64() {
    uint32_t a = rand();
    uint32_t b = rand();
    return ((uint64_t)a << 32) | b;
}

PieceColor pieceColor(PieceType p) {
    return (p >= WHITE_PAWN && p <= WHITE_KING) ? WHITE : BLACK;
}

//FIX: maybe put this into another file
std::string pieceToString(PieceType p) {
    switch (p) {
        case (WHITE_PAWN):
            return "White Pawn";
            break;
        case (WHITE_BISHOP):
            return "White Bishop";
            break;
        case (WHITE_KNIGHT):
            return "White Knight";
            break;
        case (WHITE_ROOK):
            return "White Rook";
            break;
        case (WHITE_QUEEN):
            return "White Queen";
            break;
        case (WHITE_KING):
            return "White King";
            break;
        
        case (BLACK_PAWN):
            return "Black Pawn";
            break;
        case (BLACK_BISHOP):
            return "Black Bishop";
            break;
        case (BLACK_KNIGHT):
            return "Black Knight";
            break;
        case (BLACK_ROOK):
            return "Black Rook";
            break;
        case (BLACK_QUEEN):
            return "Black Queen";
            break;
        case (BLACK_KING):  
            return "Black King";
            break;

        default:
            return "None";
            break;
    }
}

std::string pieceColorToString(PieceColor p) {
    if (p == WHITE) return "White";
    else return "Black";
}

std::string PieceMove::toString() {
    std::string str = "";
    str += (char)('a' + from.j);
    str += (char)('1' + 7 - from.i);
    str += " - ";
    str += (char)('a' + to.j);
    str += (char)('1' + 7 - to.i);
    if (promoteTo != NONE) {
        if (promoteTo == WHITE_QUEEN || promoteTo == BLACK_QUEEN) str += 'Q';
        else if (promoteTo == WHITE_ROOK || promoteTo == BLACK_ROOK) str += 'R';
        else if (promoteTo == WHITE_BISHOP || promoteTo == BLACK_BISHOP) str += 'B';
        else if (promoteTo == WHITE_KNIGHT || promoteTo == BLACK_KNIGHT) str += 'N';
    }
    return str;
}


bool readStringFromConsole(std::string& str) {
    fd_set set;
    struct timeval timeout;

    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    int rv = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);
    if(rv == -1) {
        perror("select"); // Error occurred in select()
        return false;
    } else if(rv == 0) {
        return false; // No data to read
    } else {
        char ch;
        while (read(STDIN_FILENO, &ch, 1) == 1 && !isspace(ch)) {
            str += ch;
        }
        return true;
    }
}

PieceMove algebraicToPieceMove(std::string& str, const std::set<PieceMove>& legalMoves, const PieceMatrix& pm, PieceColor turn) {
    PieceMove move, errorMove;
    errorMove = PieceMove(PiecePos(0, 0), PiecePos(0, 0)); //This is the error move
    errorMove.promoteTo = NONE;
    move.promoteTo = NONE;

    //Checks for short castling
    if (str == "O-O") {
        if (turn == WHITE) {
            move.from = PiecePos(7, 4);
            move.to = PiecePos(7, 6);
        } else {
            move.from = PiecePos(0, 4);
            move.to = PiecePos(0, 6);
        }
        return move;
    }
    //Checks for long castling
    else if (str == "O-O-O") {
        if (turn == WHITE) {
            move.from = PiecePos(7, 4);
            move.to = PiecePos(7, 2);
        } else {
            move.from = PiecePos(0, 4);
            move.to = PiecePos(0, 2);
        }
        return move;
    }

    //Explaination: the string will be read from right to left
    int it = str.size() - 1;

    //Iterate through the + and # characters
    if (str[it] == '+' || str[it] == '#')
        --it;

    //Checks if the string represents a promotion
    if (str[it] == 'Q' || str[it] == 'R' || str[it] == 'B' || str[it] == 'N') {
        if (str[it] == 'Q') move.promoteTo = (turn == WHITE) ? WHITE_QUEEN : BLACK_QUEEN;
        else if (str[it] == 'R') move.promoteTo = (turn == WHITE) ? WHITE_ROOK : BLACK_ROOK;
        else if (str[it] == 'B') move.promoteTo = (turn == WHITE) ? WHITE_BISHOP : BLACK_BISHOP;
        else if (str[it] == 'N') move.promoteTo = (turn == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT;
        --it;
        if (str[it] != '=') return errorMove;
        --it;
    }
    
    //Gets the destination square
    if (str[it] >= '1' && str[it] <= '8') {
        move.to.i = 7 - (str[it] - '1');
        --it;
    } 
    else return errorMove;
    if (str[it] >= 'a' && str[it] <= 'h') {
        move.to.j = str[it] - 'a';
        --it;
    } 
    else return errorMove;

    //Examines if takes
    if (str[it] == 'x') {
        --it;
    }

    PieceType movedPieceType = NONE;
    char file = '0', rank = '0';

    if (it == -1)
        movedPieceType = (turn == WHITE) ? WHITE_PAWN : BLACK_PAWN;
    else {
        //Checks for unambiguous move information (file, rank or both)
        if (str[it] >= '1' && str[it] <= '8') {
            rank = str[it];
            --it;
            if (str[it] >= 'a' && str[it] <= 'h') {
                file = str[it];
                --it;
            } 
        }
        else if (str[it] >= 'a' && str[it] <= 'h') {
            file = str[it];
            --it;
        }

        //Gets the piece type
        if (it == -1) {
            movedPieceType = (turn == WHITE) ? WHITE_PAWN : BLACK_PAWN;
        }
        else if (it == 0) {
            if (str[it] == 'K') movedPieceType = (turn == WHITE) ? WHITE_KING : BLACK_KING;
            else if (str[it] == 'Q') movedPieceType = (turn == WHITE) ? WHITE_QUEEN : BLACK_QUEEN;
            else if (str[it] == 'R') movedPieceType = (turn == WHITE) ? WHITE_ROOK : BLACK_ROOK;
            else if (str[it] == 'B') movedPieceType = (turn == WHITE) ? WHITE_BISHOP : BLACK_BISHOP;
            else if (str[it] == 'N') movedPieceType = (turn == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT;
            else return errorMove;
        }
        else return errorMove;
    }

    bool found = false;

    //For each legal move, we will check if it satisfies the conditions
    for (auto it : legalMoves) {
        if (it.to == move.to && it.promoteTo == move.promoteTo) {
            //If the move is ambiguous, it will check the file and rank
            if (file != '0' && it.from.j != file - 'a') continue;
            if (rank != '0' && it.from.i != 7 - (rank - '1')) continue;

            //We will check if the piece is the same
            if (pm[it.from.i][it.from.j] != movedPieceType) continue;

            //If we find two moves that satisfy the conditions, we will return an error
            if (found) {
                std::cout << "[ERROR] Ambiguous move!" << std::endl;
                return errorMove;
            }

            move.from.i = it.from.i;
            move.from.j = it.from.j;
            found = true;
        }
    }

    if (!found) return errorMove;

    return move;
}

std::string pieceMoveToAlgebraic(const PieceMove& move, const PieceMatrix& pm, const std::set<PieceMove>& legalMoves, const std::string& lastChar) {
    PieceType pt = pm[move.from.i][move.from.j];
    std::string str = "";
    if (pt == WHITE_KING || pt == BLACK_KING) {
        if (move.from.j == 4 && move.to.j == 6) return "O-O" + lastChar;
        if (move.from.j == 4 && move.to.j == 2) return "O-O-O" + lastChar;
        str += 'K';
    }
    else if (pt == WHITE_PAWN || pt == BLACK_PAWN) {
        if (move.from.j != move.to.j) {
            str += (char)('a' + move.from.j);
            str += 'x';
        }
        str += (char)('a' + move.to.j);
        str += (char)('1' + 7 - move.to.i);
        if (move.promoteTo != NONE) {
            str += '=';
            if (move.promoteTo == WHITE_QUEEN || move.promoteTo == BLACK_QUEEN) str += 'Q';
            else if (move.promoteTo == WHITE_ROOK || move.promoteTo == BLACK_ROOK) str += 'R';
            else if (move.promoteTo == WHITE_BISHOP || move.promoteTo == BLACK_BISHOP) str += 'B';
            else if (move.promoteTo == WHITE_KNIGHT || move.promoteTo == BLACK_KNIGHT) str += 'N';
        }
        return str + lastChar;
    }
    else if (pt == WHITE_QUEEN || pt == BLACK_QUEEN) str += 'Q';
    else if (pt == WHITE_ROOK || pt == BLACK_ROOK) str += 'R';
    else if (pt == WHITE_BISHOP || pt == BLACK_BISHOP) str += 'B';
    else if (pt == WHITE_KNIGHT || pt == BLACK_KNIGHT) str += 'N';

    //Checks for disambiguation
    std::string file, rank;
    file = rank = "";
    for (auto it : legalMoves) {
        //If the move is ambiguous (same destination square, different origin square, same piece type)
        if (pm[it.from.i][it.from.j] == pt && it.to == move.to && it.from != move.from) {
            if (it.from.i == move.from.i) file += (char)('a' + it.from.j);
            else if (it.from.j == move.from.j) rank += (char)('1' + 7 - it.from.i);
        }
    }
    str += file + rank;

    if (pm[move.to.i][move.to.j] != NONE) str += 'x';

    str += (char)('a' + move.to.j);
    str += (char)('1' + 7 - move.to.i);

    str += lastChar; //Adds the last character, may be +, 

    return str;
}