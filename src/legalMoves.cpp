//This file contains the implementation of the functions that will be used to get the legal moves of the pieces

#include "board.hpp"

void Board::getWhitePawnMoves(uint64_t bit, std::set<PieceMove>& pieceMoves) {
    PieceMove move;
    move.promoteTo = NONE;
    uint64_t *opponentPieces = (bit & whitePieces) ? &blackPieces : &whitePieces;
    move.from = bitToij(bit);
    //Detect Promotion
    uint64_t aux1, aux2;
    aux1 = bit >> 8; //One square forward
    if (aux1 & ~allPieces) {
        move.to = bitToij(aux1);
        if (aux1 & ~RANK_8)
            pieceMoves.insert(move);
        //Promotion
        else
            promoteWhitePawn(pieceMoves, move);
    }
    aux2 = bit >> 16; //Two squares forward
    if (bit & RANK_2 && aux1 & ~allPieces && aux2 & ~allPieces) {
        move.to = bitToij(aux2);
        pieceMoves.insert(move);
    }
    aux1 = bit >> 9; //Capture right
    if (bit & ~H_FILE) {
        if (aux1 & (*opponentPieces | enPassant)) {
            move.to = bitToij(aux1);
            pieceMoves.insert(move);
            //Promotion
            if (aux1 & RANK_8)
                promoteWhitePawn(pieceMoves, move);
        }
    }
    aux1 = bit >> 7; //Capture left
    if (bit & ~A_FILE) {
        if (aux1 & (*opponentPieces | enPassant)) {
            move.to = bitToij(aux1);
            pieceMoves.insert(move);
            //Promotion
            if (aux1 & RANK_8)
                promoteWhitePawn(pieceMoves, move);
        }
    }
}

void Board::getBlackPawnMoves(uint64_t bit, std::set<PieceMove>& pieceMoves) {
    PieceMove move;
    move.promoteTo = NONE;
    uint64_t *opponentPieces = (bit & whitePieces) ? &blackPieces : &whitePieces;
    uint64_t aux1, aux2;
    move.from = bitToij(bit);
    aux1 = bit << 8; //One square forward
    if (aux1 & ~allPieces) {
        move.to = bitToij(aux1);
        if (aux1 & ~RANK_1)
            pieceMoves.insert(move);
        //Promotion
        else
            promoteBlackPawn(pieceMoves, move);
    }
    aux2 = bit << 16; //Two squares forward
    if (bit & RANK_7 && aux1 & ~allPieces && aux2 & ~allPieces) {
        move.to = bitToij(aux2);
        pieceMoves.insert(move);
    }
    aux1 = bit << 7; //Capture right
    if (bit & ~H_FILE) {
        if (aux1 & (*opponentPieces | enPassant)) {
            move.to = bitToij(aux1);
            pieceMoves.insert(move);
            //Promotion
            if (aux1 & RANK_1)
                promoteBlackPawn(pieceMoves, move);
        }
    }
    aux1 = bit << 9; //Capture left
    if (bit & ~A_FILE) {
        if (aux1 & (*opponentPieces | enPassant)) {
            move.to = bitToij(aux1);
            pieceMoves.insert(move);
            //Promotion
            if (aux1 & RANK_1)
                promoteBlackPawn(pieceMoves, move);
        }
    }
}

void Board::getBishopMoves(uint64_t bit, std::set<PieceMove>& pieceMoves) {
    PieceMove move;
    move.promoteTo = NONE;
    move.from = bitToij(bit);
    uint64_t *myPieces = (bit & whitePieces) ? &whitePieces : &blackPieces;
    uint64_t *opponentPieces = (bit & whitePieces) ? &blackPieces : &whitePieces;
    bool pieceFound;

    const int I_MOVE[4] = {1, 1, -1, -1};
    const int J_MOVE[4] = {-1, 1, -1, 1};
    for (int i = 0; i < 4; ++i) {
        pieceFound = false;
        int newI = move.from.i + I_MOVE[i];
        int newJ = move.from.j + J_MOVE[i];
        //While there are no pieces in the way and the new square is inside the board
        while (!pieceFound && (newI >= 0 && newI < 8 && newJ >= 0 && newJ < 8)) {
            uint64_t aux;
            ijToBit(newI, newJ, aux);
            if (aux & ~*myPieces) {
                move.to = bitToij(aux);
                pieceMoves.insert(move);
                if (aux & *opponentPieces)
                    pieceFound = true;
            }
            else pieceFound = true;;

            newI += I_MOVE[i];
            newJ += J_MOVE[i];
        }
    }
}

