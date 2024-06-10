#include "myApp.hh"

MyApp::MyApp() {
    pieceMoveAvailable = false;
    promotionPending = false;
    pressed = false;
    pieceMatrix = std::vector(8, std::vector(8, NONE));

    mWhitePawnTexture = nullptr; 
    mWhiteBishopTexture = nullptr;
    mWhiteKnightTexture = nullptr;
    mWhiteRookTexture = nullptr;
    mWhiteQueenTexture = nullptr;
    mWhiteKingTexture = nullptr;

    mBlackPawnTexture = nullptr;
    mBlackBishopTexture = nullptr;
    mBlackKnightTexture = nullptr;
    mBlackRookTexture = nullptr;
    mBlackQueenTexture = nullptr;
    mBlackKingTexture = nullptr;    
}

MyApp::~MyApp() {
    free();
}

bool MyApp::init() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create window
    window = SDL_CreateWindow("Chess Board", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    if (!loadMedia()) {
        printf("Failed to load media!\n");
        return false;
    }

    return true;
}

bool MyApp::isPieceMoveAvailable() {
    return pieceMoveAvailable;
}

PieceMove MyApp::getMove() {
    pieceMoveAvailable = false;
    return lastPieceMove;
}

void MyApp::setMoveTurn(PieceColor color) {
    moveTurn = color;
}

bool MyApp::handleEvents() {
    while (SDL_PollEvent(&e) != 0) {
        //Handle QUIT
        if (e.type == SDL_QUIT) {
            return false; // SOMETHING ELSE HERE
        }
        //Handle mouse click
        else if (e.type == SDL_MOUSEBUTTONDOWN) {
            lastPieceMove.promoteTo = NONE;
            pressed = true;
            SDL_GetMouseState(&lastMouseMove.from.x, &lastMouseMove.from.y);
            if (promotionPending)
                lastPieceMove.promoteTo = mousePosToPromotionOption(lastMouseMove.from);
        }
        //Handle mouse release
        else if (e.type == SDL_MOUSEBUTTONUP && pressed) {
            pressed = false;
            if (!promotionPending) {
                SDL_GetMouseState(&lastMouseMove.to.x, &lastMouseMove.to.y);
                mouseMoveToPieceMove(lastMouseMove);
                //mouseMoveToPieceMove will set pieceMoveAvailable to true if the move is a promotion
                if (!promotionPending)
                    pieceMoveAvailable = true;
            }
            else if (lastPieceMove.promoteTo != NONE){
                promotionPending = false;
                pieceMoveAvailable = true;
            }
        }
        else if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                resizeWindow(e.window.data1, e.window.data2);
            }
        }
    }
    return true;
}

PieceType MyApp::mousePosToPromotionOption(MousePos& pos) {
    if (pos.x > A8_x + 3 * TILE_SIZE && pos.x < A8_x + 4 * TILE_SIZE) {
        if (pos.y > A8_y + 3 * TILE_SIZE && pos.y < A8_y + 4 * TILE_SIZE)
            return (promotionColor == WHITE) ? WHITE_QUEEN : BLACK_QUEEN;
        else if (pos.y > A8_y + 4 * TILE_SIZE && pos.y < A8_y + 5 * TILE_SIZE)
            return (promotionColor == WHITE) ? WHITE_ROOK : BLACK_ROOK;
    } 
    else if (pos.x > A8_x + 4 * TILE_SIZE && pos.x < A8_x + 5 * TILE_SIZE) {
        if (pos.y > A8_y + 3 * TILE_SIZE && pos.y < A8_y + 4 * TILE_SIZE)
            return (promotionColor == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT;
        else if (pos.y > A8_y + 4 * TILE_SIZE && pos.y < A8_y + 5 * TILE_SIZE)
            return (promotionColor == WHITE) ? WHITE_BISHOP : BLACK_BISHOP;
    }
    return NONE;
}

void MyApp::renderBoard() {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            SDL_Rect fillRect = { j * TILE_SIZE + A8_x, i * TILE_SIZE + A8_y, TILE_SIZE, TILE_SIZE };
            //Print board
            if ((i + j) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White
            } else {
                SDL_SetRenderDrawColor(renderer, 100, 100, 100 , 255);  // Black
            }
            SDL_RenderFillRect(renderer, &fillRect);
            
            //Print pieces
            switch (pieceMatrix[i][j]) {
                case WHITE_PAWN:
                    SDL_RenderCopy(renderer, mWhitePawnTexture, nullptr, &fillRect);
                    break;
                case WHITE_BISHOP:
                    SDL_RenderCopy(renderer, mWhiteBishopTexture, nullptr, &fillRect);
                    break;
                case WHITE_KNIGHT:
                    SDL_RenderCopy(renderer, mWhiteKnightTexture, nullptr, &fillRect);
                    break;
                case WHITE_ROOK:
                    SDL_RenderCopy(renderer, mWhiteRookTexture, nullptr, &fillRect);
                    break;
                case WHITE_QUEEN:
                    SDL_RenderCopy(renderer, mWhiteQueenTexture, nullptr, &fillRect);
                    break;
                case WHITE_KING:
                    SDL_RenderCopy(renderer, mWhiteKingTexture, nullptr, &fillRect);
                    break;
                case BLACK_PAWN:
                    SDL_RenderCopy(renderer, mBlackPawnTexture, nullptr, &fillRect);
                    break;
                case BLACK_BISHOP:
                    SDL_RenderCopy(renderer, mBlackBishopTexture, nullptr, &fillRect);
                    break;
                case BLACK_KNIGHT:
                    SDL_RenderCopy(renderer, mBlackKnightTexture, nullptr, &fillRect);
                    break;
                case BLACK_ROOK:
                    SDL_RenderCopy(renderer, mBlackRookTexture, nullptr, &fillRect);
                    break;
                case BLACK_QUEEN:
                    SDL_RenderCopy(renderer, mBlackQueenTexture, nullptr, &fillRect);
                    break;
                case BLACK_KING:
                    SDL_RenderCopy(renderer, mBlackKingTexture, nullptr, &fillRect);
                    break;
                case NONE:
                    break;
            }
        }
    }
}

