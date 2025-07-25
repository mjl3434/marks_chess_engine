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

private:

    void update_game_state(GameState& to_update);

    std::vector<GameState> _game_state;
    std::vector<Move> _moves;
    Rules _rules;
};
