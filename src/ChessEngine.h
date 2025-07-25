#pragma once

#include <list>
#include <memory>
#include <string>
#include <thread>
#include <utility>

#include "ChessGame.h"
#include "UCICommand.h"

#define DEFAULT_MAX_SEARCH_DEPTH 6

// Note: Thrown together, this may have to change
struct SearchResult {
    Move best_move;
    int score;
    int depth_searched;
    int nodes_searched;
    bool is_checkmate;
    bool is_stalemate;
    std::vector<Move> principal_variation; // Best line of play
};

class ChessEngine
{
public:
    ChessEngine();
    void start();
    void stop();

    // Command handlers
    void doDebugCommand(DebugCommand& command);
    void doGoCommand(GoCommand& command);
    void doIsReadyCommand(IsReadyCommand& command);
    void doPonderHitCommand(PonderHitCommand& command);
    void doPositionCommand(PositionCommand& command);
    void doQuitCommand(QuitCommand& command);
    void doSetOptionCommand(SetOptionCommand& command);
    void doStopCommand(StopCommand& command);
    void doUciCommand(UciCommand& command);
    void doUciNewGameCommand(UciNewGameCommand& command);

private:
    // The main loop
    void spin(void);

    // Main implementation functions
    SearchResult findBestMove(const GameState& game_state, const GoCommand& go_command);

    // Helper functions
    void setUpBoardFromFen(const std::string&);
    void printSupportedOptions(void);
    void playMove(const Move&);

    // Private data
    bool _debug_enabled = false;
    std::string engine_name = "Mark's Chess Engine Version 1.0";
    std::string author = "Mark Larwill";
    std::unique_ptr<ChessGame> _game;
    std::thread _engine_thread;
};
