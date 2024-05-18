#include "board.hh"
#include "myApp.hh"

int main(int argc, char* argv[]) {
    //IT MIGHT BE AN IF STATEMENT HERE
    // Inicialize the board
    MyApp myApp;
    Board board;

    if (!myApp.init())
        return 1;


    //Checks all the input options, and processes them
    for (int i = 1; i < argc; ++i) {
        std::cout << argv[i] << std::endl; //DELETE, just for testing
        //switch (argv[i]) {
        //    case "-b":
        //    (...)
        //}
    }

    while (1) {
        if (!myApp.handleEvents())
            break;
        board.printBoardApp(&myApp);
    }

    myApp.free();
    return 0;
}