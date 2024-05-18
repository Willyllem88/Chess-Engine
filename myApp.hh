#ifndef MYAPP_HH
#define MYAPP_HH

#include </usr/include/SDL2/SDL.h>
#include </usr/include/SDL2/SDL_image.h>

#include "utils.hh"

class MyApp{
    public:
        MyApp();
        ~MyApp();

        bool init();

        //Asks if the user has made a move
        bool isPieceMoveAvailable();

        //Gets the move made by the user
        //Pre: pieceMoveAvailable() == true
        PieceMove getMove();

        bool handleEvents();

        //Renders the board and the pieces
        void render(PieceMatrix& pm);


        //Frees the textures
        void free();
        
    private:
        //used to detect mouse slides.
        bool pressed;
        bool pieceMoveAvailable;
        MouseMove lastMouseMove;
        PieceMove lastPieceMove;
        void MousePosMoveToPieceMove(MouseMove& move);        

        //__WINDOW__
        const int TILE_SIZE = 80;
        const int SCREEN_WIDTH = 640;
        const int SCREEN_HEIGHT = 640;

        SDL_Window* window;
        SDL_Renderer* renderer;

        SDL_Event e;
        

        //__MODELS_: textures for the pieces
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

        bool loadMedia();
        SDL_Texture* loadTexture(const std::string &path);
};

#endif