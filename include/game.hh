#ifndef GAME_HH
#define GAME_HH

#include "board.hh"
#include "myApp.hh"
#include "players.hh"

class Game {
public:
    // Runs the game
    static void run(int argc, char* argv[]);

private:
    // Handles the events of the game
    static void eventHandler(std::atomic<bool>& running, std::shared_ptr<MyApp> myApp, std::unique_ptr<Player>& whitePlayer, std::unique_ptr<Player>& blackPlayer);

    // Prints usage information
    static void printUsage(const char* programName);

    // Prints the welcome message
    static void printWelcome(unsigned int seed);

    // Prints the chosen options
    static void printOptionsChosen(const std::string& whitePlayer, const std::string& blackPlayer, bool displayGUIApp, std::chrono::milliseconds engineTimeSpan, const std::string& FEN);

    // Processes the command line arguments
    static void processCommandLine(int argc, char* argv[], std::string& whitePlayerName, std::string& blackPlayerName, bool& displayGUIApp, std::chrono::milliseconds& engineTimeSpan, std::string& FEN);

    // Initializes the board and application
    static void initializeBoardApp(std::shared_ptr<Board>& myBoard, std::shared_ptr<MyApp>& myApp, bool displayGUIApp, const std::string& FEN);
    
    // Loads the players based on provided names
    static void loadPlayers(std::unique_ptr<Player>& player, const std::string& playerName, std::shared_ptr<MyApp> myApp, std::shared_ptr<Board> myBoard, std::chrono::milliseconds engineTimeSpan);
};

#endif