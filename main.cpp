#include "board.hh"
#include "myApp.hh"
#include "players.hh"

void manageError(const char* message) {
    std::cerr << message << std::endl;
    exit(1);
}

void usage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "    --help, -h: Displays this message." << std::endl;
    std::cout << "    --white, -w <PLAYER | <engine_name>>: Specify who will play with the white pieces." << std::endl;
    std::cout << "    --black, -b <PLAYER | <engine_name>>: Specify who will play with the black pieces." << std::endl;
    std::cout << "    --console-only, -c: The GUI will not be displayed." << std::endl;
    std::cout << "    --timespan <time>, -t <time>: The time span in seconds for the engine to play a turn." << std::endl;
    std::cout << std::endl;
    std::cout << "The default options are:" << std::endl;
    std::cout << "    --white PLAYER, --black PLAYER --timespan 0" << std::endl;

    exit(0);
}

void processCommandLine(int argc, char* argv[], std::string& whitePlayer, std::string& blackPlayer, bool& displayGUIApp, int& engineTimeSpan) {
    if (argc == 1)
        return;

    //Options for the command line
    struct option longOptions[] = {
        {"help",         no_argument,       0, 'h'},
        {"black",        no_argument,       0, 'b'},
        {"white",        no_argument,       0, 'w'},
        {"console-only", no_argument,       0, 'c'},
        {"timespan",     required_argument, 0, 't'},
        {0, 0, 0, 0}
    };

    //Handles the options
    int opt;
    int optionIndex = 0;
    while ((opt = getopt_long(argc, argv, "hb:w:ct:", longOptions, &optionIndex)) != -1) {
        switch (opt) {
            case 'h': //Help
                usage(argv[0]);
                break;
            case 'w': //White player
                if (optarg == nullptr) manageError("ERROR: You must specify the white player.");
                whitePlayer = optarg;
                break;
            case 'b': //Black player
                if (optarg == nullptr) manageError("ERROR: You must specify the white player.");
                blackPlayer = optarg;
                break;
            case 'c': //Console only
                displayGUIApp = false;
                break;
            case 't': //Engine time span
                engineTimeSpan = std::stoi(optarg);
                break;
            default: //Invalid option
                std::cerr << "Invalid option" << std::endl;
                exit(1);
        }
    }
    //Displays the options chosen
    std::cout << "Options chosen:" << std::endl;
    std::cout << "  White player: " << whitePlayer << std::endl;
    std::cout << "  Black player: " << blackPlayer << std::endl;
    std::cout << "  Display GUI: " << (displayGUIApp ? "Yes" : "No") << std::endl;
    std::cout << "  Engine time span: " << engineTimeSpan << " s" << std::endl;
    std::cout << std::endl;

    return;
}

int main(int argc, char* argv[]) {
    std::cout << "---------------Guillem's Chess---------------" << std::endl;
    std::cout << "For displaying the usage --help or -h." << std::endl << std::endl;

    //Default options for the game
    std::string whitePlayerName = "PLAYER";
    std::string blackPlayerName = "PLAYER";
    bool displayGUIApp = false;
    int engineTimeSpan = 0;

    // Inicialization of the app and the board
    std::shared_ptr<MyApp> myApp = std::make_shared<GUIApp>();
    std::shared_ptr<Board> myBoard = std::make_shared<Board>(myApp);
    myApp->setBoard(myBoard);

    //Handles the command line arguments
    processCommandLine(argc, argv, whitePlayerName, blackPlayerName, displayGUIApp, engineTimeSpan);

    //Loads both players
    std::unique_ptr<Player> whitePlayer, blackPlayer;
    if (whitePlayerName == "PLAYER") whitePlayer = std::make_unique<HumanPlayer>(myApp);
    else whitePlayer = std::make_unique<EngineV1>(myBoard);
    if (blackPlayerName == "PLAYER") blackPlayer = std::make_unique<HumanPlayer>(myApp);
    else blackPlayer = std::make_unique<EngineV1>(myBoard);

    //Initializes the app, if it fails, the program will exit
    if (!myApp->init())
        return 1;

    //The match starts here
    PieceMove move;
    while (1) {
        myBoard->printBoardApp();
        if (!myApp->handleEvents()) break;

        if (whitePlayer->canMove() && myBoard->getMoveTurn() == WHITE) {
            move = whitePlayer->getMove();
            myBoard->movePiece(move);
        }

        if (blackPlayer->canMove() && myBoard->getMoveTurn() == BLACK) {
            move = blackPlayer->getMove();
            myBoard->movePiece(move);
        }
    }

    return 0;
}