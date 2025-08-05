#pragma once

#include <vector>

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
    bool isDrawByThreefoldRepetition(const GameState& game_state, const position_hash_t& previous_positions) const;
    bool isDrawByThreefoldRepetition(const GameState& game_state) const;
    void addToRepetitionTable(std::size_t hash, position_hash_t& positions) const;
    void addToRepetitionTable(std::size_t hash);
    void removeFromRepetitionTable(std::size_t hash, position_hash_t& positions) const;
    void removeFromRepetitionTable(std::size_t hash);
    position_hash_t getGamePositions() const;

    // Helper functions
    const GameState& getLatestGameState() const;
    Piece getPieceAtSourceSquare(const Move&) const;
    Piece getPieceAtDestinationSquare(const Move&) const;

    Rules _rules;

private:

    std::unordered_map<std::size_t, int> _unique_positions;
    std::vector<GameState> _game_state;
    std::vector<Move> _moves;
};
