#include "myApp.hh"

MyApp::MyApp() {
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

bool MyApp::handleEvents() {
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            return false; // SOMETHING ELSE HERE
        }
    }
    return true;
}

void MyApp::render(PieceMatrix& pm) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            SDL_Rect fillRect = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
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