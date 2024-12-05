#include "Application.h"

#include <iostream>
#include <sstream>
#include <string>

void Application::run()
{
    bool app_running = true;
    std::string command;

    while (app_running) {
        std::getline(std::cin, command);  // Read the UCI command 

        // Create an istringstream to tokenize the line
        std::istringstream stream(command);
        std::string token;

        // Extract all of the tokens for this line
        std::list<std::string> tokens;
        while (stream >> token) {
            tokens.push_back(token);
        }

        // Get an enum for our state, if valid
        auto result = uci_state_machine.getCommand(tokens.front());
        if (result.has_value()) {
            std::cout << "Handling " << tokens.front() << " command\n";
            app_running = uci_state_machine.handleCommand(result.value(), tokens);
        } else {
            std::cout << "Error: Invalid command encountered.\n";
        }
    }
}
