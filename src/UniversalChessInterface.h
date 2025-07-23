#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ChessEngine.h"
#include "Keyword.h"
#include "UCICommand.h"

// This class is in charge of knowing uci command syntax, parsing input,
// validating it, and if valid creating a command, to send to the chess engine.
// It does not know anything about chess. It can't validate that a chess move
// is legal or not.
class UniversalChessInterface
{
  public:
    
    bool isValidFen(const std::string&);
    bool isValidAlgebraicNotation(const std::string&);
    bool quitReceived(const std::string& input);
    std::unique_ptr<UCICommand> getCommand(const std::string&);
    bool isValidDebugCommand(const std::list<std::string>& tokens, DebugCommand& command);
    bool isValidSetoptionCommand(const std::string& input, SetOptionCommand& set_option_command);
    bool isValidPositionCommand(std::list<std::string>& tokens, PositionCommand& position_command);
    bool isValidGoCommand(std::list<std::string>& tokens, GoCommand& go_command);
    bool isValidNoArgCommand(std::list<std::string>& tokens);

    std::unordered_map<std::string, enum Keyword> valid_keywords = {
        { "uci",          Keyword::UCI },
        { "debug",        Keyword::DEBUG },
        { "on",           Keyword::ON },
        { "off",          Keyword::OFF },
        { "isready",      Keyword::ISREADY },
        { "setoption",    Keyword::SETOPTION },
        { "name",         Keyword::NAME },
        { "value",        Keyword::VALUE },
        { "register",     Keyword::REGISTER },
        { "later",        Keyword::LATER },
        { "code",         Keyword::CODE },
        { "ucinewgame",   Keyword::UCINEWGAME },
        { "position",     Keyword::POSITION },
        { "fen",          Keyword::FEN },
        { "startpos",     Keyword::STARTPOS },
        { "moves",        Keyword::MOVES },
        { "go",           Keyword::GO },
        { "searchmoves",  Keyword::SEARCHMOVES },
        { "ponder",       Keyword::PONDER },
        { "wtime",        Keyword::WTIME },
        { "btime",        Keyword::BTIME },
        { "winc",         Keyword::WINC },
        { "binc",         Keyword::BINC },
        { "movestogo",    Keyword::MOVESTOGO },
        { "depth",        Keyword::DEPTH },
        { "nodes",        Keyword::NODES },
        { "mate",         Keyword::MATE },
        { "movetime",     Keyword::MOVETIME },
        { "infinite",     Keyword::INFINITE },
        { "stop",         Keyword::STOP },
        { "ponderhit",    Keyword::PONDERHIT },
        { "quit",         Keyword::QUIT }
    };

    std::unordered_set<std::string> valid_options = {
        { "none" }
    };
};

