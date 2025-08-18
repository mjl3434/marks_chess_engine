#include "GameState.h"

#include "Hash.h"

Piece
GameState::getPieceAtSourceSquare(const Move& move) const
{
    return board[move.source_rank-1][move.source_file-1].piece;
}

Piece
GameState::getPieceAtDestinationSquare(const Move& move) const
{
    return board[move.destination_rank-1][move.destination_file-1].piece;
}

/**
 * @brief Update any game state variables that are changed by this move
 * Assumes:
 *     - The move passed in is legal
 *     - You will check for end game conditions independently
 */
void
GameState::updateGameState(const Move& move)
{
    // Update castling rights
    if (move.piece == Piece::WHITE_ROOK) {
        if (move.source_rank == 1 && move.source_file == 1) {
            _white_queenside_castle_allowed = false;
        } else if (move.source_rank == 1 && move.source_file == 8) {
            _white_kingside_castle_allowed = false;
        }
    } else if (move.piece == Piece::BLACK_ROOK) {
        if (move.source_rank == 8 && move.source_file == 1) {
            _black_queenside_castle_allowed = false;
        } else if (move.source_rank == 8 && move.source_file == 8) {
            _black_kingside_castle_allowed = false;
        }
    } else if (move.piece == Piece::WHITE_KING) {
        _white_kingside_castle_allowed = false;
        _white_queenside_castle_allowed = false;
    } else if (move.piece == Piece::BLACK_KING) {
        _black_kingside_castle_allowed = false;
        _black_queenside_castle_allowed = false;
    }

    // Update en-passant target square
    else if (move.piece == Piece::WHITE_PAWN) {
        if (move.source_rank == 2 && move.destination_rank == 4) {
            _en_passant_target_square_rank = 3;
            _en_passant_target_square_file = move.destination_file;
        }
    } else if (move.piece == Piece::BLACK_PAWN) {
        if (move.source_rank == 7 && move.destination_rank == 5) {
            _en_passant_target_square_rank = 6;
            _en_passant_target_square_file = move.destination_file;
        }
    }

    // If no pawn move, and no capture, increment the halfmove clock (for fifty-move rule)
    if (move.captured_piece == Piece::EMPTY) {
        if (move.piece != Piece::WHITE_PAWN && move.piece != Piece::BLACK_PAWN) {
            _halfmove_clock++;
        }
    }

    // If black just moved, increment the fullmove counter
    if (_current_player == Player::BLACK) {
        _num_moves++;
    }

    // Update the game state hash, used to detect three fold repitition
    setGameStateHash();

    // In order to avoid adding a circular dependency the user should check for
    // end of game conditions immediately after this function is called.

    // Switch players
    _current_player = (_current_player == Player::WHITE) ? Player::BLACK : Player::WHITE;
}

void
GameState::setGameStateHash()
{
    std::size_t seed = 0;

    // Hash the board
    for (int rank = 1; rank <= 8; rank++) {
        for (int file = 1; file <= 8; file++) {
            hash_combine(seed, static_cast<uint32_t>(board[rank-1][file-1].piece));
        }
    }

    // Hash player to move
    hash_combine(seed, static_cast<uint32_t>(_current_player));

    // Hash castling rights
    hash_combine(seed, _white_kingside_castle_allowed);
    hash_combine(seed, _white_queenside_castle_allowed);
    hash_combine(seed, _black_kingside_castle_allowed);
    hash_combine(seed, _black_queenside_castle_allowed);

    // Hash en passant target square
    hash_combine(seed, _en_passant_target_square_rank);
    hash_combine(seed, _en_passant_target_square_file);

    _game_state_hash = seed;
}