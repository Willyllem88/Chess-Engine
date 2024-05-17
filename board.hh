#ifndef BOARD_H
#define BOARD_H

#include </usr/include/SDL2/SDL.h>
#include </usr/include/SDL2/SDL_image.h>
#include <string>

class Board {
public:
    Board(SDL_Renderer* renderer);
    ~Board();
    bool loadMedia();
    void render();
    void free();

private:
    SDL_Renderer* mRenderer;
    SDL_Texture* mWhitePawnTexture;
    SDL_Texture* mBlackPawnTexture;
    SDL_Texture* loadTexture(const std::string &path);
};

#endif