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
    const GameState& getLatestGameState() const;
    Piece getPieceAtSourceSquare(const Move&) const;
    Piece getPieceAtDestinationSquare(const Move&) const;
    void update_game_state(const Move& move, GameState& to_update) const;

    Rules _rules;

private:

    std::vector<GameState> _game_state;
    std::vector<Move> _moves;
};
