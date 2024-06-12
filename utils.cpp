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

PieceMove algebraicToPieceMove(std::string& str, PieceMatrix& pm, PieceColor turn) {
    std::cout << "Algebraic to piece move" << std::endl;
    PieceMove move, errorMove;
    errorMove = PieceMove(PiecePos(0, 0), PiecePos(0, 0)); //This is the error move
    errorMove.promoteTo = NONE;
    move.promoteTo = NONE;
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

    //INFO: (explain it later) FIX

        //FIX: pop unwanted characters
    int it = str.size() - 1;
    if (str[it] == 'Q' || str[it] == 'R' || str[it] == 'B' || str[it] == 'N') {
        switch (str[it]) {
            case 'Q':
                move.promoteTo = (turn == WHITE) ? WHITE_QUEEN : BLACK_QUEEN;
                break;
            case 'R':
                move.promoteTo = (turn == WHITE) ? WHITE_ROOK : BLACK_ROOK;
                break;
            case 'B':
                move.promoteTo = (turn == WHITE) ? WHITE_BISHOP : BLACK_BISHOP;
                break;
            case 'N':
                move.promoteTo = (turn == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT;
                break;
        }
        --it;
        if (str[it] != '=') return errorMove;
        --it;
    }
    //Gets the destination square
    if (str[it] >= '0' && str[it] <= '8') {
        move.to.i = 8 - (str[it] - '0');
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

    //Have in mind, pawns take is written: [origin file]x[destination file][destination rank]
    std::cout << "PAWN MOVE" << std::endl;
    std::cout << "Move.to: " << move.to.i << " " << move.to.j << std::endl;

    
}