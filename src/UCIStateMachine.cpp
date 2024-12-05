#include "UCIStateMachine.h"

#include <iostream>
#include <string>

#include "ChessEngine.h"
#include "HelperFunctions.h"

std::optional<UCIState>
UCIStateMachine::getCommand(const std::string& command)
{
    auto it = validCommands.find(command);
    if (it != validCommands.end()) {
        return it->second;
    }
    return std::nullopt; // Command not found
}

bool
UCIStateMachine::handleCommand(enum UCIState state, std::list<std::string> tokens)
{
    bool keep_running = true;

    switch (state)
    {
        case UCIState::UCI:
            handleUci(tokens);
            break;
        case UCIState::DEBUG:
            handleDebug(tokens);
            break;
        case UCIState::ISREADY:
            handleIsReady(tokens);
            break;
        case UCIState::SETOPTION:
            handleSetOption(tokens);
            break;
        case UCIState::REGISTER:
            handleRegister(tokens);
            break;
        case UCIState::UCINEWGAME:
            handleUciNewGame(tokens);
            break;
        case UCIState::POSITION:
            handlePosition(tokens);
            break;
        case UCIState::GO:
            handleGo(tokens);
            break;
        case UCIState::STOP:
            handleStop(tokens);
            break;
        case UCIState::PONDERHIT:
            handlePonderHit(tokens);
            break;
        case UCIState::QUIT:
            handleQuit(tokens);
            keep_running = false;
            break;
    }
    return keep_running;
}

void
UCIStateMachine::handleUci(std::list<std::string>& tokens)
{
    // After receiving the uci command the engine must identify itself with the "id" command
    std::string id_string = "id name " + engine_id + "\n";
    std::cout << id_string;
    std::cout << "id author Mark Larwill\n";

	// and send the "option" commands to tell the GUI which engine settings the engine supports if any.
    // option id Hash type spin default 1 min 1 max 128

	// After that the engine should send "uciok" to acknowledge the uci mode.
    std::cout << "uciok\n";
}

void
UCIStateMachine::handleDebug(std::list<std::string>& tokens)
{
    tokens.pop_front();
    std::string enabled = tokens.front();
    toLower(enabled);
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
UCIStateMachine::handleIsReady(std::list<std::string>& tokens)
{
    std::cout << "readyok\n";
}

void
UCIStateMachine::handleSetOption(std::list<std::string>& tokens)
{

}

void
UCIStateMachine::handleRegister(std::list<std::string>& tokens)
{
    // Registration not required, nor supported.
    return;
}

void
UCIStateMachine::handleUciNewGame(std::list<std::string>& tokens)
{
    chess_engine.startNewGame();
}

void
UCIStateMachine::handlePosition(std::list<std::string>& tokens)
{
    // * position [fen <fenstring> | startpos ]  moves <move1> .... <movei>
	// set up the position described in fenstring on the internal board and
	// play the moves on the internal chess board.
	// if the game was played  from the start position the string "startpos" will be sent
	// Note: no "new" command is needed. However, if this position is from a different game than
	// the last position sent to the engine, the GUI should have sent a "ucinewgame" inbetween.
    std::string token;

    tokens.pop_front(); // Discard the initial "position" token

    // Extract the next token in all lowercase
    token = tokens.front();
    toLower(token);
    tokens.pop_front();

    if (token == "fen") {
        std::string fen = tokens.front();
        tokens.pop_front();
        if (!chess_engine.isValidFen(fen)) {
            printf("Error: Invalid FEN");
            return;
        }
        chess_engine.setUpBoardFromFen(fen);
    } else if (token == "startpos") {
        tokens.pop_front();
        chess_engine.setUpBoardInStartPosition();
    } else {
        printf("Error processing position command\n");
    }

    // Now process any moves if present
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
