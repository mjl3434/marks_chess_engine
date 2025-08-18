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

    bool destinationContainsSamePlayer(const Move& move, const GameState& state) const;
    bool isBlack(const Piece& piece) const;
    bool isWhite(const Piece& piece) const;
    bool isCheckmate(const GameState& state) const;
    bool isDrawByFiftyMoveRule(const GameState& state) const;
    bool isDrawByInsufficientMaterial(const GameState& state) const;
    bool isKingInCheck(const GameState& state) const;
    bool isKingInCheckAfterMove(const Move& move, const GameState& game_state) const;
    bool isKingSideCastle(const Move& move) const;
    bool isQueenSideCastle(const Move& move) const;
    bool isLegalMove(const Move& move, const GameState& state) const;
    bool isSameColor(const Piece& piece1, const Piece& piece2) const;
    bool isSquareOccupied(int8_t rank, int8_t file, const GameState& state) const;
    bool isSquareOccupiedBy(int8_t rank, int8_t file, const GameState& state, Player opponent) const;
    bool isSquareUnderAttackBy(int8_t rank, int8_t file, const GameState& state, Player opponent) const;
    bool isSquareUnderAttackByBishopOrQueen(int8_t rank, int8_t file, const GameState& state, Player opponent) const;
    bool isSquareUnderAttackByKing(int8_t rank, int8_t file, const GameState& state, Player opponent) const;
    bool isSquareUnderAttackByKnight(int8_t rank, int8_t file, const GameState& state, Player opponent) const;
    bool isSquareUnderAttackByPawn(int8_t rank, int8_t file, const GameState& state, Player opponent) const;
    bool isSquareUnderAttackByRookOrQueen(int8_t rank, int8_t file, const GameState& state, Player opponent) const;
    bool isStalemate(const GameState& state) const;
    bool isValidBishopMove(const Move&, const GameState& state) const;
    bool isValidKingMove(const Move&, const GameState& state) const;
    bool isValidKnightMove(const Move&, const GameState& state) const;
    bool isValidPawnMove(const Move&, const GameState& state) const;
    bool isValidQueenMove(const Move&, const GameState& state) const;
    bool isValidRookMove(const Move&, const GameState& state) const;

    GameResult checkForGameEndings(const GameState& state, position_hash_t& repetition_table) const;
    std::list<Move> generateLegalMovesForCurrentPlayer(const GameState& game_state) const;
    std::list<Move> generateLegalMovesForPieceAt(uint8_t rank, uint8_t file,
            const GameState& game_state, const Piece& piece) const;
    std::list<Move> generatePossibleMovesForPawnAt(uint8_t rank, uint8_t file, const GameState& game_state, const Piece& piece) const;
    std::list<Move> generatePossibleMovesForBishopAt(uint8_t rank, uint8_t file, const GameState& game_state, const Piece& piece) const;
    std::list<Move> generatePossibleMovesForKnightAt(uint8_t rank, uint8_t file, const GameState& game_state, const Piece& piece) const;
    std::list<Move> generatePossibleMovesForRookAt(uint8_t rank, uint8_t file, const GameState& game_state, const Piece& piece) const;
    std::list<Move> generatePossibleMovesForQueenAt(uint8_t rank, uint8_t file, const GameState& game_state, const Piece& piece) const;
    std::list<Move> generatePossibleMovesForKingAt(uint8_t rank, uint8_t file, const GameState& game_state, const Piece& piece) const;
    void tryMoveOnStateCopy(const Move&, GameState& game_state);
private:
    const ChessGame& parent_game;
};