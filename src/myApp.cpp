#include "myApp.hh"
#include "board.hh"

MyApp::MyApp() {
    pieceMoveAvailable = false;
    pieceMatrix = std::vector(8, std::vector(8, NONE));
    prevMoveTurn = BLACK;
}

PieceMove MyApp::getMove() {
    pieceMoveAvailable = false;
    return lastPieceMove;
}

void MyApp::setBoard(std::shared_ptr<Board> b) {
    this->board = b;
}

ConsoleApp::ConsoleApp() {
    pieceMoveAvailable = false;
    pieceMatrix = std::vector(8, std::vector(8, NONE));
}

ConsoleApp::~ConsoleApp() {}

bool ConsoleApp::init() {
    return true;
}

bool ConsoleApp::isPieceMoveAvailable() {
    return pieceMoveAvailable;
}

MyApp::eventType ConsoleApp::handleEvents() {
    std::string str;
    //If the user has inputed text from the console, handle it
    if (readStringFromConsole(str)) {
        std::set<PieceMove> legalMoves = board->getCurrentLegalMoves();
        lastPieceMove = algebraicToPieceMove(str, legalMoves, pieceMatrix, board->getMoveTurn());
        if (str == "undo" || str == "u") return UNDO;
        else pieceMoveAvailable = true;
    }
    return NO_EVENT;
}

void ConsoleApp::printBoard(const PieceMatrix& pm) {
    //If the turn has changed, print the board
    if (board->getMoveTurn() == prevMoveTurn) return;
    prevMoveTurn = board->getMoveTurn();

    pieceMatrix = pm;
    char pieceChar[] = {'P', 'B', 'N', 'R', 'Q', 'K', 'p', 'b', 'n', 'r', 'q', 'k', ' '};
    for (int i = 0; i < 8; ++i) {
        std::cout << "  +---+---+---+---+---+---+---+---+\n";
        std:: cout << 8-i << std::flush;
        for (int j = 0; j < 8; ++j)
            std::cout << "| " << pieceChar[pm[i][j]] << " " << std::flush;
        std::cout << "|\n";
    }
    std::cout << "  +---+---+---+---+---+---+---+---+\n";
    std::cout << "    a   b   c   d   e   f   g   h  \n\n";
}

GUIApp::GUIApp() {
    piecesCount = 32;

    pieceMoveAvailable = false;
    promotionPending = false;
    pressed = false;

    draggedPiece = NONE;
    targetedPieces.clear();
    targetedSquares.clear();

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

    mTargetedSquareTexture = nullptr;
    mTargetedPieceTexture = nullptr;
}

GUIApp::~GUIApp() {
    SDL_DestroyTexture(mWhitePawnTexture);
    SDL_DestroyTexture(mWhiteBishopTexture);
    SDL_DestroyTexture(mWhiteKnightTexture);
    SDL_DestroyTexture(mWhiteRookTexture);
    SDL_DestroyTexture(mWhiteQueenTexture);
    SDL_DestroyTexture(mWhiteKingTexture);
    SDL_DestroyTexture(mBlackPawnTexture);
    SDL_DestroyTexture(mBlackBishopTexture);
    SDL_DestroyTexture(mBlackKnightTexture);
    SDL_DestroyTexture(mBlackRookTexture);
    SDL_DestroyTexture(mBlackQueenTexture);
    SDL_DestroyTexture(mBlackKingTexture);
    SDL_DestroyTexture(mTargetedSquareTexture);
    SDL_DestroyTexture(mTargetedPieceTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_FreeChunk(mMoveSound);
    Mix_FreeChunk(mCaptureSound);
    Mix_FreeChunk(mCheckmateSound);
    Mix_CloseAudio();
    
    SDL_Quit();
}

bool GUIApp::init() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create window
    window = SDL_CreateWindow("Chess Board", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer no pudo inicializarse: %s\n", Mix_GetError());
        return 1;
    }
    
    // Load media
    if (!loadMedia()) {
        printf("Failed to load media!\n");
        return false;
    }
    
    return true;
}

