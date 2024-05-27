#include "myApp.hh"

MyApp::MyApp() {
    pieceMoveAvailable = false;
    pressed = false;

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

bool MyApp::handleEvents() {
    while (SDL_PollEvent(&e) != 0) {
        //Handle QUIT
        if (e.type == SDL_QUIT) {
            return false; // SOMETHING ELSE HERE
        }
        //Handle mouse click
        else if (e.type == SDL_MOUSEBUTTONDOWN) {
            pressed = true;
            lastMouseMove.from = MousePos(e.button.x, e.button.y);
            SDL_GetMouseState(&lastMouseMove.from.x, &lastMouseMove.from.y);
            //std::cout << "Mouse button pressed at (" << lastMouseMove.from.x << ", " << lastMouseMove.from.y << ")\n";
        }
        //Handle mouse release
        else if (e.type == SDL_MOUSEBUTTONUP && pressed) {
            pressed = false;
            lastMouseMove.to = MousePos(e.button.x, e.button.y);
            SDL_GetMouseState(&lastMouseMove.to.x, &lastMouseMove.to.y);
            //std::cout << "Mouse button released at (" << lastMouseMove.to.x << ", " << lastMouseMove.to.y << ")\n";
            MousePosMoveToPieceMove(lastMouseMove);
            //INFO: No need to check if the move is valid, since the user can only move the pieces inside the board
            pieceMoveAvailable = true;
        }
        else if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                resizeWindow(e.window.data1, e.window.data2);
            }
        }
    }
    return true;
}

void MyApp::render(PieceMatrix& pm) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    SDL_RenderClear(renderer);

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
            //std::cout << pieceToString(pm[i][j]) << std::endl;
            switch (pm[i][j]) {
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

    SDL_RenderPresent(renderer);
}

void MyApp::free() {
    if (mWhitePawnTexture != nullptr) {
        SDL_DestroyTexture(mWhitePawnTexture);
        mWhitePawnTexture = nullptr;
    }

    if (mBlackPawnTexture != nullptr) {
        SDL_DestroyTexture(mBlackPawnTexture);
        mBlackPawnTexture = nullptr;
    }
    //FOR ALL PIECES

    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;
    SDL_Quit();
}

void MyApp::MousePosMoveToPieceMove(MouseMove& move) {
    //Convert MousePos to PieceMove
    lastPieceMove.from = PiecePos((move.from.y - A8_y)/TILE_SIZE, (move.from.x - A8_x)/TILE_SIZE);
    lastPieceMove.to = PiecePos((move.to.y - A8_y)/TILE_SIZE, (move.to.x - A8_x)/TILE_SIZE);
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