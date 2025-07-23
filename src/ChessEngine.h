#pragma once

#include <list>
#include <memory>
#include <string>
#include <thread>
#include <utility>

#include "ChessGame.h"
#include "UCICommand.h"

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

    // Helper functions
    void setUpBoardFromFen(const std::string&);
    void printSupportedOptions(void);
    void playMove(const Move&);

    // Private data
    bool debug_enabled = false;
    std::string engine_name = "Mark's Chess Engine Version 1.0";
    std::string author = "Mark Larwill";
    std::unique_ptr<ChessGame> game;
    std::thread engine_thread;
};
