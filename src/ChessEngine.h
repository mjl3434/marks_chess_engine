#pragma once

#include <string>

#include "Game.h"

class ChessEngine
{
  public:
    ChessEngine();
    bool gameInProgress();
    void startNewGame();
    bool isValidFen(const std::string&);
    bool isValidAlgebraicNotation(const std::string&);
    void setUpBoardFromFen(const std::string&);
    void setUpBoardInStartPosition(void);

  private:
    bool game_in_progress = false;
    Game game;
};
