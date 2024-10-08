#include "game.hpp"

//Static variables
std::atomic<bool> Game::running;
std::atomic<MyApp::eventType> Game::lastEvent;
std::atomic<bool> Game::lastEventHandled;
std::atomic<bool> Game::gameInIdle;

void Game::run(int argc, char* argv[]) {
    time_t seed = time(NULL);
    srand(seed);

    //Prints the welcome message
    printWelcome(seed);

    //Default options for the game
    std::string whitePlayerName = "Player";
    std::string blackPlayerName = "Player";
    bool displayGUIApp = true;
    std::chrono::milliseconds engineTimeSpan(2000);
    std::string fenBoard = "";

    //Handles the command line arguments
    processCommandLine(argc, argv, whitePlayerName, blackPlayerName, displayGUIApp, engineTimeSpan, fenBoard);

    printOptionsChosen(whitePlayerName, blackPlayerName, displayGUIApp, engineTimeSpan, fenBoard);

    // Inicialization of the app and the board
    std::shared_ptr<Board> myBoard;
    std::shared_ptr<MyApp> myApp;
    initializeBoardApp(myBoard, myApp, displayGUIApp, fenBoard);

    //Loads both players
    std::unique_ptr<Player> whitePlayer, blackPlayer;
    loadPlayers(whitePlayer, whitePlayerName, myApp, myBoard, engineTimeSpan);
    loadPlayers(blackPlayer, blackPlayerName, myApp, myBoard, engineTimeSpan);

    //Initializes the app, if it fails, the program will exit
    if (!myApp->init())
        errorAndExit("ERROR: The app could not be initialized.");

    //Prints the initial board
    myApp->printBoard(myBoard->getPieceMatrix());

    //Creates a thread to handle the events
    running = true;
    lastEvent = MyApp::NO_EVENT;
    lastEventHandled = true;
    gameInIdle = false;
    std::thread eventThread(eventDetector, std::ref(myApp), std::ref(whitePlayer), std::ref(blackPlayer));

    //The match starts here
    PieceMove move;
    while (running) {
        //Player movement logic
        if (myBoard->getMoveTurn() == WHITE && whitePlayer->canMove() && !gameInIdle) {
            move = whitePlayer->getMove();
            if (!whitePlayer->wasInterrupted()) {
                myBoard->movePiece(move);
                myBoard->printLastMove();
            }
        }
        else if (myBoard->getMoveTurn() == BLACK && blackPlayer->canMove() && !gameInIdle) {
            move = blackPlayer->getMove();
            if (!blackPlayer->wasInterrupted()) {
                myBoard->movePiece(move);
                myBoard->printLastMove();
            }
        }

        eventHandler(myBoard);

        myApp->printBoard(myBoard->getPieceMatrix());
        if (myBoard->getBoardResult() != PLAYING) break;
        
        //Sleeps for a while to decrease CPU usage
        std::this_thread::sleep_for(LOOP_SLEEP_TIME);
    }

    //Check if the game has concluded, then print the result
    if (myBoard->getBoardResult() != PLAYING) myBoard->printResult();

    //Waits for the user to close the app
    while(running) eventHandler(myBoard);

    //Joins the event thread
    if (eventThread.joinable()) eventThread.join();
}

void Game::eventHandler(std::shared_ptr<Board> myBoard) {
    if (lastEvent == MyApp::QUIT) {
        running = false;
    }
    else if (lastEvent == MyApp::UNDO) {
        myBoard->undoMove();
        //There will be a cooldown of a second where no player can move, to prevent the players from instantly making a move
        std::thread idleCooldown([&](){
            gameInIdle = true;
            std::this_thread::sleep_for(IDLE_TIME);
            gameInIdle = false;
        });
        idleCooldown.detach();
    }
    lastEventHandled = true;
    lastEvent = MyApp::NO_EVENT;
}

void Game::eventDetector(std::shared_ptr<MyApp> myApp, std::unique_ptr<Player>& whitePlayer, std::unique_ptr<Player>& blackPlayer) {
    while (running) {
        if (lastEventHandled) {
            lastEvent = myApp->handleEvents();
            if (lastEvent != MyApp::NO_EVENT) {
                whitePlayer->interrupt();
                blackPlayer->interrupt();
                lastEventHandled = false; 
            }
        }

        //Sleeps for a while to decrease CPU usage
        std::this_thread::sleep_for(LOOP_SLEEP_TIME);
    }
}

