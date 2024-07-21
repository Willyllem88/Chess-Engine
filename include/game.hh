#ifndef GAME_HH
#define GAME_HH

#include "board.hh"
#include "myApp.hh"
#include "players.hh"
#include "engine_v1.hh"

class Game {
public:
    // Runs the game
    static void run(int argc, char* argv[]);

private:
    //Makes the needed actions to handle each event. Requires another thread running the eventDetector function
    static void eventHandler(std::shared_ptr<Board> myBoard);

    // Detects the events of the game, will run in a separate thread. If it detects an event that needs to be handled, it will set lastEvent and lastEventHandled variable to false, and the eventHandler will handle the event, in the main thread. Also, it will interrupt the players if needed 
    static void eventDetector(std::shared_ptr<MyApp> myApp, std::unique_ptr<Player>& whitePlayer, std::unique_ptr<Player>& blackPlayer);

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

    static std::atomic<bool> running; //True if the game is running
    static std::atomic<MyApp::eventType> lastEvent; //The last event that happened
    static std::atomic<bool> lastEventHandled; //True if the last event has been handled
    static std::atomic<bool> gameInIdle; //True if the game is in idle state, used to prevent the players from making moves

    static constexpr std::chrono::milliseconds LOOP_SLEEP_TIME = std::chrono::milliseconds(10); //The time that the game will sleep in the main loop
    static constexpr std::chrono::milliseconds IDLE_TIME = std::chrono::milliseconds(1000); //The time that the game will be in idle state after an event
};

#endif