bool GUIApp::isPieceMoveAvailable() {
    return pieceMoveAvailable;
}

MyApp::eventType GUIApp::handleEvents() {
    //If there has been an SDL event
    while (SDL_PollEvent(&e)) {
        //Handle QUIT
        if (e.type == SDL_QUIT) {
            return QUIT;
        }
        //Handle mouse click
        else if (e.type == SDL_MOUSEBUTTONDOWN) {
            lastPieceMove.promoteTo = NONE;
            pressed = true;
            SDL_GetMouseState(&lastMouseMove.from.x, &lastMouseMove.from.y);
            SDL_GetMouseState(&lastMouseMove.to.x, &lastMouseMove.to.y);

            if (promotionPending)
                lastPieceMove.promoteTo = mousePosToPromotionOption(lastMouseMove.from);
            else {
                //Get the piece that is being dragged and the possible moves, only when we are not displaying the promotion options
                int i, j;
                i = (lastMouseMove.from.y - A8_y)/TILE_SIZE;
                j = (lastMouseMove.from.x - A8_x)/TILE_SIZE;
                draggedPiece = board->getPieceType(i, j);
                draggedPiecePos = PiecePos(i, j);
                getTargets();
            }
        }
        //Handle mouse release
        else if (e.type == SDL_MOUSEBUTTONUP && pressed) {
            draggedPiece = NONE;
            targetedPieces.clear();
            targetedSquares.clear();
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
        //Handle mouse move
        else if (e.type == SDL_MOUSEMOTION && pressed) {
            SDL_GetMouseState(&lastMouseMove.to.x, &lastMouseMove.to.y);
        }
        else if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                resizeWindow(e.window.data1, e.window.data2);
        }
        //Handles left arrow pressed
        else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LEFT) {
            return UNDO;
        }
    }

    std::string str;
    if (readStringFromConsole(str)) {
        std::set<PieceMove> legalMoves = board->getCurrentLegalMoves();
        lastPieceMove = algebraicToPieceMove(str, legalMoves, pieceMatrix, board->getMoveTurn());
        if (str == "undo" || str == "u") return UNDO;
        else pieceMoveAvailable = true;
    }
    
    return NO_EVENT;
}

PieceType GUIApp::mousePosToPromotionOption(MousePos& pos) {
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

void GUIApp::renderBoard() {
    SDL_Rect fillRect;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            fillRect = { j * TILE_SIZE + A8_x, i * TILE_SIZE + A8_y, TILE_SIZE, TILE_SIZE };
            
            //Print board
            if ((i + j) % 2 == 0)
                SDL_SetRenderDrawColor(renderer, WHITE_SQUARES.r, WHITE_SQUARES.g, WHITE_SQUARES.b, WHITE_SQUARES.a);  // White
            else
                SDL_SetRenderDrawColor(renderer, BLACK_SQUARES.r, BLACK_SQUARES.g, BLACK_SQUARES.b, BLACK_SQUARES.a);  // Black
            SDL_RenderFillRect(renderer, &fillRect);

            //Print targeted squares
            if (targetedSquares.find(PiecePos(i, j)) != targetedSquares.end())
                SDL_RenderCopy(renderer, mTargetedSquareTexture, nullptr, &fillRect);
            else if (targetedPieces.find(PiecePos(i, j)) != targetedPieces.end())
                SDL_RenderCopy(renderer, mTargetedPieceTexture, nullptr, &fillRect);
            
            if (draggedPiecePos == PiecePos(i, j) && draggedPiece != NONE) continue;

            //Print pieces
            SDL_Texture* pieceTexture = getPieceTexture(pieceMatrix[i][j]);
            SDL_RenderCopy(renderer, pieceTexture, nullptr, &fillRect);
        }
    }

    //If a piece is being dragged and the player is not promoting, render the piece being dragged
    if (draggedPiece != NONE) {
        fillRect = { lastMouseMove.to.x - TILE_SIZE/2, lastMouseMove.to.y - TILE_SIZE/2, TILE_SIZE, TILE_SIZE };
        SDL_Texture* pieceTexture = getPieceTexture(draggedPiece);
        SDL_RenderCopy(renderer, pieceTexture, nullptr, &fillRect);  
    }
}

