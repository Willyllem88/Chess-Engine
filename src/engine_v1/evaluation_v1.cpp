#include "engine_v1.hpp"
#include "board.hpp"

int EngineV1::evaluate() {
    int whiteEval = countMaterial(WHITE);
    int blackEval = countMaterial(BLACK);

    float whiteEndGamePhase = currentEndGamePhaseWeight(WHITE);
    float blackEndGamePhase = currentEndGamePhaseWeight(BLACK);

    int whitePositionalValue = countPositionalValue(WHITE, whiteEndGamePhase);
    int blackPositionalValue = countPositionalValue(BLACK, blackEndGamePhase);

    int evaluation = whiteEval - blackEval + whitePositionalValue - blackPositionalValue;

    int perspective = (board->getMoveTurn() == WHITE) ? 1 : -1;
    return evaluation * perspective;
}

float EngineV1::currentEndGamePhaseWeight(PieceColor myColor) {
    //Based on heuristics: the less major pieces, the more endgame it is, we exclude the pawns. We will consider the endgame when the material is less than endgameMaterialStart (2 Rooks + Knight + Bishop)
    const float multiplier = 1.0f / static_cast<float>(endgameMaterialStart);
    int materialWithoutPawns = countMaterial(myColor) - board->getPawnsCount(myColor) * PAWN_VALUE;
    return 1.0f - std::min(1.0f, multiplier * materialWithoutPawns);
}

int EngineV1::countPositionalValue(PieceColor myColor, float myEndGamePhase) {
    int positionalValue = 0;
    PieceMatrix pm = board->getPieceMatrix();
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            PieceType p = pm[i][j];

            //If the piece is not mine, we continue
            if (p == NONE || pieceColor(p) != myColor) continue;

            switch (p) {
                case WHITE_PAWN:
                    positionalValue += pawnEvals[i][j];
                    break;
                case BLACK_PAWN:
                    positionalValue -= pawnEvals[7-i][7-j];
                    break;
                case WHITE_KNIGHT:
                    positionalValue += knightEvals[i][j];
                    break;
                case BLACK_KNIGHT:
                    positionalValue -= knightEvals[7-i][7-j];
                    break;
                case WHITE_BISHOP:
                    positionalValue += bishopEvals[i][j];
                    break;
                case BLACK_BISHOP:
                    positionalValue -= bishopEvals[7-i][7-j];
                    break;
                case WHITE_ROOK:
                    positionalValue += rookEvals[i][j];
                    break;
                case BLACK_ROOK:
                    positionalValue -= rookEvals[7-i][7-j];
                    break;
                case WHITE_QUEEN:
                    positionalValue += queenEvals[i][j];
                    break;
                case BLACK_QUEEN:
                    positionalValue -= queenEvals[7-i][7-j];
                    break;
                case WHITE_KING:
                    positionalValue += kingEvalsMidGame[i][j]*(1.0f-myEndGamePhase) + kingEvalsEndGame[i][j]*myEndGamePhase;
                    break;
                case BLACK_KING:
                    positionalValue -= kingEvalsMidGame[7-i][7-j]*(1.0f-myEndGamePhase) + kingEvalsEndGame[7-i][7-j]*myEndGamePhase;
                    break;
                default:
                    break;
            }
        }
    }

    return positionalValue;
}

int EngineV1::countMaterial(PieceColor myColor) {
    int material = 0;
    material += board->getPawnsCount(myColor) * PAWN_VALUE;
    material += board->getBishopsCount(myColor) * BISHOP_VALUE;
    material += board->getKnightsCount(myColor) * KNIGHT_VALUE;
    material += board->getRooksCount(myColor) * ROOK_VALUE;
    material += board->getQueensCount(myColor) * QUEEN_VALUE;
    return material;
}