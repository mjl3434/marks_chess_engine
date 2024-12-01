#include <iostream>
#include <sstream>
#include <string>

#include "UCIStateMachine.h"

int main(int argc, char* argv[])
{
    std::string command;
    UCIStateMachine uci_state_machine;

    while (true) {
        std::getline(std::cin, command);  // Read the UCI command 

        // Create an istringstream to tokenize the line
        std::istringstream stream(command);
        std::string token;

        std::cout << "Received Command: " << command << "\n";

        // Extract all of the tokens for this line
        std::list<std::string> tokens;
        while (stream >> token) {
            tokens.push_back(token);
        }

        // Get an enum for our state, if valid
        auto result = uci_state_machine.getCommand(tokens.front());
        if (result.has_value()) {
            std::cout << "Found command: " << tokens.front() << "\n";
            // Act on it
            uci_state_machine.handleCommand(result.value(), tokens);
        } else {
            std::cout << "Error: Invalid command encountered.\n";
        }
    }

    return 0;
}
