#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <map>

using Command = std::function<void(ChessEngine&)>;

enum class Keyword {
    UCI,
    DEBUG,
        ON,
        OFF,
    ISREADY,
    SETOPTION,
        NAME,
        VALUE,
    REGISTER,
        LATER,
        // NAME (duplicated from above, different meaning here)
        CODE,
    UCINEWGAME,
    POSITION,
        FEN,
        STARTPOS,
            MOVES,
    GO,
        SEARCHMOVES,
        PONDER,
        WTIME,
        BTIME,
        WINC,
        BINC,
        MOVESTOGO,
        DEPTH,
        NODES,
        MATE,
        MOVETIME,
        INFINITE,
    STOP,
    PONDERHIT,
    QUIT
};

// This class is in charge of knowing uci command syntax, parsing input,
// validating it, and if valid creating a command, to send to the chess engine.
class UniversalChessInterface
{
  public:
    
    bool isValidFen(const std::string&);
    bool isValidAlgebraicNotation(const std::string&);
    bool quitReceived(const std::string& input);
    std::optional<Command> getCommand(const std::string&);
    bool isValidDebugCommand(std::list<std::string>& tokens, bool&);
    bool isValidSetoptionCommand(const std::string& input,
            std::string& option_name, std::string& option_value);
    bool isValidPositionCommand(std::list<std::string>& tokens,
            std::string& fen, std::list<Move>& moves);

    std::unordered_map<std::string, enum Keywords> valid_keywords = {
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

