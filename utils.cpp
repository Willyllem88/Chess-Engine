#include "utils.hh"

//FIX: maybe put this into another file
std::string pieceToString(pieceType p) {
    switch (p) {
        case (WHITE_PAWN):
            return "White Pawn";
        case (BLACK_PAWN):
            return "Black Pawn";
        // (...)
        default:
            return "";
    }
}