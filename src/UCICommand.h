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
    bool ponder = false;
    bool infinite = false;
    int32_t movetime = -1;
    int32_t wtime = -1;
    int32_t btime = -1;
    int32_t winc = -1;
    int32_t binc = -1;
    int32_t nodes = -1;
    int32_t movestogo = -1;
    int32_t mate = -1;
    int32_t depth = -1;
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