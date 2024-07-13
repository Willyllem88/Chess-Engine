#ifndef MYAPP_HH
#define MYAPP_HH

#include </usr/include/SDL2/SDL.h>
#include </usr/include/SDL2/SDL_image.h>
#include </usr/include/SDL2/SDL_mixer.h>

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
        
        PieceType draggedPiece; //The piece that is currently being dragged
        PiecePos draggedPiecePos; //The position of the piece that is currently being dragged

        std::set<PiecePos> targetedSquares; //The possible moves of the piece that is pressed
        std::set<PiecePos> targetedPieces; //The pieces that can be captured by the piece that is pressed

        //declare rgb colors
        const SDL_Color BACKGROUND = {0, 0, 0, 255};
        const SDL_Color WHITE_SQUARES = {255, 255, 255, 255};
        const SDL_Color BLACK_SQUARES = {100, 100, 100, 255};
        const SDL_Color PROMOTE_BACKGROUND = {175, 175, 175, 255};
        

        //PROMOTION INFORMATION

        bool promotionPending; //True if the user has to choose wich piece to promote to, false otherwise
        PieceColor promotionColor; //The color of the player that will promote

        //BOARD INFORMATION
        int piecesCount; //The number of pieces in the board
        

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
        SDL_Texture* mTargetedSquareTexture;
        SDL_Texture* mTargetedPieceTexture;

        //Audio files
        Mix_Chunk* mMoveSound;
        Mix_Chunk* mCaptureSound;

        //When clicking on a piece, its possible moves are displayed, this functions gets those squares
        void getTargets();

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

        SDL_Texture* getPieceTexture(PieceType p);
};

#endif