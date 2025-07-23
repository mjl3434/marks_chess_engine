#pragma once

#include <atomic>
#include <thread>

#include "ChessEngine.h"
#include "CommandQueue.h"

class Worker
{
public:
    Worker(ChessEngine& chess_engine, CommandQueue& command_queue);
    void start();
    void stop();

private:
    void spin();

    ChessEngine& chess_engine;
    CommandQueue& command_queue;
    std::thread worker_thread;
    std::atomic<bool> worker_retired;
};