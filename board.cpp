#include "Board.hh"

const int TILE_SIZE = 80;

Board::Board(SDL_Renderer* renderer)
    : mRenderer(renderer), mWhitePawnTexture(nullptr), mBlackPawnTexture(nullptr) {
}

Board::~Board() {
    free();
}

bool Board::loadMedia() {
    mWhitePawnTexture = loadTexture("./img/whitePawn.png");
    if (mWhitePawnTexture == nullptr) {
        return false;
    }

    mBlackPawnTexture = loadTexture("./img/blackPawn.png");
    if (mBlackPawnTexture == nullptr) {
        return false;
    }

    return true;
}

void Board::render() {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            SDL_Rect fillRect = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            if ((i + j) % 2 == 0) {
                SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);  // White
            } else {
                SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);  // Black
            }
            SDL_RenderFillRect(mRenderer, &fillRect);

            if (i == 1) {
                SDL_RenderCopy(mRenderer, mBlackPawnTexture, nullptr, &fillRect);
            } else if (i == 6) {
                SDL_RenderCopy(mRenderer, mWhitePawnTexture, nullptr, &fillRect);
            }
        }
    }
}

void Board::free() {
    if (mWhitePawnTexture != nullptr) {
        SDL_DestroyTexture(mWhitePawnTexture);
        mWhitePawnTexture = nullptr;
    }

    if (mBlackPawnTexture != nullptr) {
        SDL_DestroyTexture(mBlackPawnTexture);
        mBlackPawnTexture = nullptr;
    }
}

SDL_Texture* Board::loadTexture(const std::string &path) {
    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    } else {
        newTexture = SDL_CreateTextureFromSurface(mRenderer, loadedSurface);
        if (newTexture == nullptr) {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}