void MyApp::printBoard(PieceMatrix& pm) {
    pieceMatrix = pm;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    renderBoard();
    if (promotionPending)
        displayPromotionOptions(promotionColor);

    SDL_RenderPresent(renderer);
}

void MyApp::mouseMoveToPieceMove(MouseMove& move) {
    //Convert MousePos to PieceMove
    lastPieceMove.from = PiecePos((move.from.y - A8_y)/TILE_SIZE, (move.from.x - A8_x)/TILE_SIZE);
    lastPieceMove.to = PiecePos((move.to.y - A8_y)/TILE_SIZE, (move.to.x - A8_x)/TILE_SIZE);
    //if the move is a promotion, the user will have to click on the piece to promote to
    //FIX: has to detect which turn it is. It now shows the promotion options for both players even though it's not their turn
    if (moveTurn == WHITE && pieceMatrix[lastPieceMove.from.i][lastPieceMove.from.j] == WHITE_PAWN && lastPieceMove.from.i == 1 && lastPieceMove.to.i == 0) {
        promotionColor = WHITE;
        promotionPending = true;
    }
    else if (moveTurn == BLACK && pieceMatrix[lastPieceMove.from.i][lastPieceMove.from.j] == BLACK_PAWN && lastPieceMove.from.i == 6 && lastPieceMove.to.i == 7) {
        promotionColor = BLACK;
        promotionPending = true;
    }
    else
        promotionPending = false;

}

void MyApp::displayPromotionOptions(PieceColor color) {
    //Display a 3x3 square for the background of the promotion options
    SDL_Rect fillRect = { 5*TILE_SIZE/2 + A8_x, 5*TILE_SIZE/2 + A8_y, 3*TILE_SIZE, 3*TILE_SIZE };
    SDL_SetRenderDrawColor(renderer, 175, 175, 175, 255);
    SDL_RenderFillRect(renderer, &fillRect);

    //Pints each piece in the promotion options
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            fillRect = { j * TILE_SIZE + A8_x + 3 * TILE_SIZE, i * TILE_SIZE + A8_y + 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            if (color == WHITE) {
                switch (i * 2 + j) {
                    case 0:
                        SDL_RenderCopy(renderer, mWhiteQueenTexture, nullptr, &fillRect);
                        break;
                    case 1:
                        SDL_RenderCopy(renderer, mWhiteKnightTexture, nullptr, &fillRect);
                        break;
                    case 2:
                        SDL_RenderCopy(renderer, mWhiteRookTexture, nullptr, &fillRect);
                        break;
                    case 3:
                        SDL_RenderCopy(renderer, mWhiteBishopTexture, nullptr, &fillRect);
                        break;
                }
            }
            else {
                switch (i * 2 + j) {
                    case 0:
                        SDL_RenderCopy(renderer, mBlackQueenTexture, nullptr, &fillRect);
                        break;
                    case 1:
                        SDL_RenderCopy(renderer, mBlackKnightTexture, nullptr, &fillRect);
                        break;
                    case 2:
                        SDL_RenderCopy(renderer, mBlackRookTexture, nullptr, &fillRect);
                        break;
                    case 3:
                        SDL_RenderCopy(renderer, mBlackBishopTexture, nullptr, &fillRect);
                        break;
                }
            }
        }
    }
}

