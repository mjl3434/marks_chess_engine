#include "ChessGame.h"

ChessGame::ChessGame()
    : _rules(*this)
{
}

/**
 * @brief This function performs a move, and stores the move and state
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

    // Update the game state as a result of the move
    new_game_state.updateGameState(new_move);

    // FIXME: Do we need to update the unique_positions here

    // Now update our list of moves, and the game state
    _moves.push_back(new_move);
    _game_state.push_back(new_game_state);
}

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
ChessGame::tryMoveOnStateCopy(const Move& move, GameState& game_state) const
{
    // Find the piece we're moving
    Piece pice_moved = game_state.getPieceAtSourceSquare(move);

    // Find the piece we're capturing, if any
    Piece captured_piece = game_state.getPieceAtDestinationSquare(move);

    // Make the move on the game board
    game_state.board[move.destination_rank-1][move.destination_file-1].piece = pice_moved;
    game_state.board[move.source_rank-1][move.source_file-1].piece = Piece::EMPTY;

    // Update the game state as a result of the move
    game_state.updateGameState(move);
}


bool
ChessGame::isDrawByFiftyMoveRule(const GameState& state) const
{
/*
for (const auto& state : _game_state) {
    std::string key = state.positionKey(); // Implement this!
    position_counts[key]++;
    if (position_counts[key] >= 3) {
        return true; // Draw by threefold repetition
    }
}
return false;
*/

    // FIXME: implement this
    return false;
}

bool 
ChessGame::isDrawByThreefoldRepetition(const GameState& state) const
{
    // FIXME: implement this
    return false;
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