void Board::getKnightMoves(uint64_t bit, std::set<PieceMove>& pieceMoves) {
    PieceMove move;
    move.promoteTo = NONE;
    move.from = bitToij(bit);
    uint64_t *myPieces = (bit & whitePieces) ? &whitePieces : &blackPieces;
    uint64_t aux;

    const int I_MOVE[8] = {2, 1, -1, -2, -2, -1, 1, 2};
    const int J_MOVE[8] = {1, 2, 2, 1, -1, -2, -2, -1};
    for (int i = 0; i < 8; ++i) {
        int newI = move.from.i + I_MOVE[i];
        int newJ = move.from.j + J_MOVE[i];
        if (newI >= 0 && newI < 8 && newJ >= 0 && newJ < 8) {
            ijToBit(newI, newJ, aux);
            if (aux & ~*myPieces) {          
                move.to = bitToij(aux);
                pieceMoves.insert(move);
            }
        }
    }
}

void Board::getRookMoves(uint64_t bit, std::set<PieceMove>& pieceMoves) {
    PieceMove move;
    move.promoteTo = NONE;
    move.from = bitToij(bit);
    uint64_t *myPieces = (bit & whitePieces) ? &whitePieces : &blackPieces;
    uint64_t *opponentPieces = (bit & whitePieces) ? &blackPieces : &whitePieces;
    bool pieceFound;

    const int I_MOVE[4] = {1, 0, -1, 0};
    const int J_MOVE[4] = {0, 1, 0, -1};
    for (int i = 0; i < 4; ++i) {
        pieceFound = false;
        int newI = move.from.i + I_MOVE[i];
        int newJ = move.from.j + J_MOVE[i];
        //While there are no pieces in the way and the new square is inside the board
        while (!pieceFound && (newI >= 0 && newI < 8 && newJ >= 0 && newJ < 8)) {
            uint64_t aux;
            ijToBit(newI, newJ, aux);
            if (aux & ~*myPieces) {
                move.to = bitToij(aux);
                pieceMoves.insert(move);
                if (aux & *opponentPieces)
                    pieceFound = true;
            }
            else pieceFound = true;;

            newI += I_MOVE[i];
            newJ += J_MOVE[i];
        }
    }
}

void Board::getQueenMoves(uint64_t bit, std::set<PieceMove>& pieceMoves) {
    //Queen moves are the combination of rook and bishop moves
    getRookMoves(bit, pieceMoves);
    getBishopMoves(bit, pieceMoves);
}