void GUIApp::printBoard(const PieceMatrix& pm) {
    //If the turn has changed, play the sound (i don't really like this, i think the sound should be addressed in a different way)
    if (prevMoveTurn != board->getMoveTurn()) {
        if (board->getBoardResult() != PLAYING)
            Mix_PlayChannel(-1, mCheckmateSound, 0);
        else if (board->getAllPiecesCount() < piecesCount)
            Mix_PlayChannel(-1, mCaptureSound, 0);
        else
            Mix_PlayChannel(-1, mMoveSound, 0);
    }
    prevMoveTurn = board->getMoveTurn();
    piecesCount = board->getAllPiecesCount();

    pieceMatrix = pm;

    SDL_SetRenderDrawColor(renderer, BACKGROUND.r, BACKGROUND.g, BACKGROUND.b, BACKGROUND.a);
    SDL_RenderClear(renderer);

    renderBoard();
    if (promotionPending)
        displayPromotionOptions(promotionColor);

    SDL_RenderPresent(renderer);
}

void GUIApp::getTargets() {
    targetedSquares.clear();
    targetedPieces.clear();
    
    std::set<PieceMove> legalMoves = board->getCurrentLegalMoves();
    for (PieceMove move : legalMoves) {
        if (move.from == draggedPiecePos) {
            if (board->getPieceType(move.to.i, move.to.j) == NONE)
                targetedSquares.insert(move.to);
            else
                targetedPieces.insert(move.to);
        }
    }
}

void GUIApp::mouseMoveToPieceMove(MouseMove& move) {
    //Convert MousePos to PieceMove
    lastPieceMove.from = PiecePos((move.from.y - A8_y)/TILE_SIZE, (move.from.x - A8_x)/TILE_SIZE);
    lastPieceMove.to = PiecePos((move.to.y - A8_y)/TILE_SIZE, (move.to.x - A8_x)/TILE_SIZE);
    //if the move is a promotion, the user will have to click on the piece to promote to
    //FIX: has to detect which turn it is. It now shows the promotion options for both players even though it's not their turn
    if (board->getMoveTurn() == WHITE && pieceMatrix[lastPieceMove.from.i][lastPieceMove.from.j] == WHITE_PAWN && lastPieceMove.from.i == 1 && lastPieceMove.to.i == 0) {
        promotionColor = WHITE;
        promotionPending = true;
    }
    else if (board->getMoveTurn() == BLACK && pieceMatrix[lastPieceMove.from.i][lastPieceMove.from.j] == BLACK_PAWN && lastPieceMove.from.i == 6 && lastPieceMove.to.i == 7) {
        promotionColor = BLACK;
        promotionPending = true;
    }
    else
        promotionPending = false;

}

