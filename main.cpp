#include "board.hh"
#include "myApp.hh"

int main(int argc, char* argv[]) {
    //IT MIGHT BE AN IF STATEMENT HERE
    // Inicialize the board
    MyApp* myApp = new GUIApp();
    Board board(myApp);

    if (!myApp->init())
        return 1;


    //DELETE, just for testing
    for (int i = 1; i < argc; ++i) {
        std::cout << argv[i] << std::endl; //DELETE, just for testing
    }

    PieceMove lastPieceMove;
    while (1) {
        if (!myApp->handleEvents())
            break;
        
        if (myApp->isPieceMoveAvailable()) {
            lastPieceMove = myApp->getMove();
            board.movePiece(lastPieceMove);
        }
        board.printBoardApp();
    }

    delete myApp;
    return 0;
}