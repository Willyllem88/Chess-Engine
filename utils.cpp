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

        case NONE:
            return "None";
            break;
    }
}