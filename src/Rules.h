#pragma once

#include <list>

#include "Common.h"
#include "GameState.h"

class ChessGame;    // Forward declaration

// This class is completly stateless (except for a single reference to the parent class)
class Rules
{
public:
    Rules(const ChessGame& game);

    bool isLegalMove(const Move& move, const GameState& state);
    bool isValidPawnMove(const Move&, const GameState& state);
    bool isValidBishopMove(const Move&, const GameState& state);
    bool isValidKnightMove(const Move&, const GameState& state);
    bool isValidRookMove(const Move&, const GameState& state);
    bool isValidQueenMove(const Move&, const GameState& state);
    bool isValidKingMove(const Move&, const GameState& state);
    bool isKingInCheck(const GameState& state);
    bool isCheckmate(const GameState& state);
    bool isStalemate(const GameState& state);
    bool isGameOver(const GameState& state, position_hash_t& repetition_table);
    bool isDrawByInsufficientMaterial(const GameState& state);
    bool isDrawByFiftyMoveRule(const GameState& state) const;
    bool isSquareOccupied(int8_t rank, int8_t file, const GameState& state);
    bool isSquareUnderAttack(int8_t rank, int8_t file, const GameState& state);
    bool isKingSideCastle(const Move& move);
    bool isQueenSideCastle(const Move& move);
    bool isSameColor(const Piece& piece1, const Piece& piece2);
    bool isWhite(const Piece& piece);
    bool isBlack(const Piece& piece);

    void tryMoveOnStateCopy(const Move&, GameState& game_state);
    bool isKingInCheckAfterMove(const Move& move, const GameState& game_state);
private:
    const ChessGame& parent_game;
};