void Board::getKingMoves(uint64_t bit, std::set<PieceMove>& pieceMoves) {
    PieceMove move;
    move.promoteTo = NONE;
    move.from = bitToij(bit);
    uint64_t *myPieces = (bit & whitePieces) ? &whitePieces : &blackPieces;
    uint64_t *myTargetedSquares = (bit & whitePieces) ? &whiteTargetedSquares : &blackTargetedSquares;
    uint64_t *opponentTargetedeSquares = (bit & whitePieces) ? &blackTargetedSquares : &whiteTargetedSquares;
    uint64_t aux;
    const int I_MOVE[8] = {1, 1, 1, 0, -1, -1, -1, 0};
    const int J_MOVE[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
    for (int i = 0; i < 8; ++i) {
        int newI = move.from.i + I_MOVE[i];
        int newJ = move.from.j + J_MOVE[i];
        if (newI >= 0 and newI < 8 and newJ >= 0 and newJ < 8) {
            ijToBit(newI, newJ, aux);
            *opponentTargetedeSquares |= aux;
            if (aux & ~*myPieces && aux & ~*myTargetedSquares) {          
                move.to = bitToij(aux);
                pieceMoves.insert(move);
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
                pieceMoves.insert(move);
        }
        //WHITE: castle queen side (long castle)
        if (castleBitmap & 0x2000000000000000 && 
            whiteRook &0x8000000000000000 && 
            (allPieces & 0x7000000000000000) == 0 
            && (whiteTargetedSquares & 0x3800000000000000) == 0) {
                move.to = {std::make_pair(7, 2)};
                pieceMoves.insert(move);
        }
    }
    if (bit & blackPieces) {
        //BLACK: castle king side (short castle)
        if (castleBitmap & 0x0000000000000002 && 
            blackRook & 0x0000000000000001 && 
            (allPieces & 0x0000000000000006) == 0 
            && (blackTargetedSquares & 0x000000000000000e) == 0) {
                move.to = {std::make_pair(0, 6)};
                pieceMoves.insert(move);
        }
        //BLACK: castle queen side (long castle)
        if (castleBitmap & 0x0000000000000020 && 
            blackRook &0x0000000000000080 && 
            (allPieces & 0x0000000000000070) == 0 
            && (blackTargetedSquares & 0x0000000000000038) == 0) {
                move.to = {std::make_pair(0, 2)};
                pieceMoves.insert(move);
        }
    }
}

void Board::promoteWhitePawn(std::set<PieceMove>& pieceMoves, PieceMove& move) {
    move.promoteTo = WHITE_QUEEN;
    pieceMoves.insert(move);
    move.promoteTo = WHITE_ROOK;
    pieceMoves.insert(move);
    move.promoteTo = WHITE_BISHOP;
    pieceMoves.insert(move);
    move.promoteTo = WHITE_KNIGHT;
    pieceMoves.insert(move);
    move.promoteTo = NONE;
}

void Board::promoteBlackPawn(std::set<PieceMove>& pieceMoves, PieceMove& move) {
    move.promoteTo = BLACK_QUEEN;
    pieceMoves.insert(move);
    move.promoteTo = BLACK_ROOK;
    pieceMoves.insert(move);
    move.promoteTo = BLACK_BISHOP;
    pieceMoves.insert(move);
    move.promoteTo = BLACK_KNIGHT;
    pieceMoves.insert(move);
    move.promoteTo = NONE;
}

void Board::targetedByWhitePawn(uint64_t bit) {
    uint64_t aux1;
    aux1 = bit >> 9; //Capture right
    if (bit & ~H_FILE) {
        blackTargetedSquares |= aux1;
    }
    aux1 = bit >> 7; //Capture left
    if (bit & ~A_FILE) {
        blackTargetedSquares |= aux1;
    }
}

void Board::targetedByBlackPawn(uint64_t bit) {
    uint64_t aux1;
    aux1 = bit << 7; //Capture right
    if (bit & ~H_FILE) {
        whiteTargetedSquares |= aux1;
    }
    aux1 = bit << 9; //Capture left
    if (bit & ~A_FILE) {
        whiteTargetedSquares |= aux1;
    }
}

void Board::targetedByBishop(uint64_t bit) {
    std::pair<int, int> from = bitToij(bit);
    uint64_t *myPieces = (bit & whitePieces) ? &whitePieces : &blackPieces;
    uint64_t *opponentPieces = (bit & whitePieces) ? &blackPieces : &whitePieces;
    uint64_t *opponentTargetedeSquares = (moveTurn == WHITE) ? &blackTargetedSquares : &whiteTargetedSquares;
    uint64_t *opponentKing = (bit & whitePieces) ? &blackKing : &whiteKing;
    uint64_t *opponentPinned = (bit & whitePieces) ? &blackPinnedSquares : &whitePinnedSquares;
    uint64_t pinned;
    int piecesFound;
    
    const int I_MOVE[4] = {1, 1, -1, -1};
    const int J_MOVE[4] = {-1, 1, -1, 1};
    for (int i = 0; i < 4; ++i) {
        piecesFound = 0;
        int newI = from.first;
        int newJ = from.second;
        while (piecesFound < 2) {
            newI += I_MOVE[i];
            newJ += J_MOVE[i];
            if (newI < 0 || newI > 7 || newJ < 0 || newJ > 7) 
                break;

            uint64_t aux;
            ijToBit(newI, newJ, aux);
            if (piecesFound == 0) *opponentTargetedeSquares |= aux;

            if (aux & *myPieces) break;

            if (piecesFound == 0) {
                if (aux & *opponentPieces) {
                    pinned = aux;
                    ++piecesFound;
                }
            }
            else if (aux & *opponentKing)
                *opponentPinned |= pinned;
        }
    }
}

void Board::targetedByKnight(uint64_t bit) {
    uint64_t aux;
    uint64_t *opponentTargetedeSquares = (bit & whitePieces) ? &blackTargetedSquares : &whiteTargetedSquares;
    const int I_MOVE[8] = {2, 1, -1, -2, -2, -1, 1, 2};
    const int J_MOVE[8] = {1, 2, 2, 1, -1, -2, -2, -1};
    for (int i = 0; i < 8; ++i) {
        int newI = bitToij(bit).first + I_MOVE[i];
        int newJ = bitToij(bit).second + J_MOVE[i];
        if (newI >= 0 && newI < 8 && newJ >= 0 && newJ < 8) {
            ijToBit(newI, newJ, aux);
            *opponentTargetedeSquares |= aux;
        }
    }
}

void Board::targetedByRook(uint64_t bit) {
    std::pair<int, int> from = bitToij(bit);
    uint64_t *myPieces = (bit & whitePieces) ? &whitePieces : &blackPieces;
    uint64_t *opponentPieces = (bit & whitePieces) ? &blackPieces : &whitePieces;
    uint64_t *opponentTargetedeSquares = (bit & whitePieces) ? &blackTargetedSquares : &whiteTargetedSquares;
    uint64_t *opponentKing = (bit & whitePieces) ? &blackKing : &whiteKing;
    uint64_t *opponentPinned = (bit & whitePieces) ? &blackPinnedSquares : &whitePinnedSquares;
    uint64_t pinned;
    int piecesFound;

    const int I_MOVE[4] = {1, 0, -1, 0};
    const int J_MOVE[4] = {0, 1, 0, -1};
    for (int i = 0; i < 4; ++i) {
        piecesFound = 0;
        int newI = from.first;
        int newJ = from.second;
        while (piecesFound < 2) {
            newI += I_MOVE[i];
            newJ += J_MOVE[i];
            uint64_t aux;

            if (newI < 0 || newI > 7 || newJ < 0 || newJ > 7) 
                break;
            
            ijToBit(newI, newJ, aux);
            if (piecesFound == 0) *opponentTargetedeSquares |= aux;

            if (aux & *myPieces) break;
            
            if (piecesFound == 0) {
                if (aux & *opponentPieces) {
                    pinned = aux;
                    ++piecesFound;
                }
            }
            else if (aux & *opponentKing)
                *opponentPinned |= pinned;
        }
    }
}

void Board::targetedByQueen(uint64_t bit) {
    targetedByRook(bit);
    targetedByBishop(bit);
}

void Board::targetedByKing(uint64_t bit) {
    uint64_t aux;
    uint64_t *opponentTargetedeSquares = (bit & whitePieces) ? &blackTargetedSquares : &whiteTargetedSquares;
    const int I_MOVE[8] = {1, 1, 1, 0, -1, -1, -1, 0};
    const int J_MOVE[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
    for (int i = 0; i < 8; ++i) {
        int newI = bitToij(bit).first + I_MOVE[i];
        int newJ = bitToij(bit).second + J_MOVE[i];
        if (newI >= 0 and newI < 8 and newJ >= 0 and newJ < 8) {
            ijToBit(newI, newJ, aux);
            *opponentTargetedeSquares |= aux;
        }
    }
}