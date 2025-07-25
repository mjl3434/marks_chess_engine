#pragma once

#include "Common.h"
#include "GameState.h"
#include "Rules.h"

class ChessGame
{
public:
    void doMove(const Move&);
    void undoMove();

    // Helper functions
    const GameState&getLatestGameState();
    Piece getPieceAtSourceSquare(const Move&);
    Piece getPieceAtDestinationSquare(const Move&);

private:

    std::vector<GameState> _game_state;
    std::vector<Move> _moves;
    Rules _rules;
};