void MyApp::resizeWindow(int newWidth, int newHeight) {
    this->SCREEN_WIDTH = newWidth;
    this->SCREEN_HEIGHT = newHeight;
    TILE_SIZE = std::min(SCREEN_WIDTH, SCREEN_HEIGHT) / 8;
    A8_x = (SCREEN_WIDTH - 8 * TILE_SIZE) / 2;
    A8_y = (SCREEN_HEIGHT - 8 * TILE_SIZE) / 2;
    // Adjust TILE_SIZE based on new dimensions, if necessary
}

bool MyApp::loadMedia() {
    mWhitePawnTexture = loadTexture("./img/whitePawn.png");
    if (mWhitePawnTexture == nullptr) return false;
    mWhiteBishopTexture = loadTexture("./img/whiteBishop.png");
    if (mWhiteBishopTexture == nullptr) return false;
    mWhiteKnightTexture = loadTexture("./img/whiteKnight.png");
    if (mWhiteKnightTexture == nullptr) return false;
    mWhiteRookTexture = loadTexture("./img/whiteRook.png");
    if (mWhiteRookTexture == nullptr) return false;
    mWhiteQueenTexture = loadTexture("./img/whiteQueen.png");
    if (mWhiteQueenTexture == nullptr) return false;
    mWhiteKingTexture = loadTexture("./img/whiteKing.png");
    if (mWhiteKingTexture == nullptr) return false;

    mBlackPawnTexture = loadTexture("./img/blackPawn.png");
    if (mBlackPawnTexture == nullptr) return false;
    mBlackBishopTexture = loadTexture("./img/blackBishop.png");
    if (mBlackBishopTexture == nullptr) return false;
    mBlackKnightTexture = loadTexture("./img/blackKnight.png");
    if (mBlackKnightTexture == nullptr) return false;
    mBlackRookTexture = loadTexture("./img/blackRook.png");
    if (mBlackRookTexture == nullptr) return false;
    mBlackQueenTexture = loadTexture("./img/blackQueen.png");
    if (mBlackQueenTexture == nullptr) return false;
    mBlackKingTexture = loadTexture("./img/blackKing.png");
    if (mBlackKingTexture == nullptr) return false;

    return true;
}

SDL_Texture* MyApp::loadTexture(const std::string &path) {
    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    } else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == nullptr) {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}

void MyApp::free() {
    if (mWhitePawnTexture != nullptr) {
        SDL_DestroyTexture(mWhitePawnTexture);
        mWhitePawnTexture = nullptr;}
    if (mBlackPawnTexture != nullptr) {
        SDL_DestroyTexture(mBlackPawnTexture);
        mBlackPawnTexture = nullptr;}
    if (mWhiteBishopTexture != nullptr) {
        SDL_DestroyTexture(mWhiteBishopTexture);
        mWhiteBishopTexture = nullptr;}
    if (mBlackBishopTexture != nullptr) {
        SDL_DestroyTexture(mBlackBishopTexture);
        mBlackBishopTexture = nullptr;}
    if (mWhiteKnightTexture != nullptr) {
        SDL_DestroyTexture(mWhiteKnightTexture);
        mWhiteKnightTexture = nullptr;}
    if (mBlackKnightTexture != nullptr) {
        SDL_DestroyTexture(mBlackKnightTexture);
        mBlackKnightTexture = nullptr;}
    if (mWhiteRookTexture != nullptr) {
        SDL_DestroyTexture(mWhiteRookTexture);
        mWhiteRookTexture = nullptr;}
    if (mBlackRookTexture != nullptr) {
        SDL_DestroyTexture(mBlackRookTexture);
        mBlackRookTexture = nullptr;}
    if (mWhiteQueenTexture != nullptr) {
        SDL_DestroyTexture(mWhiteQueenTexture);
        mWhiteQueenTexture = nullptr;}
    if (mBlackQueenTexture != nullptr) {
        SDL_DestroyTexture(mBlackQueenTexture);
        mBlackQueenTexture = nullptr;}
    if (mWhiteKingTexture != nullptr) {
        SDL_DestroyTexture(mWhiteKingTexture);
        mWhiteKingTexture = nullptr;}
    if (mBlackKingTexture != nullptr) {
        SDL_DestroyTexture(mBlackKingTexture);
        mBlackKingTexture = nullptr;}

    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;
    SDL_Quit();
}