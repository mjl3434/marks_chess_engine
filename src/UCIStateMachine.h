#ifndef UCIStateMachine_H
#define UCIStateMachine_H

#include <list>
#include <string>
#include <unordered_map>
#include <optional>

enum class UCIState {
    UCI,
    DEBUG,
    ISREADY,
    SETOPTION,
    REGISTER,
    UCINEWGAME,
    POSITION,
    GO,
    STOP,
    PONDERHIT,
    QUIT
};

//
// This class implements the state machine for a UCI Chess Engine. The UCI
// protocol is defined here:
//     https://gist.github.com/DOBRO/2592c6dad754ba67e6dcaec8c90165bf
//
class UCIStateMachine
{
  public:

    // Functions
    std::optional<UCIState> getCommand(const std::string& command);
    void handleCommand(enum UCIState state, std::list<std::string> tokens);

  private:

    // Functions
    void handleUci(std::list<std::string>& tokens);
    void handleDebug(std::list<std::string>& tokens);
    void handleIsready(std::list<std::string>& tokens);
    void handleSetOption(std::list<std::string>& tokens);
    void handleRegister(std::list<std::string>& tokens);
    void handleUciNewGame(std::list<std::string>& tokens);
    void handlePosition(std::list<std::string>& tokens);
    void handleGo(std::list<std::string>& tokens);
    void handleStop(std::list<std::string>& tokens);
    void handlePonderHit(std::list<std::string>& tokens);
    void handleQuit(std::list<std::string>& tokens);

    // Data Members
    std::unordered_map<std::string, enum UCIState> validCommands = {
        { "uci", UCIState::UCI },
        { "debug", UCIState::DEBUG },
        { "isready", UCIState::ISREADY },
        { "setoption", UCIState::SETOPTION },
        { "register", UCIState::REGISTER },
        { "ucinewgame", UCIState::UCINEWGAME },
        { "position", UCIState::POSITION },
        { "go", UCIState::GO },
        { "stop", UCIState::STOP },
        { "ponderhit", UCIState::PONDERHIT },
        { "quit", UCIState::QUIT }
    };

    std::string engine_id = "marks_chess_engine_version_1.0.0";
    bool debug_enabled = false;
};

#endif // UCIStateMachine_H