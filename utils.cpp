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