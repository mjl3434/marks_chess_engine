#pragma once

#include "ChessEngine.h"
#include "UniversalChessInterface.h"

// This is the main application class, all other classes are aggregated here
class Application
{
  public:
    void run();
  private:
    ChessEngine chess_engine;
    UniversalChessInterface uci;

    //UCIStateMachine uci_state_machine;  // Depends on ChessEngine, must be constructed after
};