void GUIApp::displayPromotionOptions(PieceColor color) {
    //Display a 3x3 square for the background of the promotion options
    SDL_Rect fillRect = { 5*TILE_SIZE/2 + A8_x, 5*TILE_SIZE/2 + A8_y, 3*TILE_SIZE, 3*TILE_SIZE };
    SDL_SetRenderDrawColor(renderer, PROMOTE_BACKGROUND.r, PROMOTE_BACKGROUND.g, PROMOTE_BACKGROUND.b, PROMOTE_BACKGROUND.a);
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

void GUIApp::resizeWindow(int newWidth, int newHeight) {
    this->SCREEN_WIDTH = newWidth;
    this->SCREEN_HEIGHT = newHeight;
    //The board will be centered in the window
    TILE_SIZE = std::min(SCREEN_WIDTH, SCREEN_HEIGHT) / 8;
    A8_x = (SCREEN_WIDTH - 8 * TILE_SIZE) / 2;
    A8_y = (SCREEN_HEIGHT - 8 * TILE_SIZE) / 2;
}

bool GUIApp::loadMedia() {
    // Load textures
    mWhitePawnTexture = loadTexture("assets/img/whitePawn.png");
    if (mWhitePawnTexture == nullptr) return false;
    mWhiteBishopTexture = loadTexture("assets/img/whiteBishop.png");
    if (mWhiteBishopTexture == nullptr) return false;
    mWhiteKnightTexture = loadTexture("assets/img/whiteKnight.png");
    if (mWhiteKnightTexture == nullptr) return false;
    mWhiteRookTexture = loadTexture("assets/img/whiteRook.png");
    if (mWhiteRookTexture == nullptr) return false;
    mWhiteQueenTexture = loadTexture("assets/img/whiteQueen.png");
    if (mWhiteQueenTexture == nullptr) return false;
    mWhiteKingTexture = loadTexture("assets/img/whiteKing.png");
    if (mWhiteKingTexture == nullptr) return false;
    mBlackPawnTexture = loadTexture("assets/img/blackPawn.png");
    if (mBlackPawnTexture == nullptr) return false;
    mBlackBishopTexture = loadTexture("assets/img/blackBishop.png");
    if (mBlackBishopTexture == nullptr) return false;
    mBlackKnightTexture = loadTexture("assets/img/blackKnight.png");
    if (mBlackKnightTexture == nullptr) return false;
    mBlackRookTexture = loadTexture("assets/img/blackRook.png");
    if (mBlackRookTexture == nullptr) return false;
    mBlackQueenTexture = loadTexture("assets/img/blackQueen.png");
    if (mBlackQueenTexture == nullptr) return false;
    mBlackKingTexture = loadTexture("assets/img/blackKing.png");
    if (mBlackKingTexture == nullptr) return false;
    mTargetedSquareTexture = loadTexture("assets/img/targetedSquare.png");
    if (mTargetedSquareTexture == nullptr) return false;
    mTargetedPieceTexture = loadTexture("assets/img/targetedPiece.png");
    if (mTargetedPieceTexture == nullptr) return false;

    // Load sounds
    mMoveSound = loadSound("assets/sounds/move.wav");
    if (mMoveSound == nullptr) return false;
    mCaptureSound = loadSound("assets/sounds/capture.wav");
    if (mCaptureSound == nullptr) return false;
    mCheckmateSound = loadSound("assets/sounds/checkmate.wav");
    if (mCheckmateSound == nullptr) return false;

    return true;
}

SDL_Texture* GUIApp::loadTexture(const std::string &path) {
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

Mix_Chunk* GUIApp::loadSound(const std::string &path) {
    Mix_Chunk* newSound = Mix_LoadWAV(path.c_str());
    if (newSound == nullptr) {
        printf("Unable to load sound %s! SDL_mixer Error: %s\n", path.c_str(), Mix_GetError());
    }
    return newSound;
}

SDL_Texture* GUIApp::getPieceTexture(PieceType p) {
    switch (p) {
        case WHITE_PAWN:
            return mWhitePawnTexture;
        case WHITE_BISHOP:
            return mWhiteBishopTexture;
        case WHITE_KNIGHT:
            return mWhiteKnightTexture;
        case WHITE_ROOK:
            return mWhiteRookTexture;
        case WHITE_QUEEN:
            return mWhiteQueenTexture;
        case WHITE_KING:
            return mWhiteKingTexture;
        case BLACK_PAWN:
            return mBlackPawnTexture;
        case BLACK_BISHOP:
            return mBlackBishopTexture;
        case BLACK_KNIGHT:
            return mBlackKnightTexture;
        case BLACK_ROOK:
            return mBlackRookTexture;
        case BLACK_QUEEN:
            return mBlackQueenTexture;
        case BLACK_KING:
            return mBlackKingTexture;
        default:
            return nullptr;
    }
}