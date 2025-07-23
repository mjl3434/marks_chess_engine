#include "UCICommand.h"

#include "ChessEngine.h"

void DebugCommand::doCommand(ChessEngine& engine) {
    engine.doDebugCommand(*this);
}

void UciCommand::doCommand(ChessEngine& engine) {
    engine.doUciCommand(*this);
}

void IsReadyCommand::doCommand(ChessEngine& engine) {
    engine.doIsReadyCommand(*this);
}

void UciNewGameCommand::doCommand(ChessEngine& engine) {
    engine.doUciNewGameCommand(*this);
}

void SetOptionCommand::doCommand(ChessEngine& engine) {
    engine.doSetOptionCommand(*this);
}

void PositionCommand::doCommand(ChessEngine& engine) {
    engine.doPositionCommand(*this);
}

void GoCommand::doCommand(ChessEngine& engine) {
    engine.doGoCommand(*this);
}

void StopCommand::doCommand(ChessEngine& engine) {
    engine.doStopCommand(*this);
}

void PonderHitCommand::doCommand(ChessEngine& engine) {
    engine.doPonderHitCommand(*this);
}

void QuitCommand::doCommand(ChessEngine& engine) {
    engine.doQuitCommand(*this);
}