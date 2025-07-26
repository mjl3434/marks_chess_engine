#include "GameState.h"

Piece
GameState::getPieceAtSourceSquare(const Move& move)
{
    return board[move.source_rank][move.source_file].piece;
}

Piece
GameState::getPieceAtDestinationSquare(const Move& move)
{
    return board[move.destination_rank][move.destination_file].piece;
}

/**
 * @brief Update any game state variables that are changed by this move
 * Assumes:
 *     - The move passed in is legal
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

    // FIXME: Do we want to check for these things here? Should we add them to game state?
    //     - checkmate
    //     - stalemate
    //     - draw by fifty-move rule
    //     - draw by threefold repitition
    //     - draw by insufficient material

    // Switch players
    _current_player = (_current_player == Player::WHITE) ? Player::BLACK : Player::WHITE;
}