void Game::printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Play a game of chess with the engine." << std::endl << std::endl;
    std::cout << "Commands (through the console):" << std::endl;
    std::cout << "    u, undo: in the console will undo the last move." << std::endl;
    std::cout << "    <algebraic-move-notation>: if it's a valid move, it will perform it." << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "    --help, -h: Displays this message." << std::endl;
    std::cout << "    --white, -w <Player | <engine_name>>: specify who will play with the white pieces." << std::endl;
    std::cout << "    --black, -b <Player | <engine_name>>: specify who will play with the black pieces." << std::endl;
    std::cout << "    --console-only, -c: the GUI will not be displayed." << std::endl;
    std::cout << "    --timespan <time>, -t <time>: the time span in seconds for the engine to play a turn, can use decimals." << std::endl;
    std::cout << "    --load-fen \"<fen>\", -f \"<fen>\": load a FEN board. IMPORTANT: The FEN string must be enclosed in quotes." << std::endl;
    std::cout << std::endl;
    std::cout << "The default options are:" << std::endl;
    std::cout << "    --white Player, --black Player --timespan 2" << std::endl;
    std::cout << "----------------------------------------------------" << std::endl << std::endl;

    exit(0);
}

void Game::printWelcome(unsigned int seed) {
    std::cout << "-------------- Guillem's Chess Engine --------------" << std::endl;
    std::cout << "For displaying the usage --help or -h." << std::endl;
    std::cout << "The current available engines are: " << std::endl;
    std::cout << "    - RandomEngine" << std::endl;
    std::cout << "    - EngineV1" << std::endl;
    std::cout << "The random seed used was: " << seed << "." << std::endl;
    std::cout << "----------------------------------------------------" << std::endl << std::endl;;
}

void Game::printOptionsChosen(const std::string& whitePlayer,const std::string& blackPlayer, bool displayGUIApp, std::chrono::milliseconds engineTimeSpan, const std::string& FEN) {
    std::cout << "Options chosen:" << std::endl;
    std::cout << "    - White player: " << whitePlayer << std::endl;
    std::cout << "    - Black player: " << blackPlayer << std::endl;
    std::cout << "    - Display method: " << (displayGUIApp ? "GUI" : "Console") << std::endl;
    std::cout << "    - Engine time span: " << engineTimeSpan.count() / 1000.0 << " s" << std::endl;
    std::cout << "    - FEN: " << (FEN == "" ? "default" : FEN) << std::endl;
    std::cout << "----------------------------------------------------" << std::endl << std::endl;;
}

void Game::processCommandLine(int argc, char* argv[], std::string& whitePlayer, std::string& blackPlayer, bool& displayGUIApp, std::chrono::milliseconds& engineTimeSpan, std::string& FEN) {
    if (argc == 1)
        return;

    //Options for the command line
    struct option longOptions[] = {
        {"help",         no_argument,       0, 'h'},
        {"black",        no_argument,       0, 'b'},
        {"white",        no_argument,       0, 'w'},
        {"console-only", no_argument,       0, 'c'},
        {"timespan",     required_argument, 0, 't'},
        {"load-fen",     required_argument, 0, 'f'},
        {0, 0, 0, 0}
    };
    std::vector<std::string> fenParts;
    //Handles the options
    int opt;
    int optionIndex = 0;
    while ((opt = getopt_long(argc, argv, "hb:w:ct:f:", longOptions, &optionIndex)) != -1) {
        switch (opt) {
            case 'h': //Help
                printUsage(argv[0]);
                break;
            case 'w': //White player
                if (optarg == nullptr) errorAndExit("ERROR: You must specify the white player.");
                whitePlayer = optarg;
                break;
            case 'b': //Black player
                if (optarg == nullptr) errorAndExit("ERROR: You must specify the white player.");
                blackPlayer = optarg;
                break;
            case 'c': //Console only
                displayGUIApp = false;
                break;
            case 't': //Engine time span
                //stof
                engineTimeSpan = std::chrono::milliseconds(int(std::stof(optarg) * 1000));
                break;
            case 'f': //Load FEN
                FEN = optarg;
                break;
            default: //Invalid option
                std::cerr << "Invalid option" << std::endl;
                exit(1);
        }
    }
    return;
}

void Game::initializeBoardApp(std::shared_ptr<Board>& myBoard, std::shared_ptr<MyApp>& myApp, bool displayGUIApp, const std::string& FEN) {
    //Initializes the board
    myBoard = std::make_shared<Board>();

    //Initializes the app
    if (displayGUIApp) myApp = std::make_shared<GUIApp>();
    else myApp = std::make_shared<ConsoleApp>();

    //Links the board with the app
    myApp->setBoard(myBoard);

    //Initializes the pieces on the board
    if (FEN == "") myBoard->setDefaulValues();
    else myBoard->loadFEN(FEN);
}

void Game::loadPlayers(std::unique_ptr<Player>& player, const std::string& playerName, std::shared_ptr<MyApp> myApp, std::shared_ptr<Board> myBoard, std::chrono::milliseconds engineTimeSpan) {
    if (playerName == "Player") player = std::make_unique<HumanPlayer>(myApp);
    else if (playerName == "RandomEngine") player = std::make_unique<RandomEngine>(myBoard);
    else if (playerName == "EngineV1") player = std::make_unique<EngineV1>(myBoard, engineTimeSpan);
    else errorAndExit("ERROR: " + playerName + " is not a valid player.");
}