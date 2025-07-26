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

void
GameState::updateGameState(const Move& move)
{
    // FIXME: Implement this

    // Update castling rights

    // Update en-passant target square

    // If no pawn move, or capture increment the halfmove clock (for fifty-move rule)

    // If black just moved, increment the fullmove counter
    if (_current_player == Player::BLACK) {
        _num_moves++;
    }

    // Switch players
    _current_player = (_current_player == Player::WHITE) ? Player::BLACK : Player::WHITE;
}