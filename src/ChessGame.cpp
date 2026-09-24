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
    new_game_state.board[new_move.destination_rank-1][new_move.destination_file-1].piece = new_move.piece;
    new_game_state.board[new_move.source_rank-1][new_move.source_file-1].piece = Piece::EMPTY;

    // Handle castling - need to move the rook too
    if (_rules.isKingSideCastle(new_move)) {
        // Move rook from h1/h8 to f1/f8
        int rank = (new_move.piece == Piece::WHITE_KING) ? 1 : 8;
        Piece rook = (new_move.piece == Piece::WHITE_KING) ? Piece::WHITE_ROOK : Piece::BLACK_ROOK;
        new_game_state.board[rank-1][8-1].piece = Piece::EMPTY;     // Clear h1/h8
        new_game_state.board[rank-1][5-1].piece = rook;             // Place rook on f1/f8
    }
    else if (_rules.isQueenSideCastle(new_move)) {
        // Move rook from a1/a8 to d1/d8
        int rank = (new_move.piece == Piece::WHITE_KING) ? 1 : 8;
        Piece rook = (new_move.piece == Piece::WHITE_KING) ? Piece::WHITE_ROOK : Piece::BLACK_ROOK;
        new_game_state.board[rank-1][1-1].piece = Piece::EMPTY;     // Clear a1/a8
        new_game_state.board[rank-1][3-1].piece = rook;             // Place rook on d1/d8
    }

    // Update the game state as a result of the move
    new_game_state.updateGameState(new_move);

    // Now update our list of moves
    _moves.push_back(new_move);
    _game_state.push_back(new_game_state);

    // Update the count of how many times this unique position has been seen
    addToRepetitionTable(new_game_state._game_state_hash);
}

void
ChessGame::undoMove()
{
    if (_moves.empty())
        return;

    removeFromRepetitionTable(_game_state.back()._game_state_hash);

    // Remove the last move and game state
    _moves.pop_back();
    _game_state.pop_back();
}

void
ChessGame::tryMoveOnStateCopy(const Move& move, GameState& game_state) const
{
    // Find the piece we're moving
    Piece piece_moved = game_state.getPieceAtSourceSquare(move);

    // Find the piece we're capturing, if any
    Piece captured_piece = game_state.getPieceAtDestinationSquare(move);

    // Make the move on the game board
    game_state.board[move.destination_rank-1][move.destination_file-1].piece = piece_moved;
    game_state.board[move.source_rank-1][move.source_file-1].piece = Piece::EMPTY;

    // Handle castling - need to move the rook too
    if (_rules.isKingSideCastle(move)) {
        // Move rook from h1/h8 to f1/f8
        int rank = (piece_moved == Piece::WHITE_KING) ? 1 : 8;
        Piece rook = (piece_moved == Piece::WHITE_KING) ? Piece::WHITE_ROOK : Piece::BLACK_ROOK;
        game_state.board[rank-1][8-1].piece = Piece::EMPTY;     // Clear h1/h8
        game_state.board[rank-1][5-1].piece = rook;             // Place rook on f1/f8
    }
    else if (_rules.isQueenSideCastle(move)) {
        // Move rook from a1/a8 to d1/d8
        int rank = (piece_moved == Piece::WHITE_KING) ? 1 : 8;
        Piece rook = (piece_moved == Piece::WHITE_KING) ? Piece::WHITE_ROOK : Piece::BLACK_ROOK;
        game_state.board[rank-1][1-1].piece = Piece::EMPTY;     // Clear a1/a8
        game_state.board[rank-1][3-1].piece = rook;             // Place rook on d1/d8
    }

    // Update the game state as a result of the move
    game_state.updateGameState(move);
}

/**
 * @brief Checks if the hash in the current GameState has occured at least 3 times
 * Assumes: You have already added the new hash to the repetition_table
 */
bool
ChessGame::isDrawByThreefoldRepetition(const GameState& game_state, const position_hash_t& repetition_table) const
{
    if (repetition_table.at(game_state._game_state_hash) >= 3) {
        return true;
    }
    return false;
}

bool 
ChessGame::isDrawByThreefoldRepetition(const GameState& state) const
{
    return isDrawByThreefoldRepetition(state, _unique_positions);
}


void
ChessGame::addToRepetitionTable(std::size_t hash)
{
    addToRepetitionTable(hash, _unique_positions);
}

void
ChessGame::addToRepetitionTable(std::size_t hash, position_hash_t& positions) const
{
    // Insert and increment count of how many times this unique position has been seen
    positions[hash]++;
}

void
ChessGame::removeFromRepetitionTable(std::size_t hash)
{
    removeFromRepetitionTable(hash, _unique_positions);
}

void
ChessGame::removeFromRepetitionTable(std::size_t hash, position_hash_t& positions) const
{
    // Decrement the count of how many times this unique position has been seen
    if (positions[hash] > 0) {
        positions[hash]--;
    }
    // If it's the last one then remove the entry
    if (positions[hash] == 0) {
        positions.erase(hash);
    }
}

position_hash_t ChessGame::getGamePositions() const
{
    return _unique_positions;
}

Piece
ChessGame::getPieceAtSourceSquare(const Move& move) const
{
    const GameState& current_game_state = _game_state.back();
    return current_game_state.board[move.source_rank-1][move.source_file-1].piece;
}

Piece
ChessGame::getPieceAtDestinationSquare(const Move& move) const
{
    const GameState& current_game_state = _game_state.back();
    return current_game_state.board[move.destination_rank-1][move.destination_file-1].piece;
}

const GameState&
ChessGame::getLatestGameState() const
{
    return _game_state.back();
}
