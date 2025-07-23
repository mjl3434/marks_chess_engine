#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

#include "UniversalChessInterface.h"

class CommandQueue
{
public:
    void enqueue(Command&& command);
    std::optional<Command> dequeue();
    void shutdown();

private:

    bool queue_shutdown = false;
    std::queue<Command> command_queue;
    std::mutex command_queue_mutex;
    std::condition_variable command_queue_condition;
};