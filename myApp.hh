#ifndef MYAPP_HH
#define MYAPP_HH

#include </usr/include/SDL2/SDL.h>
#include </usr/include/SDL2/SDL_image.h>

#include "structs.hh"

class MyApp{
    public:
        MyApp();
        ~MyApp();

        bool init();
        
        //Loads the initial textures for the pieces, it'll return false if there has been any error
        bool loadMedia();

        bool handleEvents();

        //Renders the board and the pieces
        void render(PieceMatrix& pm);

        //Frees the textures
        void free();
        
    private:
        //Window
        const int TILE_SIZE = 80;
        const int SCREEN_WIDTH = 640;
        const int SCREEN_HEIGHT = 640;

        SDL_Window* window;
        SDL_Renderer* renderer;

        SDL_Event e;
        

        //Models
        SDL_Texture* mWhitePawnTexture;
        SDL_Texture* mWhiteBishopTexture;
        SDL_Texture* mWhiteKnightTexture;
        SDL_Texture* mWhiteRookTexture;
        SDL_Texture* mWhiteQueenTexture;
        SDL_Texture* mWhiteKingTexture;

        SDL_Texture* mBlackPawnTexture;
        SDL_Texture* mBlackBishopTexture;
        SDL_Texture* mBlackKnightTexture;
        SDL_Texture* mBlackRookTexture;
        SDL_Texture* mBlackQueenTexture;
        SDL_Texture* mBlackKingTexture;

        SDL_Texture* loadTexture(const std::string &path);
};

#endif