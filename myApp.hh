#ifndef MYAPP_HH
#define MYAPP_HH

#include </usr/include/SDL2/SDL.h>
#include </usr/include/SDL2/SDL_image.h>

#include <memory>

#include "utils.hh"

class Board;

class MyApp{
    public:
        //  Creates a MyApp object
        MyApp();

        virtual ~MyApp() {}

        //  Returns the last piece move made by the player
        PieceMove getMove();

        //  Sets the board to the app
        void setBoard(std::shared_ptr<Board> b);

        //  Sets the move turn to the color passed as argument
        void setMoveTurn(PieceColor color);

        //  Initializes the app
        virtual bool init() = 0;

        //  Returns true if a piece move is available
        virtual bool isPieceMoveAvailable() = 0;

        //  Handles the events of the app
        virtual bool handleEvents() = 0;

        //  Prints the board
        virtual void printBoard([[maybe_unused]] PieceMatrix& pm) = 0;
    
    protected:
        std::shared_ptr<Board> board; //The board that the app will interact with

        PieceMatrix pieceMatrix; //The matrix that represents the board
        PieceMove lastPieceMove; //The last piece move made by the player
        bool pieceMoveAvailable; //True if a piece move is available, false otherwise
        PieceColor moveTurn; //The color of the player that will move next

        //  Structs for the mouse and the pieces moves.
        //      The mouse will be trackts with (x, y) coordinates
        //      The pieces with (i, j) coordinates
        struct MousePos {
            int x;
            int y;

            inline MousePos() : x(0), y(0) { };
            inline MousePos(int x, int y) : x(x), y(y) { };
        };

        struct MouseMove {
            MousePos from;
            MousePos to;

            inline MouseMove() : from(MousePos(0, 0)), to(MousePos(0, 0)) { };
        };
};

class ConsoleApp : public MyApp {
    public:
        ConsoleApp();
        ~ConsoleApp() override;

        bool init() override;
        bool isPieceMoveAvailable() override;
        bool handleEvents() override;
        void printBoard(PieceMatrix& pm) override;
    
    private:
        PieceColor prevMoveTurn; //The color of the player that moved last
};

class GUIApp : public MyApp {
    public:
        GUIApp();
        ~GUIApp() override;

        bool init() override;
        bool isPieceMoveAvailable() override;
        bool handleEvents() override;
        void printBoard(PieceMatrix& pm) override;
        
    private:
        //MOUSE DRAGGING INFORMATION

        bool pressed; //True if the mouse is currently pressed, false otherwise
        MouseMove lastMouseMove; //The last mouse move made by the player
        MousePos promotionClickPos; //The position of the click that will determine the promotion piece
        

        //PROMOTION INFORMATION

        bool promotionPending; //True if the user has to choose wich piece to promote to, false otherwise
        PieceColor promotionColor; //The color of the player that will promote
        

        //SDL INFORMATION

        //The dimensions and position of the board
        int TILE_SIZE = 80;
        int SCREEN_WIDTH = 640;
        int SCREEN_HEIGHT = 640;
        int A8_x = 0; //The coordinates of the top-right corner of the board (a8 square)
        int A8_y = 0;    

        //The window and renderer and the event
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Event e;

        //The textures of the pieces
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

        //Translates the mouse move to a piece move
        void mouseMoveToPieceMove(MouseMove& move);

        //Translates the mouse position to a piece type when promotiong
        PieceType mousePosToPromotionOption(MousePos& pos);

        //Modifies some values of the window when resizing it, in order to keep the board centered and not deformed
        void resizeWindow(int width, int height);

        //Renders the board
        void renderBoard();

        //Displays the four promotion options
        void displayPromotionOptions(PieceColor color);

        //Loads all the media
        bool loadMedia();

        //Loads a single piece texture
        SDL_Texture* loadTexture(const std::string &path);
};

#endif