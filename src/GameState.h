#pragma once

#include <cstdint>

#include "Common.h"

class GameState
{
public:
    Piece getPieceAtSourceSquare(const Move& move) const;
    Piece getPieceAtDestinationSquare(const Move& move) const;
    void updateGameState(const Move& move);
    void setGameStateHash();

    Square board[8][8]; // [num_rows][num_columns]
    Player _current_player;

    // These fields are needed by games started with FEN, where we don't have
    // the full moves list and can't determine some of the game state, however
    // they're still useful regardless so we always keep them updated.

    int _halfmove_clock = 0; // For calculating fifty-move rule
    int _num_moves = 0; // Num prior full moves from game started w/o moves list
    uint8_t _en_passant_target_square_rank = 0;
    uint8_t _en_passant_target_square_file = 0;
    bool _two_square_pawn_push_just_occured = false;
    bool _white_kingside_castle_allowed = true;
    bool _white_queenside_castle_allowed = true;
    bool _black_kingside_castle_allowed = true;
    bool _black_queenside_castle_allowed = true;
    std::size_t _game_state_hash;
};