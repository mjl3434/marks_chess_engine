#include "ChessGame.h"

/**
 * @brief FIXME:
 * Assumptions:
 *     - Previous code has already checked that the move is legal
 */
void
ChessGame::doMove(const Move& move)
{
    Move new_move = move;
    GameState new_game_state = getLatestGameState();

    // Find the piece we're moving
    new_move.piece = getPieceAtSourceSquare(new_move);

    // Find the piece we're capturing, if any
    new_move.captured_piece = getPieceAtDestinationSquare(new_move);

    // Make the move on the game board
    new_game_state.board[new_move.destination_rank][new_move.destination_file].piece = new_move.piece;
    new_game_state.board[new_move.source_rank][new_move.source_file].piece = Piece::EMPTY;

    // Update the game state, which may have changed due to the move
    update_game_state(new_game_state);

    // Now update our list of moves, and the game state
    _moves.push_back(new_move);
    _game_state.push_back(new_game_state);
}

// FIXME: Do we want to return the move that was undone? This could be useful for un-doing and re-doing moves
void
ChessGame::undoMove()
{
    if (_moves.empty())
        return;

    // Remove the last move and game state
    _moves.pop_back();
    _game_state.pop_back();
}

void
ChessGame::update_game_state(GameState& to_update)
{
    // FIXME: Implement this

    // Update castling rights

    // Update en-passant target square

    // If no pawn move, or capture increment the halfmove clock (for fifty-move rule)

    // If black just moved, increment the fullmove counter
}

Piece
ChessGame::getPieceAtSourceSquare(const Move& move) const
{
    const GameState& current_game_state = _game_state.back();
    return current_game_state.board[move.source_rank][move.source_file].piece;
}

Piece
ChessGame::getPieceAtDestinationSquare(const Move& move) const
{
    const GameState& current_game_state = _game_state.back();
    return current_game_state.board[move.destination_rank][move.destination_file].piece;
}

const GameState&
ChessGame::getLatestGameState() const
{
    return _game_state.back();
}
