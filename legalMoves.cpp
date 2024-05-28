//This file contains the implementation of the functions that will be used to get the legal moves of the pieces

#include "board.hh"

void Board::getWhitePawnMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves) {
    PieceMove move;
    uint64_t *oponentPieces = (bit & whitePieces) ? &blackPieces : &whitePieces;
    uint64_t *oponentTargetedeSquares = (bit & whitePieces) ? &blackTargetedSquares : &whiteTargetedSquares;
    move.from = bitToij(bit);
    uint64_t aux1, aux2;
    aux1 = bit >> 8; //One square forward
    if (bit & ~RANK_8 && aux1 & ~allPieces) {
        move.to = bitToij(aux1);
        pieceLegalMoves.insert(move);
    }
    aux2 = bit >> 16; //Two squares forward
    if (bit & RANK_2 && aux1 & ~allPieces && aux2 & ~allPieces) {
        move.to = bitToij(aux2);
        pieceLegalMoves.insert(move);
    }
    aux1 = bit >> 9; //Capture right
    if (bit & ~H_FILE && bit & ~RANK_8) {
        *oponentTargetedeSquares |= aux1;
        if (aux1 & (*oponentPieces | enPassant)) {
            move.to = bitToij(aux1);
            pieceLegalMoves.insert(move);
        }
    }
    aux1 = bit >> 7; //Capture left
    if (bit & ~A_FILE && bit & ~RANK_8) {
        *oponentTargetedeSquares |= aux1;
        if (aux1 & (*oponentPieces | enPassant)) {
            move.to = bitToij(aux1);
            pieceLegalMoves.insert(move);
        }
    }
}


void Board::getBlackPawnMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves) {
    PieceMove move;
    uint64_t *oponentPieces = (bit & whitePieces) ? &blackPieces : &whitePieces;
    uint64_t *oponentTargetedeSquares = (bit & whitePieces) ? &blackTargetedSquares : &whiteTargetedSquares;
    uint64_t aux1, aux2;
    move.from = bitToij(bit);
    aux1 = bit << 8; //One square forward
    if (bit & ~RANK_1 && aux1 & ~allPieces) {
        move.to = bitToij(aux1);
        pieceLegalMoves.insert(move);
    }
    aux2 = bit << 16; //Two squares forward
    if (bit & RANK_7 && aux1 & ~allPieces && aux2 & ~allPieces) {
        move.to = bitToij(aux2);
        pieceLegalMoves.insert(move);
    }
    aux1 = bit << 7; //Capture right
    if (bit & ~H_FILE && bit & ~RANK_1) {
        *oponentTargetedeSquares |= aux1;
        if (aux1 & (*oponentPieces | enPassant)) {
            move.to = bitToij(aux1);
            pieceLegalMoves.insert(move);
        }
    }
    aux1 = bit << 9; //Capture left
    if (bit & ~A_FILE && bit & ~RANK_1) {
        *oponentTargetedeSquares |= aux1;
        if (aux1 & (*oponentPieces | enPassant)) {
            move.to = bitToij(aux1);
            pieceLegalMoves.insert(move);
        }
    }
}

void Board::getBishopMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves) {
    PieceMove move;
    move.from = bitToij(bit);
    uint64_t *myPieces = (bit & whitePieces) ? &whitePieces : &blackPieces;
    uint64_t *oponentPieces = (bit & whitePieces) ? &blackPieces : &whitePieces;
    uint64_t *oponentTargetedeSquares = (moveTurn == WHITE) ? &blackTargetedSquares : &whiteTargetedSquares;
    uint64_t *oponentKing = (bit & whitePieces) ? &blackKing : &blackPieces;
    uint64_t *oponentPinned = (bit & whitePieces) ? &blackPinnedSquares : &whitePinnedSquares;
    uint64_t pinned;
    int piecesFound;

    const int I_MOVE[4] = {1, 1, -1, -1};
    const int J_MOVE[4] = {-1, 1, -1, 1};
    for (int i = 0; i < 4; ++i) {
        piecesFound = 0;
        int newI = move.from.i;
        int newJ = move.from.j;
        while (piecesFound < 2) {
            newI += I_MOVE[i];
            newJ += J_MOVE[i];
            uint64_t aux;
            if (newI < 0 || newI > 7 || newJ < 0 || newJ > 7) 
                break;
            
            ijToBit(newI, newJ, aux);
            if (piecesFound == 0) *oponentTargetedeSquares |= aux;

            if (aux & ~*myPieces) {
                if (piecesFound == 0) {
                    move.to = bitToij(aux);
                    pieceLegalMoves.insert(move);
                    if (aux & *oponentPieces) {
                        pinned = aux;
                        ++piecesFound;
                    }
                }
                else if (aux & *oponentKing)
                    *oponentPinned |= pinned;
            }
            else break;
        }
    }
}

void Board::getKnightMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves) {
    PieceMove move;
    move.from = bitToij(bit);
    uint64_t *myPieces = (bit & whitePieces) ? &whitePieces : &blackPieces;
    uint64_t *oponentTargetedeSquares = (bit & whitePieces) ? &blackTargetedSquares : &whiteTargetedSquares;
    uint64_t aux;

    const int I_MOVE[8] = {2, 1, -1, -2, -2, -1, 1, 2};
    const int J_MOVE[8] = {1, 2, 2, 1, -1, -2, -2, -1};
    for (int i = 0; i < 8; ++i) {
        int newI = move.from.i + I_MOVE[i];
        int newJ = move.from.j + J_MOVE[i];
        if (newI >= 0 && newI < 8 && newJ >= 0 && newJ < 8) {
            ijToBit(newI, newJ, aux);
            *oponentTargetedeSquares |= aux;
            if (aux & ~*myPieces) {          
                move.to = bitToij(aux);
                pieceLegalMoves.insert(move);
            }
        }
    }
}

