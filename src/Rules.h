#pragma once

#include <list>

#include "Common.h"
#include "GameState.h"

// This class is completly stateless
class Rules
{
public:
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
    bool isGameOver(const GameState& state);
    bool isDrawByFiftyMoveRule(const GameState& state);
    bool isDrawByThreefoldRepetition(const GameState& state);
    bool isDrawByInsufficientMaterial(const GameState& state);
    bool isSquareOccupied(int8_t rank, int8_t file, const GameState& state);
    bool isSquareUnderAttack(int8_t rank, int8_t file, const GameState& state);
    bool isKingSideCastle(const Move& move);
    bool isQueenSideCastle(const Move& move);

    void tryMoveOnStateCopy(const Move&, GameState& game_state);
    /*
    bool isWhite(Piece piece);
    bool isPawn(Piece piece);
    */
private:
    bool isKingInCheckAfterMove(const Move& move, const GameState& game_state);
};