#include "UCIStateMachine.h"

#include <iostream>

std::optional<UCIState>
UCIStateMachine::getCommand(const std::string& command)
{
    auto it = validCommands.find(command);
    if (it != validCommands.end()) {
        return it->second;
    }
    return std::nullopt; // Command not found
}

void
UCIStateMachine::handleCommand(enum UCIState state, std::list<std::string> tokens)
{
    switch (state)
    {
        case UCIState::UCI:
            handleUci(tokens);
            break;
        case UCIState::DEBUG:
            handleDebug(tokens);
            break;
        case UCIState::ISREADY:
            break;
        case UCIState::SETOPTION:
            break;
        case UCIState::REGISTER:
            break;
        case UCIState::UCINEWGAME:
            break;
        case UCIState::POSITION:
            break;
        case UCIState::GO:
            break;
        case UCIState::STOP:
            break;
        case UCIState::PONDERHIT:
            break;
        case UCIState::QUIT:
            break;
    }
}

void
UCIStateMachine::handleUci(std::list<std::string>& tokens)
{
    // After receiving the uci command the engine must identify itself with the "id" command
    std::string id_string = "id " + engine_id + "\n";
    std::cout << id_string;

	// and send the "option" commands to tell the GUI which engine settings the engine supports if any.
    // option id Hash <value in MB>

	// After that the engine should send "uciok" to acknowledge the uci mode.
    std::cout << "uciok\n";
}

void
UCIStateMachine::handleDebug(std::list<std::string>& tokens)
{
    tokens.pop_front();
    std::string enabled = tokens.front();
    if (enabled == "on") {
        // In debug mode the engine should send additional infos to the GUI,
        // e.g. with the "info string" command,	to help debugging, e.g. the
        // commands that the engine has received etc.
        debug_enabled = true;
        printf("Debug mode enabled.\n");
    } else if (enabled == "off") {
        debug_enabled = false;
        printf("Debug mode disabled.\n");
    }
}

void
UCIStateMachine::handleIsready(std::list<std::string>& tokens)
{

}

void
UCIStateMachine::handleSetOption(std::list<std::string>& tokens)
{

}

void
UCIStateMachine::handleRegister(std::list<std::string>& tokens)
{

}

void
UCIStateMachine::handleUciNewGame(std::list<std::string>& tokens)
{

}

void
UCIStateMachine::handlePosition(std::list<std::string>& tokens)
{

}

void
UCIStateMachine::handleGo(std::list<std::string>& tokens)
{

}

void
UCIStateMachine::handleStop(std::list<std::string>& tokens)
{

}

void
UCIStateMachine::handlePonderHit(std::list<std::string>& tokens)
{

}

void
UCIStateMachine::handleQuit(std::list<std::string>& tokens)
{

}
