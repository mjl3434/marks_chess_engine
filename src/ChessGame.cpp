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

    // Now update our list of moves
    _moves.push_back(new_move);
    _game_state.push_back(new_game_state);

    // Update the count of how many times this unique position has been seen
    _unique_positions[new_game_state._game_state_hash]++;
}

void
ChessGame::undoMove()
{
    if (_moves.empty())
        return;

    // Decrement the count of how many times this unique position has been seen
    int last_position_hash = _game_state.back()._game_state_hash;
    if (_unique_positions[last_position_hash] > 0) {
        _unique_positions[last_position_hash]--;
    }
    // If it's the last one then remove the entry alltogether
    if (_unique_positions[last_position_hash] == 0) {
        _unique_positions.erase(last_position_hash);
    }

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

    // FIXME: It's really confusing on how to best detect three fold repitition without having a ChessGame
    // Perhaps we need to keep something like the GameState that's for the game itself so we can have one
    // copy for the real game, and another copy for the game being experimented on
}


bool
ChessGame::isDrawByFiftyMoveRule(const GameState& state) const
{

}

bool 
ChessGame::isDrawByThreefoldRepetition(const GameState& state) const
{
    for (const auto& state : _game_state) {
        std::size_t key = state._game_state_hash;
        if (_unique_positions[key] >= 3) {
            return true;
        }
    }
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