void Board::getRookMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves) {
    PieceMove move;
    move.from = bitToij(bit);
    uint64_t *myPieces = (bit & whitePieces) ? &whitePieces : &blackPieces;
    uint64_t *oponentPieces = (bit & whitePieces) ? &blackPieces : &whitePieces;
    uint64_t *oponentTargetedeSquares = (bit & whitePieces) ? &blackTargetedSquares : &whiteTargetedSquares;
    uint64_t *oponentKing = (bit & whitePieces) ? &blackKing : &blackPieces;
    uint64_t *oponentPinned = (bit & whitePieces) ? &blackPinnedSquares : &whitePinnedSquares;
    uint64_t pinned;
    int piecesFound;

    const int I_MOVE[4] = {1, 0, -1, 0};
    const int J_MOVE[4] = {0, 1, 0, -1};
    for (int i = 0; i < 4; ++i) {
        piecesFound = 0;
        int newI = move.from.i;
        int newJ = move.from.j;
        while (piecesFound < 2) {
            newI += I_MOVE[i];
            newJ += J_MOVE[i];
            uint64_t aux;
            if (newI < 0 || newI > 7 || newJ < 0 || newJ > 7) 
                break;
            
            ijToBit(newI, newJ, aux);
            if (piecesFound == 0) *oponentTargetedeSquares = *oponentTargetedeSquares | aux;

            if (aux & ~*myPieces) {
                if (piecesFound == 0) {
                    move.to = bitToij(aux);
                    pieceLegalMoves.insert(move);
                    if (aux & *oponentPieces) {
                        pinned = aux;
                        ++piecesFound;
                    }
                }
                else if (aux & *oponentKing)
                    *oponentPinned |= pinned;
            }
            else break;
        }
    }
}

void Board::getQueenMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves) {
    //Queen moves are the combination of rook and bishop moves
    getRookMoves(bit, pieceLegalMoves);
    getBishopMoves(bit, pieceLegalMoves);
}

void Board::getKingMoves(uint64_t& bit, std::set<PieceMove>& pieceLegalMoves) {
    PieceMove move;
    move.from = bitToij(bit);
    uint64_t *myPieces = (bit & whitePieces) ? &whitePieces : &blackPieces;
    uint64_t *myTargetedSquares = (bit & whitePieces) ? &whiteTargetedSquares : &blackTargetedSquares;
    uint64_t *oponentTargetedeSquares = (bit & whitePieces) ? &blackTargetedSquares : &whiteTargetedSquares;
    uint64_t aux;
    const int I_MOVE[8] = {1, 1, 1, 0, -1, -1, -1, 0};
    const int J_MOVE[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
    for (int i = 0; i < 8; ++i) {
        int newI = move.from.i + I_MOVE[i];
        int newJ = move.from.j + J_MOVE[i];
        if (newI >= 0 and newI < 8 and newJ >= 0 and newJ < 8) {
            ijToBit(newI, newJ, aux);
            *oponentTargetedeSquares |= aux;
            if (aux & ~*myPieces && aux & ~*myTargetedSquares) {          
                move.to = bitToij(aux);
                pieceLegalMoves.insert(move);
            }
        }
    }

    //Calculate castle moves
    if (bit & whitePieces) {
        //WHITE: castle king side (short castle)
        if (castleBitmap & 0x0200000000000000 && 
            whiteRook &0x0100000000000000 && 
            (allPieces & 0x0600000000000000) == 0 
            && (whiteTargetedSquares & 0x0e00000000000000) == 0) {
                move.to = {std::make_pair(7, 6)};
                pieceLegalMoves.insert(move);
                std::cout << "WHITE: Kingside castle" << std::endl;
        }
        //WHITE: castle queen side (long castle)
        if (castleBitmap & 0x2000000000000000 && 
            whiteRook &0x8000000000000000 && 
            (allPieces & 0x7000000000000000) == 0 
            && (whiteTargetedSquares & 0x3800000000000000) == 0) {
                move.to = {std::make_pair(7, 2)};
                pieceLegalMoves.insert(move);
                std::cout << "WHITE: Queenside castle" << std::endl;
        }
    }
    if (bit & blackPieces) {
        //BLACK: castle king side (short castle)
        if (castleBitmap & 0x0000000000000002 && 
            blackRook &0x0000000000000001 && 
            (allPieces & 0x0000000000000006) == 0 
            && (blackTargetedSquares & 0x000000000000000e) == 0) {
                move.to = {std::make_pair(0, 6)};
                pieceLegalMoves.insert(move);
                std::cout << "BLACK: Kingside castle" << std::endl;
        }
        //BLACK: castle queen side (long castle)
        if (castleBitmap & 0x0000000000000020 && 
            blackRook &0x0000000000000080 && 
            (allPieces & 0x0000000000000070) == 0 
            && (blackTargetedSquares & 0x0000000000000038) == 0) {
                move.to = {std::make_pair(0, 2)};
                pieceLegalMoves.insert(move);
                std::cout << "BLACK: Queenside castle" << std::endl;
        }
    }
}