//This file contains the implementation of the functions that will be used to get the legal moves of the pieces

#include "board.hh"

void Board::getWhitePawnLegalMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves) {
    PieceMove move;
    move.from = bitToij(bit);
    uint64_t aux;
    aux = bit >> 8; //One square forward
    if (aux & ~allPieces) {
        move.to = bitToij(aux);
        pieceLegalMoves.insert(move);
    }
    aux = bit >> 16; //Two squares forward
    if (aux & ~allPieces && bit & 0x00ff000000000000) {
        move.to = bitToij(aux);
        pieceLegalMoves.insert(move);
    }
    aux = bit >> 9; //Capture right
    if (aux & blackPieces) {
        move.to = bitToij(aux);
        pieceLegalMoves.insert(move);
    }
    aux = bit >> 7; //Capture light
    if (aux & blackPieces) {
        move.to = bitToij(aux);
        pieceLegalMoves.insert(move);
    }
}


void Board::getBlackPawnLegalMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves) {
    PieceMove move;
    move.from = bitToij(bit);
    uint64_t aux;
    aux = bit << 8; //One square forward
    if (aux & ~allPieces) {
        move.to = bitToij(aux);
        pieceLegalMoves.insert(move);
    }
    aux = bit << 16; //Two squares forward
    if (aux & ~allPieces && bit & 0x000000000000ff00) {
        move.to = bitToij(aux);
        pieceLegalMoves.insert(move);
    }
    aux = bit << 9; //Capture right
    if (aux & whitePieces) {
        move.to = bitToij(aux);
        pieceLegalMoves.insert(move);
    }
    aux = bit << 7; //Capture light
    if (aux & whitePieces) {
        move.to = bitToij(aux);
        pieceLegalMoves.insert(move);
    }
}

void Board::getBishopLegalMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves) {
    PieceMove move;
    move.from = bitToij(bit);
    uint64_t *targetBitmap = (bit & whitePieces) ? &whitePieces : &blackPieces;
    uint64_t aux;
    //move up-right
    aux = bit;
    while (aux & ~H_FILE or aux & ~RANK_8) {
        aux = aux >> 9;
        if (aux & ~*targetBitmap) {
            move.to = bitToij(aux);
            pieceLegalMoves.insert(move);
        }
        if (aux & allPieces) break;
    }
    //move up-left
    aux = bit;
    while (aux & ~A_FILE or aux & ~RANK_8) {
        aux = aux >> 7;
        if (aux & ~*targetBitmap) {
            move.to = bitToij(aux);
            pieceLegalMoves.insert(move);
        }
        if (aux & allPieces) break;
    }
    //move down-right
    aux = bit;
    while (aux & ~H_FILE or aux & ~RANK_1) {
        aux = aux << 7;
        if (aux & ~*targetBitmap) {
            move.to = bitToij(aux);
            pieceLegalMoves.insert(move);
        }
        if (aux & allPieces) break;
    }
    //move down-left
    aux = bit;
    while (aux & ~A_FILE or aux & ~RANK_1) {
        aux = aux << 9;
        if (aux & ~*targetBitmap) {
            move.to = bitToij(aux);
            pieceLegalMoves.insert(move);
        }
        if (aux & allPieces) break;
    }
}

void Board::getKnightLegalMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves) {
    PieceMove move;
    move.from = bitToij(bit);
    uint64_t *targetBitmap = (bit & whitePieces) ? &whitePieces : &blackPieces;
    uint64_t aux;
    //move 2 up, 1 right
    const int I_MOVE[8] = {2, 1, -1, -2, -2, -1, 1, 2};
    const int J_MOVE[8] = {1, 2, 2, 1, -1, -2, -2, -1};
    for (int i = 0; i < 8; ++i) {
        int newI = move.from.i + I_MOVE[i];
        int newJ = move.from.j + J_MOVE[i];
        if (newI >= 0 and newI < 8 and newJ >= 0 and newJ < 8) {
            ijToBit(newI, newJ, aux);
            if (aux & ~*targetBitmap) {
                move.to = bitToij(aux);
                std::cout << "Move to " << move.to.i << " " << move.to.j << "\n";
                pieceLegalMoves.insert(move);
            }
        }
    }
}

void Board::getRookLegalMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves) {
    PieceMove move;
    move.from = bitToij(bit);
    uint64_t *targetBitmap = (bit & whitePieces) ? &whitePieces : &blackPieces;
    uint64_t aux;
    //move up
    aux = bit;
    while (aux & ~RANK_8) {
        aux = aux >> 8;
        if (aux & ~*targetBitmap) {
            move.to = bitToij(aux);
            pieceLegalMoves.insert(move);
        }
        if (aux & allPieces) break;
    }
    //move down
    aux = bit;
    while (aux & ~RANK_1) {
        aux = aux << 8;
        if (aux & ~*targetBitmap) {
            move.to = bitToij(aux);
            pieceLegalMoves.insert(move);
        }
        if (aux & allPieces) break;
    }
    //move right
    aux = bit;
    while (aux & ~H_FILE) {
        aux = aux >> 1;
        if (aux & ~*targetBitmap) {
            move.to = bitToij(aux);
            pieceLegalMoves.insert(move);
        }
        if (aux & allPieces) break;
    }
    //move left
    aux = bit;
    while (aux & ~A_FILE) {
        aux = aux << 1;
        if (aux & ~*targetBitmap) {
            move.to = bitToij(aux);
            pieceLegalMoves.insert(move);
        }
        if (aux & allPieces) break;
    }
}

void Board::getQueenLegalMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves) {
    //Queen moves are the combination of rook and bishop moves
    getRookLegalMoves(bit, pieceLegalMoves);
    getBishopLegalMoves(bit, pieceLegalMoves);
}
