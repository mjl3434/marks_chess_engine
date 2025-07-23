#pragma once

#include <atomic>
#include <thread>

#include "ChessEngine.h"
#include "CommandQueue.h"
#include "UniversalChessInterface.h"
#include "Worker.h"

// This is the main application class, all other classes are aggregated here
class Application
{
  public:
    Application();
    ~Application();
    void run();

  private:
    ChessEngine chess_engine;
    CommandQueue command_queue;
    UniversalChessInterface uci;
    Worker worker; // Depends on ChessEngine, and CommandQueue, must be constructed after them

    //UCIStateMachine uci_state_machine;  // Depends on ChessEngine, must be constructed after
};
