#pragma once

#include <list>
#include <string>
#include <utility>

#include "Game.h"

class ChessEngine
{
  public:
    ChessEngine();
    void setDebug(bool enabled);
    void doUciCommand(void);
    void printSupportedOptions(void);
    void respondWhenReady(void);
    void setConfigurationOption(const std::string&, const std::string&);
    void startNewGame(void);
    void setUpPosition(std::string fen, std::list<Move>);
    void startCalculating(
            std::list<Move> searchMoves,
            bool ponder,
            bool infinite,
            int32_t movetime,
            int32_t wtime,
            int32_t btime,
            int32_t winc,
            int32_t binc,
            int16_t movestogo,
            int16_t nodes,
            int16_t mate);
    void stopCalculating(void);
    void ponderHit(void);
    void quit(void);
    void setUpBoardFromFen(const std::string&);

  private:
    bool debug_enabled = false;
    std::string engine_name = "Mark's Chess Engine Version 1.0";
    std::string author = "Mark Larwill";
    Game game;
};
