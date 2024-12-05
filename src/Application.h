#pragma once

#include "UCIStateMachine.h"
#include "ChessEngine.h"

// This is the main application class, all other classes are aggregated here
class Application
{
  public:
    Application() : uci_state_machine(chess_engine) {}
    void run();
  private:
    ChessEngine chess_engine;
    UCIStateMachine uci_state_machine;  // Depends on ChessEngine, must be constructed after
};
