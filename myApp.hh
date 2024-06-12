#ifndef MYAPP_HH
#define MYAPP_HH

#include </usr/include/SDL2/SDL.h>
#include </usr/include/SDL2/SDL_image.h>

#include "utils.hh"


class MyApp{
    public:
        MyApp() {}
        virtual ~MyApp() {}

        PieceMove getMove();

        virtual bool init() = 0;
        virtual bool isPieceMoveAvailable() = 0;
        virtual void setMoveTurn([[maybe_unused]] PieceColor color) = 0;
        virtual bool handleEvents() = 0;
        virtual void printBoard([[maybe_unused]] PieceMatrix& pm) = 0;
    
    protected:
        PieceMove lastPieceMove;
        bool pieceMoveAvailable;
        PieceColor moveTurn;
};

class ConsoleApp : public MyApp {
    public:
};

class GUIApp : public MyApp {
    public:
        GUIApp();
        ~GUIApp() override;

        bool init() override;
        bool isPieceMoveAvailable() override;
        void setMoveTurn(PieceColor color) override;
        bool handleEvents() override;
        void printBoard(PieceMatrix& pm) override;
        
    private:
        bool pressed;
        MouseMove lastMouseMove;
        MousePos promotionClickPos;
        
        bool promotionPending;
        PieceColor promotionColor;
        PieceType mousePosToPromotionOption(MousePos& pos);

        PieceMatrix pieceMatrix;
        void mouseMoveToPieceMove(MouseMove& move);        

        int TILE_SIZE = 80;
        int SCREEN_WIDTH = 640;
        int SCREEN_HEIGHT = 640;
        int A8_x = 0; //The coordinates of the A8 square, in pixels
        int A8_y = 0; 
        void resizeWindow(int width, int height);

        SDL_Window* window;
        SDL_Renderer* renderer;

        SDL_Event e;
        

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

        void renderBoard();
        void displayPromotionOptions(PieceColor color);
        bool loadMedia();
        SDL_Texture* loadTexture(const std::string &path);
};

#endif