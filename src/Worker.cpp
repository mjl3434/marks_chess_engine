#include "Worker.h"

Worker::Worker(ChessEngine& chess_engine, CommandQueue& command_queue)
    : chess_engine(chess_engine),
      command_queue(command_queue)
{
    // Constructor implementation can be added here if needed
}

void
Worker::start()
{
    // Start the worker thread that processes commands from the command queue
    worker_thread = std::thread([this]() {
        while (true) {
            std::optional<Command> command = command_queue.dequeue();
            if (command) {
                (*command)(chess_engine);
            } else {
                // If command is empty, it means the queue was shutdown
                break;  // Exit the loop gracefully
            }
        }
    });
}

void
Worker::stop()
{
    command_queue.shutdown(); // Clear the command queue and notify the worker
    if (worker_thread.joinable()) { // Wait for clean exit
        worker_thread.join();
    }
}