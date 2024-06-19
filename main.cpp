#include "board.hh"
#include "myApp.hh"

int main(int argc, char* argv[]) {
    // Inicialization of the app and the board
    std::shared_ptr<MyApp> myApp = std::make_shared<GUIApp>();
    std::shared_ptr<Board> board = std::make_shared<Board>(myApp);
    myApp->setBoard(board);

    if (!myApp->init())
        return 1;


    //DELETE, just for testing
    for (int i = 1; i < argc; ++i) {
        std::cout << argv[i] << std::endl; //DELETE, just for testing
    }

    board->printBoardApp();
    PieceMove lastPieceMove;
    while (1) {
        if (!myApp->handleEvents())
            break;
        
        if (myApp->isPieceMoveAvailable()) {
            lastPieceMove = myApp->getMove();
            board->movePiece(lastPieceMove);
            board->printBoardApp();
            //SDL_Delay(100);
        }
    }

    return 0;
}