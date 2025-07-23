#pragma once

#include <atomic>
#include <thread>

#include "ChessEngine.h"
#include "ThreadSafeQueue.h"
#include "UniversalChessInterface.h"

extern ThreadSafeQueue<std::unique_ptr<UCICommand>> command_queue;

// This is the main application class, all other classes are aggregated here
class Application
{
  public:
    Application();
    ~Application();
    void run();

  private:
    ChessEngine _chess_engine;
    UniversalChessInterface _uci;
};
