#include "CommandQueue.h"

void
CommandQueue::enqueue(Command&& command)
{
    std::unique_lock<std::mutex> lock(command_queue_mutex);
    command_queue.push(std::move(command));
    // Notify worker thread something is waiting dequeue
    command_queue_condition.notify_one();
}

std::optional<Command>
CommandQueue::dequeue()
{
    std::unique_lock<std::mutex> lock(command_queue_mutex);

    // Wait until: queue has items OR shutdown is requested
    command_queue_condition.wait(lock, [this]() {
        return !command_queue.empty() || queue_shutdown;
    });

    // If shutdown and no commands, return empty optional
    if (queue_shutdown && command_queue.empty()) {
        return std::nullopt;  // Signal shutdown gracefully
    }

    Command command = std::move(command_queue.front());
    command_queue.pop();
    return command;
}

void
CommandQueue::shutdown()
{
    std::unique_lock<std::mutex> lock(command_queue_mutex);
    while (!command_queue.empty()) {
        command_queue.pop();
    }
    queue_shutdown = true;
    // Notify worker thread to wake up and exit
    command_queue_condition.notify_one();
}