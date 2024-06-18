#include "utils.hh"

bool isWhitePiece(PieceType p) {
    return p >= WHITE_PAWN && p <= WHITE_KING;
}

bool isBlackPiece(PieceType p) {
    return p >= BLACK_PAWN && p <= BLACK_KING;
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

    //Checks if the string represents a promotion
    int it = str.size() - 1;
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
                std::cout << "Ambiguous move!" << std::endl;
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