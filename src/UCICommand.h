#pragma once

#include <list>
#include <string>

#include "ChessGame.h"

class ChessEngine; // Forward declaration

class UCICommand {
public:
    UCICommand(const std::list<std::string>& tokens) : command_as_tokens(tokens) {}
    UCICommand() = default;
    virtual ~UCICommand() = default;
    virtual void doCommand(ChessEngine& engine) = 0;
protected:
    std::list<std::string> command_as_tokens;
};

class DebugCommand : public UCICommand {
public:
    DebugCommand(const std::list<std::string>& tokens)
        : UCICommand(tokens)
    {}
    void doCommand(ChessEngine& engine) override;
    bool debug_enabled = false;
};

class UciCommand : public UCICommand {
public:
    UciCommand(const std::list<std::string>& tokens)
        : UCICommand(tokens)
    {}
    void doCommand(ChessEngine& engine) override;
};

class IsReadyCommand : public UCICommand {
public:
    IsReadyCommand(const std::list<std::string>& tokens)
        : UCICommand(tokens) 
    {}
    void doCommand(ChessEngine& engine) override;
};

class UciNewGameCommand : public UCICommand {
public:
    UciNewGameCommand(const std::list<std::string>& tokens)
        : UCICommand(tokens) 
    {}
    void doCommand(ChessEngine& engine) override;
};

class SetOptionCommand : public UCICommand {
public:
    SetOptionCommand(const std::list<std::string>& tokens)
        : UCICommand(tokens) 
    {}
    void doCommand(ChessEngine& engine) override;
    std::string name;
    std::string value;
};

class PositionCommand : public UCICommand {
public:
    PositionCommand(const std::list<std::string>& tokens)
        : UCICommand(tokens) 
    {}
    void doCommand(ChessEngine& engine) override;
    std::string fen;
    std::list<Move> moves;
};

class GoCommand : public UCICommand {
public:
    GoCommand(const std::list<std::string>& tokens)
        : UCICommand(tokens) 
    {}
    void doCommand(ChessEngine& engine) override;
    std::list<Move> search_moves;   // Restrict search to these moves
    bool ponder = false;            // Enter ponder mode
    bool infinite = false;          // No time limit, search until "stop" command
    int32_t max_movetime_ms = -1;   // Limit search to this time in milliseconds
    int32_t max_nodes = -1;         // Limit search to this many nodes
    int32_t mate_in_x = -1;         // Search for mate in this many moves
    int32_t max_depth = -1;         // Limit search to this many plys (half moves)
    int32_t wtime_ms = -1;          // Game time left for white in milliseconds
    int32_t btime_ms = -1;          // Game time left for black in milliseconds
    int32_t winc_ms = -1;           // Time increment per move for white in milliseconds
    int32_t binc_ms = -1;           // Time increment per move for black in milliseconds
    int32_t movestogo = -1;         // Moves until next time control

};

class StopCommand : public UCICommand {
public:
    StopCommand(const std::list<std::string>& tokens)
        : UCICommand(tokens) 
    {}
    void doCommand(ChessEngine& engine) override;
};

class PonderHitCommand : public UCICommand {
public:
    PonderHitCommand(const std::list<std::string>& tokens)
        : UCICommand(tokens) 
    {}
    void doCommand(ChessEngine& engine) override;
};

class QuitCommand : public UCICommand {
public:
    QuitCommand(const std::list<std::string>& tokens)
        : UCICommand(tokens) 
    {}
    QuitCommand()
        : UCICommand() 
    {}
    void doCommand(ChessEngine& engine) override;
};