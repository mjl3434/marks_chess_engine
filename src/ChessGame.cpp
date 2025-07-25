#include "ChessGame.h"

Piece
ChessGame::getPieceAtSourceSquare(const Move& move)
{
    const GameState& current_game_state = _game_state.back();
    return current_game_state.board[move.source_rank][move.source_file].piece;
}

Piece
ChessGame::getPieceAtDestinationSquare(const Move& move)
{
    const GameState& current_game_state = _game_state.back();
    return current_game_state.board[move.destination_rank][move.destination_file].piece;
}

/**
 * @brief Simply moves a piece, and adds the move to the moves list
 * Assumptions:
 *     - Previous code has already checked that the move is legal
 */
void
ChessGame::doMove(const Move& move)
{
    Move player_move = move;
    GameState& current_game_state = _game_state.back();

    // Populate the move with the piece found at the source square on the board
    player_move.piece = getPieceAtSourceSquare(player_move);

    // Populate the captured piece, if any (if not destination will simply be empty)
    player_move.captured_piece = getPieceAtDestinationSquare(player_move);

    // Make the move on the game board
    current_game_state.board[player_move.destination_rank][player_move.destination_file].piece = player_move.piece;
    current_game_state.board[player_move.source_rank][player_move.source_file].piece = Piece::EMPTY;

    _moves.push_back(move);
}

void
ChessGame::undoMove()
{

}

const GameState&
ChessGame::getLatestGameState()
{
    return _game_state.back();
}
