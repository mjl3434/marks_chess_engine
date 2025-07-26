#pragma once

#include "Common.h"
#include "GameState.h"
#include "Rules.h"

class ChessGame
{
public:

    ChessGame();
    void doMove(const Move&);
    void undoMove();
    void tryMoveOnStateCopy(const Move& move, GameState& game_state) const;
    bool isDrawByFiftyMoveRule(const GameState& state) const;
    bool isDrawByThreefoldRepetition(const GameState& state) const;

    // Helper functions
    const GameState& getLatestGameState() const;
    Piece getPieceAtSourceSquare(const Move&) const;
    Piece getPieceAtDestinationSquare(const Move&) const;

    Rules _rules;

private:

    std::unordered_map<std::string, int> unique_positions;
    std::vector<GameState> _game_state;
    std::vector<Move> _moves;
};
